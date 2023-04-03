/* --------------------------------------------------------------------------
 * File: xadmipex2.c
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

/* xadmipex2.c - Use the heuristic callback for optimizing an all binary
                MIP problem using cplexx.h */

/* To run this example, command line arguments are required:
       xadmipex2 [-r] filename
   where 
       filename  Name of the file, with .mps, .lp, or .sav
                 extension, and a possible additional .gz 
                 extension
       -r        Indicates that callbacks will refer to the
                 presolved model
   Example:
       admipex2  mexample.mps
       admipex2 -r mexample.mps */

/* Bring in the CPLEX function declarations and the C library 
   header file stdio.h with the following single include */

#include <ilcplex/cplexx.h>

/* Bring in the declarations for the string and character functions, 
   malloc, and fabs */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Declarations for functions in this program */

static int CPXPUBLIC 
   rounddownheur (CPXCENVptr env, void *cbdata, int wherefrom,
                  void *cbhandle, double *objval_p, double *x,
                  int *checkfeas_p, int *useraction_p);

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
   if ( status != 0 ) {
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

   if ( CPXXgetnumcols (env, lp) != CPXXgetnumbin (env, lp) ) {
      fprintf (stderr, "Problem contains non-binary variables, exiting\n");
      goto TERMINATE;
   }

   /* Set parameters */

   if ( wantorig ) {
      /* Assure linear mappings between the presolved and original
         models */

      status = CPXXsetintparam (env, CPXPARAM_Preprocessing_Reformulations,
                                CPX_PREREFORM_INTERFERE_UNCRUSH);
      if ( status )  goto TERMINATE;

      /* Let MIP callbacks work on the original model */

      status = CPXXsetintparam (env, CPXPARAM_MIP_Strategy_CallbackReducedLP,
                                CPX_OFF);
      if ( status )  goto TERMINATE;
   }

   

   status = CPXXsetdblparam (env, CPXPARAM_MIP_Tolerances_MIPGap,
                             (double) 1e-6);
   if ( status )  goto TERMINATE;

   /* Turn on traditional search for use with control callbacks */

   status = CPXXsetintparam (env, CPXPARAM_MIP_Strategy_Search,
                             CPX_MIPSEARCH_TRADITIONAL);
   if ( status )  goto TERMINATE;

   /* Set up to use MIP callback */

   status = CPXXsetheuristiccallbackfunc (env, rounddownheur, NULL);
   if ( status )  goto TERMINATE;

   /* Optimize the problem and obtain solution */

   status = CPXXmipopt (env, lp);
   if ( status ) {
      fprintf (stderr, "Failed to optimize MIP.\n");
      goto TERMINATE;
   }

   solstat = CPXXgetstat (env, lp);
   printf ("Solution status %d.\n", solstat);

   status = CPXXgetobjval (env, lp, &objval);
   if ( status ) {
      fprintf (stderr, "Failed to obtain objective value.\n");
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


/* This simple routine frees up the pointer *ptr, and sets *ptr 
   to NULL */

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
rounddownheur (CPXCENVptr env,
               void       *cbdata,
               int        wherefrom, 
               void       *cbhandle,
               double     *objval_p,
               double     *x,
               int        *checkfeas_p,
               int        *useraction_p)
{
   int status = 0;
   
   int       j, cols;
   double    roundobjval;
   int       *feas = NULL;
   CPXCLPptr lp;
   double    *objcoefs = NULL;

   /* Silence compiler warning(s) about unused parameter(s). */

   (void)cbhandle;

   /* Initialize useraction to indicate no user action taken */

   *useraction_p = CPX_CALLBACK_DEFAULT;

   /* Heuristic motivated by knapsack constrained problems.
      Rounding down all fractional values will give an integer
      solution that is feasible, since all constraints are <= 
      with positive coefficients */

   status = CPXXgetcallbacklp (env, cbdata, wherefrom, &lp);
   if ( status ) {
      fprintf (stdout, "Can't get lp pointer.");
      goto TERMINATE;
   }

   cols = CPXXgetnumcols (env, lp);
   if ( cols <= 0 ) {
      fprintf (stdout, "numcols = %d.", cols);
      status = CPXERR_CALLBACK;
      goto TERMINATE;
   }
   
   objcoefs = malloc (cols * sizeof (*objcoefs));
   feas     = malloc (cols * sizeof (*feas));
   if ( objcoefs == NULL || feas == NULL ) {
      fprintf (stdout, "Out of memory.");
      status = CPXERR_CALLBACK;
      goto TERMINATE;
   } 
 
   status = CPXXgetobj (env, lp, objcoefs, 0, cols-1);
   if ( status ) {
      fprintf (stdout, "Can't get objective.");
      goto TERMINATE;
   }

   status = CPXXgetcallbacknodeintfeas (env, cbdata, wherefrom, feas,
                                       0, cols-1);
   if ( status ) {
      fprintf (stdout,
               "Can't get variable feasible status for node.");
      goto TERMINATE;
   }

   roundobjval = *objval_p;
   for (j = 0; j < cols; j++) {

      /* Set the fractional variable to zero and update
         the objective value */

      if ( feas[j] == CPX_INTEGER_INFEASIBLE ) {
         roundobjval -= x[j] * objcoefs[j];
         x[j] = 0.0;
      }
   }
   *objval_p = roundobjval;

   /* Have CPLEX check the solution for integer feasibility */

   *checkfeas_p = 1;

   /* Tell CPLEX that a solution is being returned */

   *useraction_p = CPX_CALLBACK_SET;

TERMINATE:

   free_and_null ((char **) &objcoefs);
   free_and_null ((char **) &feas);  

   return (status);

} /* END rounddown */
