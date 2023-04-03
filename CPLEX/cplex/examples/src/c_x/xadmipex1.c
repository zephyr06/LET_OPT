/* --------------------------------------------------------------------------
 * File: xadmipex1.c
 * Version 22.1.0
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of IBM
 * 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
 * Copyright IBM Corporation 1997, 2022. All Rights Reserved.
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with
 * IBM Corp.
 * --------------------------------------------------------------------------
 */

/* xadmipex1.c - Use the node, branch and solve callbacks
                for optimizing a MIP problem with cplexx.h */

/* To run this example, command line arguments are required:
       xadmipex1 [-r] filename
   where 
       filename  Name of the file, with .mps, .lp, or .sav
                 extension, and a possible additional .gz 
                 extension.
       -r        Indicates that callbacks will refer to the
                 presolved model.
   Example:
       admipex1  mexample.mps
       admipex1 -r mexample.mps */

/* Bring in the CPLEX function declarations and the C library 
   header file stdio.h with the following single include */

#include <ilcplex/cplexx.h>

/* Bring in the declarations for the string and character functions,
   malloc, fabs, and floor */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Declarations for functions in this program */


static int CPXPUBLIC 
   usersolve      (CPXCENVptr env, void *cbdata, int wherefrom,
                   void *cbhandle, int *useraction_p);
static int CPXPUBLIC
   usersetbranch  (CPXCENVptr env, void *cbdata, int wherefrom,
                   void *cbhandle, int brtype, CPXDIM sos, int nodes,
                   CPXDIM bdcnt, const CPXDIM *nodebeg,
                   const CPXDIM *indices, const char *lu, const double *bd,
                   const double *nodeest, int *useraction_p);
static int CPXPUBLIC 
   userselectnode (CPXCENVptr env, void *cbdata, int wherefrom,
                   void *cbhandle, CPXCNT *nodeid_p,
                   int *useraction_p);

static void
   free_and_null (char **ptr),
   usage         (char *progname);



int
main (int  argc,
      char *argv[])
{
   int status = 0;

   /* Declare and allocate space for the variables and arrays where
      we will store the optimization results, including the status, 
      objective value, and variable values */
   
   int    solstat;
   double objval;
   double *x = NULL;
   
   CPXENVptr env = NULL;
   CPXLPptr  lp = NULL;

   CPXDIM j;
   CPXDIM cur_numcols;
   int wantorig = 1;
   int nameind = 1;

   /* Check the command line arguments */

   if ( argc != 2 ) {
      if ( argc != 3         ||
           argv[1][0] != '-' ||
           argv[1][1] != 'r'   ) {
         usage (argv[0]);
         goto TERMINATE;
      }
      wantorig = 0;
      nameind = 2;
   }

   /* Initialize the CPLEX environment */

   env = CPXXopenCPLEX (&status);

   /* If an error occurs, the status value indicates the reason for
      failure.  A call to CPXXgeterrorstring will produce the text of
      the error message.  Note that CPXXopenCPLEX produces no
      output, so the only way to see the cause of the error is to use
      CPXXgeterrorstring.  For other CPLEX routines, the errors will
      be seen if the CPXPARAM_ScreenOutput parameter is set to CPX_ON */

   if ( env == NULL ) {
      char errmsg[CPXMESSAGEBUFSIZE];
      fprintf (stderr, "Could not open CPLEX environment.\n");
      CPXXgeterrorstring (env, status, errmsg);
      fprintf (stderr, "%s", errmsg);
      goto TERMINATE;
   }

   /* Turn on output to the screen */

   status = CPXXsetintparam (env, CPXPARAM_ScreenOutput, CPX_ON);
   if ( status ) {
      fprintf (stderr, 
               "Failure to turn on screen indicator, error %d.\n",
               status);
      goto TERMINATE;
   }

   /* Create the problem, using the filename as the problem name */

   lp = CPXXcreateprob (env, &status, argv[nameind]);

   /* A returned pointer of NULL may mean that not enough memory
      was available or there was some other problem.  In the case of
      failure, an error message will have been written to the error
      channel from inside CPLEX.  In this example, the setting of
      the parameter CPXPARAM_ScreenOutput causes the error message to
      appear on stdout.  Note that most CPLEX routines return
      an error code to indicate the reason for failure */

   if ( lp == NULL ) {
      fprintf (stderr, "Failed to create LP.\n");
      goto TERMINATE;
   }

   /* Now read the file, and copy the data into the created lp */

   status = CPXXreadcopyprob (env, lp, argv[nameind], NULL);
   if ( status ) {
      fprintf (stderr,
               "Failed to read and copy the problem data.\n");
      goto TERMINATE;
   }

   /* Set up to use MIP callbacks */

   status = CPXXsetnodecallbackfunc (env, userselectnode, NULL)  ||
            CPXXsetbranchcallbackfunc (env, usersetbranch, NULL) ||
            CPXXsetsolvecallbackfunc (env, usersolve, NULL);

   if ( wantorig ) {
      /* Assure linear mappings between the presolved and original
         models */

      status = CPXXsetintparam (env, CPXPARAM_Preprocessing_Reformulations,
                                CPX_PREREFORM_NONE);
      if ( status )  goto TERMINATE;

      /* Let MIP callbacks work on the original model */

      status = CPXXsetintparam (env, CPXPARAM_MIP_Strategy_CallbackReducedLP,
                                CPX_OFF);
      if ( status )  goto TERMINATE;
   }

   /* Set MIP log interval to 1 */

   status = CPXXsetcntparam (env, CPXPARAM_MIP_Interval, 1);
   if ( status )  goto TERMINATE;

   /* Turn on traditional search for use with control callbacks */

   status = CPXXsetintparam (env, CPXPARAM_MIP_Strategy_Search,
                             CPX_MIPSEARCH_TRADITIONAL);
   if ( status )  goto TERMINATE;



   /* Optimize the problem and obtain solution */

   status = CPXXmipopt (env, lp);
   if ( status ) {
      fprintf (stderr, "Failed to optimize MIP.\n");
      goto TERMINATE;
   }

   solstat = CPXXgetstat (env, lp);
   printf ("Solution status %d.\n", solstat);

   status  = CPXXgetobjval (env, lp, &objval);
   if ( status ) {
      fprintf (stderr,"Failed to obtain objective value.\n");
      goto TERMINATE;
   }

   printf ("Objective value %.10g\n", objval);

   cur_numcols = CPXXgetnumcols (env, lp);

   /* Allocate space for solution */

   x = malloc (cur_numcols * sizeof (*x));
   if ( x == NULL ) {
      fprintf (stderr, "No memory for solution values.\n");
      goto TERMINATE;
   }

   status = CPXXgetx (env, lp, x, 0, cur_numcols-1);
   if ( status ) {
      fprintf (stderr, "Failed to obtain solution.\n");
      goto TERMINATE;
   }

   /* Write out the solution */ 

   for (j = 0; j < cur_numcols; j++) {
      if ( fabs (x[j]) > 1e-10 ) {
         printf ( "Column %d:  Value = %17.10g\n", j, x[j]);
      }
   }


TERMINATE:

   /* Free the solution vector */

   free_and_null ((char **) &x);

   /* Free the problem as allocated by CPXXcreateprob and
      CPXXreadcopyprob, if necessary */

   if ( lp != NULL ) {
      int xstatus = CPXXfreeprob (env, &lp);

      if ( !status ) status = xstatus;
   }

   /* Free the CPLEX environment, if necessary */

   if ( env != NULL ) {
      int xstatus = CPXXcloseCPLEX (&env);

      if ( !status ) status = xstatus;
   }
     
   return (status);

} /* END main */


/* This simple routine frees up the pointer *ptr, and sets *ptr to 
   NULL */

static void
free_and_null (char **ptr)
{
   if ( *ptr != NULL ) {
      free (*ptr);
      *ptr = NULL;
   }
} /* END free_and_null */ 


static void
usage (char *progname)
{
   fprintf (stderr,
    "Usage: %s [-r] filename\n", progname);
   fprintf (stderr,
    "  filename   Name of a file, with .mps, .lp, or .sav\n");
   fprintf (stderr,
    "             extension, and a possible, additional .gz\n"); 
   fprintf (stderr,
    "             extension\n");
   fprintf (stderr,
    "  -r         Indicates that callbacks will refer to the\n");
   fprintf (stderr,
    "             presolved model\n");
} /* END usage */


static int CPXPUBLIC 
usersolve (CPXCENVptr env,
           void       *cbdata,
           int        wherefrom,
           void       *cbhandle,
           int        *useraction_p)
{
   int      status = 0;
   CPXCNT   nodecount;
   CPXLPptr nodelp;

   /* Silence compiler warning(s) about unused parameter(s). */

   (void)cbhandle;

   /* Initialize useraction to indicate no user action taken */

   *useraction_p = CPX_CALLBACK_DEFAULT;

   /* Get pointer to LP subproblem */

   status = CPXXgetcallbacknodelp (env, cbdata, wherefrom, &nodelp);
   if ( status )  goto TERMINATE;

   /* Find out what node is being processed */

   status = CPXXgetcallbackinfo (env, cbdata, wherefrom,
                                CPX_CALLBACK_INFO_NODE_COUNT_LONG,
                                &nodecount);
   if ( status )  goto TERMINATE;

   /* Solve initial node with primal, others with dual */

   if ( nodecount < 1 )  status = CPXXprimopt (env, nodelp);
   else                  status = CPXXdualopt (env, nodelp);

   /* If the solve was OK, set return to say optimization has
      been done in callback, otherwise return the CPLEX error
      code */

   if ( !status )  *useraction_p = CPX_CALLBACK_SET;

TERMINATE:

   return (status);

} /* END usersolve */


static int CPXPUBLIC
usersetbranch (CPXCENVptr   env,
               void         *cbdata,
               int          wherefrom,
               void         *cbhandle,
               int          brtype,
               CPXDIM       sos,
               int          nodecnt,
               CPXDIM       bdcnt,
               const CPXDIM *nodebeg,
               const CPXDIM *indices,
               const char   *lu,
               const double *bd,
               const double *nodeest,
               int          *useraction_p)   
{
   int status = 0;
 
   CPXDIM   j, bestj = -1;
   CPXDIM   cols;
   double   maxobj = -CPX_INFBOUND;
   double   maxinf = -CPX_INFBOUND;
   double   xj_inf;
   double   xj_lo;
   double   objval;
   double   *x   = NULL;
   double   *obj = NULL;
   int      *feas = NULL;
 
   char     varlu[1];
   double   varbd[1];
   CPXCNT   seqnum1, seqnum2;
 
   CPXCLPptr lp;

   /* Silence compiler warning(s) about unused parameter(s). */

   (void)cbhandle;
   (void)brtype;
   (void)nodecnt;
   (void)bdcnt;
   (void)nodebeg;
   (void)indices;
   (void)lu;
   (void)bd;
   (void)nodeest;

   /* Initialize useraction to indicate no user action taken */
 
   *useraction_p = CPX_CALLBACK_DEFAULT;
 
   /* If CPLEX is choosing an SOS branch, take it */
 
   if ( sos >= 0 )  return (status);
 
   /* Get pointer to the problem */
 
   status = CPXXgetcallbacklp (env, cbdata, wherefrom, &lp);
   if ( status ) {
      fprintf (stdout, "Can't get LP pointer.\n");
      goto TERMINATE;
   }
 
   cols = CPXXgetnumcols (env, lp);
   if ( cols <= 0 ) {
      fprintf (stdout, "Can't get number of columns.\n");
      status = CPXERR_CALLBACK;
      goto TERMINATE;
   }
 
   /* Get solution values and objective coefficients */
 
   x    = malloc (cols * sizeof (*x));
   obj  = malloc (cols * sizeof (*obj));
   feas = malloc (cols * sizeof (*feas));
   if ( x     == NULL ||
        obj   == NULL ||
        feas  == NULL   ) {
      fprintf (stdout, "Out of memory.");
      status = CPXERR_CALLBACK;
      goto TERMINATE;
   }
 
   status = CPXXgetcallbacknodex (env, cbdata, wherefrom, x, 0,
                                  cols-1);
   if ( status ) {
      fprintf (stdout, "Can't get node solution.");
      goto TERMINATE;
   }
 
   status = CPXXgetcallbacknodeobjval (env, cbdata, wherefrom,
                                       &objval);
   if ( status ) {
      fprintf (stdout, "Can't get node objective value.");
      goto TERMINATE;
   }
 
   status = CPXXgetobj (env, lp, obj, 0, cols-1);
   if ( status ) {
      fprintf (stdout, "Can't get obj.");
      goto TERMINATE;
   }
 
   status = CPXXgetcallbacknodeintfeas (env, cbdata, wherefrom, feas,
                                        0, cols-1);
   if ( status ) {
      fprintf (stdout,
               "Can't get variable feasible status for node.");
      goto TERMINATE;
   }
 
   /* Branch on var with largest objective coefficient among those
      with largest infeasibility */
 
   for (j = 0; j < cols; j++) {
      if ( feas[j] == CPX_INTEGER_INFEASIBLE ) {
         xj_inf = x[j] - floor (x[j]);
         if ( xj_inf > 0.5 )  xj_inf = 1.0 - xj_inf;
 
         if ( xj_inf >= maxinf                            &&
              (xj_inf > maxinf || fabs (obj[j]) >= maxobj)  ) {
            bestj  = j;
            maxinf = xj_inf; 
            maxobj = fabs (obj[j]);
         }
      }
   }
 
   /* If there weren't any eligible variables, take default branch */
 
   if ( bestj < 0 ) {
      goto TERMINATE;
   }
 
   /* Now set up node descriptions */
 
   xj_lo = floor (x[bestj]);
 
   /* Up node */
 
   varlu[0] = 'L';
   varbd[0] = xj_lo + 1;
   status = CPXXbranchcallbackbranchbds (env, cbdata, wherefrom,
                                        1, &bestj, varlu, varbd,
                                        objval, NULL, &seqnum1);
   if ( status )  goto TERMINATE;
 
   /* Down node */
 
   varlu[0] = 'U';
   varbd[0] = xj_lo;
 
   status = CPXXbranchcallbackbranchbds (env, cbdata, wherefrom,
                                         1, &bestj, varlu, varbd,
                                         objval, NULL, &seqnum2);
   if ( status )  goto TERMINATE;
 
   /* Set useraction to indicate a user-specified branch */
 
   *useraction_p = CPX_CALLBACK_SET;
 
TERMINATE:
 
   free_and_null ((char **) &x);
   free_and_null ((char **) &obj);
   free_and_null ((char **) &feas); 
   return (status);
 
} /* END usersetbranch */


static int CPXPUBLIC 
userselectnode (CPXCENVptr env,
                void       *cbdata,
                int        wherefrom,
                void       *cbhandle,
                CPXCNT     *nodenum_p,
                int        *useraction_p)
{
   int status = 0;

   CPXCNT thisnode;
   CPXCNT nodesleft;
   CPXCNT bestnode = 0;
   CPXCNT depth;
   CPXCNT maxdepth = -1;
   double siinf;
   double maxsiinf = 0.0;

   /* Silence compiler warning(s) about unused parameter(s). */

   (void)cbhandle;

   /* Initialize useraction to indicate no user node selection */

   *useraction_p = CPX_CALLBACK_DEFAULT;

   /* Choose the node with the largest sum of infeasibilities among
      those at the greatest depth */

   status = CPXXgetcallbackinfo (env, cbdata, wherefrom,
                                CPX_CALLBACK_INFO_NODES_LEFT_LONG,
                                &nodesleft);
   if ( status )  goto TERMINATE;

   for (thisnode = 0; thisnode < nodesleft; thisnode++) {
      status = CPXXgetcallbacknodeinfo (env, cbdata, wherefrom,
                                       thisnode,
                                       CPX_CALLBACK_INFO_NODE_DEPTH_LONG,
                                       &depth);
      if ( !status ) {
         status = CPXXgetcallbacknodeinfo (env, cbdata, wherefrom,
                                        thisnode,
                                        CPX_CALLBACK_INFO_NODE_SIINF,
                                        &siinf);
      }
      if ( status )  break;

      if ( (depth >= maxdepth)                   &&
           (depth > maxdepth || siinf > maxsiinf)  ) {
         bestnode = thisnode;
         maxdepth = depth;
         maxsiinf = siinf;
      }
   }

   *nodenum_p = bestnode;
   *useraction_p = CPX_CALLBACK_SET;

TERMINATE:

   return (status);

} /* END userselectnode */
