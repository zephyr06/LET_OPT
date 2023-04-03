/* --------------------------------------------------------------------------
 * File: xmultiobjex1.c
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

/* xmultiobjex1.c - Reading in and optimizing a multiobjective problem;
 *                  uses cplexx.h */

/* To run this example, command line arguments are required.
      xmultiobjex1   filename paramset.prm
   where
       filename is the name of the file, with .mps, .lp, or .sav extension
       paramset.prm is a .prm file that defines default parameter to
                    apply for optimizing the first objective
   Example:
       xmultiobjex1  example.mps paramset.prm
 */

/* Bring in the CPLEX function declarations and the C library
   header file stdio.h with the following single include. */

#include <ilcplex/cplexx.h>

/* Bring in the declarations for the string and character functions
   and malloc */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* Include declarations for functions in this program */

static void
   free_and_null (char **ptr),
   usage         (char *progname);


int
main (int argc, char *argv[])
{
   /* Declare and allocate space for the variables and arrays where we will
      store the optimization results including the status, objective value,
      maximum bound violation, variable values, and basis. */

   int      solnstat, solnmethod, solntype;
   int      nobjs, i;

   double   objval, maxviol;
   double   *x     = NULL;
   int      *cstat = NULL;
   int      *rstat = NULL;

   CPXENVptr     env = NULL;
   CPXLPptr      lp = NULL;
   int           status = 0;
   CPXDIM        j;
   CPXDIM        cur_numrows, cur_numcols;

   char          *basismsg;

   CPXPARAMSETptr *paramsets = NULL;

   /* Check the command line arguments */

   if ( argc < 2 || argc > 3 ) {
      usage (argv[0]);
      goto TERMINATE;
   }

   /* Initialize the CPLEX environment */

   env = CPXXopenCPLEX (&status);

   /* If an error occurs, the status value indicates the reason for
      failure.  A call to CPXXgeterrorstring will produce the text of
      the error message.  Note that CPXXopenCPLEX produces no output,
      so the only way to see the cause of the error is to use
      CPXXgeterrorstring.  For other CPLEX routines, the errors will
      be seen if the CPXPARAM_ScreenOutput indicator is set to CPX_ON.  */

   if ( env == NULL ) {
      char  errmsg[CPXMESSAGEBUFSIZE];
      fprintf (stderr, "Could not open CPLEX environment.\n");
      CPXXgeterrorstring (env, status, errmsg);
      fprintf (stderr, "%s", errmsg);
      goto TERMINATE;
   }

   /* Turn on output to the screen */

   status = CPXXsetintparam (env, CPXPARAM_ScreenOutput, CPX_ON);
   if ( status ) {
      fprintf (stderr,
               "Failure to turn on screen indicator, error %d.\n", status);
      goto TERMINATE;
   }

   /* Create the problem, using the filename as the problem name */

   lp = CPXXcreateprob (env, &status, argv[1]);

   /* A returned pointer of NULL may mean that not enough memory
      was available or there was some other problem.  In the case of
      failure, an error message will have been written to the error
      channel from inside CPLEX.  In this example, the setting of
      the parameter CPXPARAM_ScreenOutput causes the error message to
      appear on stdout.  Note that most CPLEX routines return
      an error code to indicate the reason for failure.   */

   if ( lp == NULL ) {
      fprintf (stderr, "Failed to create LP.\n");
      goto TERMINATE;
   }

   /* Now read the file, and copy the data into the created lp */

   status = CPXXreadcopyprob (env, lp, argv[1], NULL);
   if ( status ) {
      fprintf (stderr, "Failed to read and copy the problem data.\n");
      goto TERMINATE;
   }

   if ( argc == 3 ) {
      CPXDIM numobj = CPXXgetnumobjs (env, lp);
      paramsets = calloc (numobj, sizeof(*paramsets));

      /* Set the paramset only for the first optimization */
      paramsets[0] = CPXXparamsetcreate (env, &status);
      if ( status ) {
         fprintf (stderr,
                  "Failure to create parameter set %d.\n", status);
         goto TERMINATE;
      }
      status = CPXXparamsetreadcopy (env, paramsets[0], argv[2]);
      if ( status ) {
         fprintf (stderr, "Failure to read parameter file %s into "
                          "parameter set\n", argv[2]);
         goto TERMINATE;
      }
   }

   CPXXchgprobtype (env, lp, 0);

   status = CPXXmultiobjopt(env, lp, (CPXCPARAMSETptr *)paramsets);
   if ( status ) {
      fprintf (stderr, "Failed to optimize LP.\n");
      goto TERMINATE;
   }

   solnstat = CPXXgetstat (env, lp);

   if      ( solnstat == CPX_STAT_UNBOUNDED ) {
      printf ("Model is unbounded\n");
      goto TERMINATE;
   }
   else if ( solnstat == CPX_STAT_INFEASIBLE ) {
      printf ("Model is infeasible\n");
      goto TERMINATE;
   }
   else if ( solnstat == CPX_STAT_INForUNBD ) {
      printf ("Model is infeasible or unbounded\n");
      goto TERMINATE;
   }

   status = CPXXsolninfo (env, lp, &solnmethod, &solntype, NULL, NULL);
   if ( status ) {
      fprintf (stderr, "Failed to obtain solution info.\n");
      goto TERMINATE;
   }
   printf ("Solution status %d, solution method %d\n", solnstat, solnmethod);

   if ( solntype == CPX_NO_SOLN ) {
      fprintf (stderr, "Solution not available.\n");
      goto TERMINATE;
   }

   /* The size of the problem should be obtained by asking CPLEX what
      the actual size is.  cur_numrows and cur_numcols store the
      current number of rows and columns, respectively.  */

   cur_numcols = CPXXgetnumcols (env, lp);
   cur_numrows = CPXXgetnumrows (env, lp);

   nobjs = CPXXgetnumobjs (env, lp);

   for (i = 0; i < nobjs; ++i) {
#define buffersize 50
      char oname[buffersize];
      CPXSIZE surplus;

      double reltol, abstol, weight;
      int priority;
      status = CPXXmultiobjgetobjval (env, lp, i, &objval);
      if ( status ) {
         fprintf (stderr, "Failed to obtain objective value.\n");
         goto TERMINATE;
      }

      status = CPXXmultiobjgetname (env, lp, i, oname, buffersize, &surplus);
      if ( surplus < 0 ) {
         fprintf (stderr, "Failed to obtain objective name buffer too small.\n");
         goto TERMINATE;
      }
      if ( status ) {
         fprintf (stderr, "Failed to obtain objective name.\n");
         goto TERMINATE;
      }

      status = CPXXmultiobjgetobj (env, lp, i, NULL, 1, 0, NULL,
                                   &weight, &priority, &abstol, &reltol);
      if ( status ) {
         fprintf (stderr, "Failed to obtain objective attributes.\n");
         goto TERMINATE;
      }
      printf ("Objective value of obj %s: %.3g (weight %g, priority %i).\n",
              oname, objval, weight, priority);
   }

   /* Retrieve basis, if one is available */

   if ( solntype == CPX_BASIC_SOLN ) {
      cstat = malloc (cur_numcols*sizeof(*cstat));
      rstat = malloc (cur_numrows*sizeof(*rstat));
      if ( cstat == NULL || rstat == NULL ) {
         fprintf (stderr, "No memory for basis statuses.\n");
         goto TERMINATE;
      }

      status = CPXXgetbase (env, lp, cstat, rstat);
      if ( status ) {
         fprintf (stderr, "Failed to get basis; error %d.\n", status);
         goto TERMINATE;
      }
   }
   else {
      printf ("No basis available\n");
   }


   /* Retrieve solution vector */

   x = malloc (cur_numcols*sizeof(*x));
   if ( x == NULL ) {
      fprintf (stderr, "No memory for solution.\n");
      goto TERMINATE;
   }

   status = CPXXgetx (env, lp, x, 0, cur_numcols-1);
   if ( status ) {
      fprintf (stderr, "Failed to obtain primal solution.\n");
      goto TERMINATE;
   }


   /* Write out the solution */

   for (j = 0; j < cur_numcols; j++) {
      printf ( "Column %d:  Value = %17.10g", j, x[j]);
      if ( cstat != NULL ) {
         switch (cstat[j]) {
            case CPX_AT_LOWER:
               basismsg = "Nonbasic at lower bound";
               break;
            case CPX_BASIC:
               basismsg = "Basic";
               break;
            case CPX_AT_UPPER:
               basismsg = "Nonbasic at upper bound";
               break;
            case CPX_FREE_SUPER:
               basismsg = "Superbasic, or free variable at zero";
               break;
            default:
               basismsg = "Bad basis status";
               break;
         }
         printf ("  %s",basismsg);
      }
      printf ("\n");
   }

   /* Display the maximum bound violation. */

   status = CPXXgetdblquality (env, lp, &maxviol, CPX_MAX_PRIMAL_INFEAS);
   if ( status ) {
      fprintf (stderr, "Failed to obtain bound violation.\n");
      goto TERMINATE;
   }
   printf ("Maximum bound violation = %17.10g\n", maxviol);


TERMINATE:

   /* Free up the basis and solution */

   free_and_null ((char **) &cstat);
   free_and_null ((char **) &rstat);
   free_and_null ((char **) &x);

   /* Free up the problem, if necessary */

   if ( lp != NULL ) {
      status = CPXXfreeprob (env, &lp);
      if ( status ) {
         fprintf (stderr, "CPXXfreeprob failed, error code %d.\n", status);
      }
   }

   /* Free up the paramset, if necessary */
   if ( paramsets != NULL && paramsets[0] != NULL ) {
      status = CPXXparamsetfree (env, &paramsets[0]);
      if ( status ) {
         fprintf (stderr, "CPXXparamsetfree failed, error code %d.\n", status);
      }
   }
   free_and_null ((char **)&paramsets);

   /* Free up the CPLEX environment, if necessary */

   if ( env != NULL ) {
      status = CPXXcloseCPLEX (&env);

      /* Note that CPXXcloseCPLEX produces no output,
         so the only way to see the cause of the error is to use
         CPXXgeterrorstring.  For other CPLEX routines, the errors will
         be seen if the CPXPARAM_ScreenOutput indicator is set to CPX_ON. */

      if ( status ) {
         char  errmsg[CPXMESSAGEBUFSIZE];
         fprintf (stderr, "Could not close CPLEX environment.\n");
         CPXXgeterrorstring (env, status, errmsg);
         fprintf (stderr, "%s", errmsg);
      }
   }

   return (status);

}  /* END main */

/* This simple routine frees up the pointer *ptr, and sets *ptr to NULL */

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
   fprintf (stderr,"Usage: %s filename\n", progname);
   fprintf (stderr,"   where filename is a file with extension \n");
   fprintf (stderr," Exiting...\n");
} /* END usage */
