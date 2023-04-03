/* --------------------------------------------------------------------------
 * File: xglobalmiqpex1.c
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

/* xglobalmiqpex1.c - Reading in and optimizing a (convex or nonconvex) MIQP
 * problem with global optimizer.*/

/* To run this example, command line arguments are required.
   i.e.,   xglobalmiqpex1   filename 
   where
       filename is the name of the file, with .mps, .lp, or .sav extension
   Example:
       xglobalmiqpex1  nonconvexmiqpex.lp 
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
   double   objval, maxviol;
   double   *x     = NULL;

   CPXENVptr     env = NULL;
   CPXLPptr      lp = NULL;
   int           status = 0;
   CPXDIM        j;
   CPXDIM        cur_numcols;

   /* Check the command line arguments */

   if (( argc != 2 )) {
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
      be seen if the CPXX_PARAM_SCRIND indicator is set to CPX_ON.  */

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
      the parameter CPXX_PARAM_SCRIND causes the error message to
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

   if ( CPXXgetprobtype (env, lp) != CPXPROB_MIQP ) {
      fprintf (stderr, "Input file is not a MIQP.  Exiting.\n");
      goto TERMINATE;
   }

   /* Optimize the problem and obtain solution. */

   status = CPXXsetintparam (env, CPXPARAM_OptimalityTarget,
                            CPX_OPTIMALITYTARGET_OPTIMALGLOBAL);
   if ( status ) goto TERMINATE;

   status = CPXXmipopt (env, lp);
   if ( status ) {
      fprintf (stderr, "Failed to optimize nonconvex MIQP.\n");
      goto TERMINATE;
   }

   solnstat = CPXXgetstat (env, lp);

   if      ( solnstat == CPXMIP_UNBOUNDED  ||
             solnstat == CPX_STAT_UNBOUNDED  ) {
      printf ("Model is unbounded\n");
      goto TERMINATE;
   }
   else if ( solnstat == CPXMIP_INFEASIBLE  ||
             solnstat == CPX_STAT_INFEASIBLE  ) {
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

   status = CPXXgetobjval (env, lp, &objval);
   if ( status ) {
      fprintf (stderr, "Failed to obtain objective value.\n");
      goto TERMINATE;
   }
   printf ("Objective value %.10g.\n", objval);

   /* The size of the problem should be obtained by asking CPLEX what
      the actual size is.  cur_numcols stores the current number of
      columns.  */

   cur_numcols = CPXXgetnumcols (env, lp);

   /* Similarly, CPXXgetnumrows can be used to query the number of rows. */

   /* Retrieve solution vector */

   x = (double *) malloc (cur_numcols*sizeof(double));
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

   free_and_null ((char **) &x);

   /* Free up the problem, if necessary */

   if ( lp != NULL ) {
      status = CPXXfreeprob (env, &lp);
      if ( status ) {
         fprintf (stderr, "CPXXfreeprob failed, error code %d.\n", status);
      }
   }

   /* Free up the CPLEX environment, if necessary */

   if ( env != NULL ) {
      status = CPXXcloseCPLEX (&env);

      /* Note that CPXXcloseCPLEX produces no output,
         so the only way to see the cause of the error is to use
         CPXXgeterrorstring.  For other CPLEX routines, the errors will
         be seen if the CPXX_PARAM_SCRIND indicator is set to CPX_ON. */

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
   fprintf (stderr,"Usage: %s filename \n", progname);
   fprintf (stderr,"   where filename is a file with extension \n");
   fprintf (stderr,"      MPS, SAV, or LP (lower case is allowed)\n");
   fprintf (stderr," Exiting...\n");
} /* END usage */
