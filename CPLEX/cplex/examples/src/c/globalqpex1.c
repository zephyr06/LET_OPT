/* --------------------------------------------------------------------------
 * File: globalqpex1.c
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

/* globalqpex1.c - Reading in and optimizing a (convex or non-convex) QP
 * problem with convex, first order or global optimizer.*/

/* To run this example, command line arguments are required.
   i.e.,   globalqpex1   filename   optimalitytarget
   where
       filename is the name of the file, with .mps, .lp, or .sav extension
       optimalitytarget   is the optimality target
                   c          for a convex qp
                   f          for a first order solution
                   g          for the global optimum
   Example:
       globalqp1ex  nonconvexqpex.lp  g
 */

/* Bring in the CPLEX function declarations and the C library
   header file stdio.h with the following single include. */

#include <ilcplex/cplex.h>

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
   int      *cstat = NULL;
   int      *rstat = NULL;

   CPXENVptr     env = NULL;
   CPXLPptr      lp = NULL;
   int           status = 0;
   int           j;
   int           cur_numrows, cur_numcols;

   char          *basismsg;

   /* Check the command line arguments */

   if (( argc != 3 )                         ||
       ( strchr ("cfg", argv[2][0]) == NULL )  ) {
      usage (argv[0]);
      goto TERMINATE;
   }

   /* Initialize the CPLEX environment */

   env = CPXopenCPLEX (&status);

   /* If an error occurs, the status value indicates the reason for
      failure.  A call to CPXgeterrorstring will produce the text of
      the error message.  Note that CPXopenCPLEX produces no output,
      so the only way to see the cause of the error is to use
      CPXgeterrorstring.  For other CPLEX routines, the errors will
      be seen if the CPXPARAM_ScreenOutput indicator is set to CPX_ON.  */

   if ( env == NULL ) {
      char  errmsg[CPXMESSAGEBUFSIZE];
      fprintf (stderr, "Could not open CPLEX environment.\n");
      CPXgeterrorstring (env, status, errmsg);
      fprintf (stderr, "%s", errmsg);
      goto TERMINATE;
   }

   /* Turn on output to the screen */

   status = CPXsetintparam (env, CPXPARAM_ScreenOutput, CPX_ON);
   if ( status ) {
      fprintf (stderr,
               "Failure to turn on screen indicator, error %d.\n", status);
      goto TERMINATE;
   }

   /* Create the problem, using the filename as the problem name */

   lp = CPXcreateprob (env, &status, argv[1]);

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

   status = CPXreadcopyprob (env, lp, argv[1], NULL);
   if ( status ) {
      fprintf (stderr, "Failed to read and copy the problem data.\n");
      goto TERMINATE;
   }

   if ( CPXgetprobtype (env, lp) != CPXPROB_QP ) {
      fprintf (stderr, "Input file is not a QP.  Exiting.\n");
      goto TERMINATE;
   }

   /* Optimize the problem and obtain solution. */

   switch (argv[2][0]) {
      case 'c':
         status = CPXsetintparam (env, CPXPARAM_OptimalityTarget,
                                  CPX_OPTIMALITYTARGET_OPTIMALCONVEX);
         if ( status ) goto TERMINATE;

         status = CPXqpopt (env, lp);
         if ( status ) {
            if ( status == CPXERR_Q_NOT_POS_DEF )
               printf ("Problem is not convex. Use argument f to get local "
                       "optimum or g to get global optimum.\n");
            else
               fprintf (stderr, "Failed to optimize QP.\n");
            goto TERMINATE;
         }
            
         break;
      case 'f':
         status = CPXsetintparam (env, CPXPARAM_OptimalityTarget,
                                  CPX_OPTIMALITYTARGET_FIRSTORDER);
         if ( status ) goto TERMINATE;

         status = CPXqpopt (env, lp);
         if ( status ) {
            fprintf (stderr, "Failed to optimize QP.\n");
            goto TERMINATE;
         }
         break;
      case 'g':
         status = CPXsetintparam (env, CPXPARAM_OptimalityTarget,
                                  CPX_OPTIMALITYTARGET_OPTIMALGLOBAL);
         if ( status ) goto TERMINATE;

         status = CPXqpopt (env, lp);
         if ( status ) {
            fprintf (stderr, "Failed to optimize noncvonex QP.\n");
            goto TERMINATE;
         }
         break;
      default:
         break;
   }

   solnstat = CPXgetstat (env, lp);

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

   status = CPXsolninfo (env, lp, &solnmethod, &solntype, NULL, NULL);
   if ( status ) {
      fprintf (stderr, "Failed to obtain solution info.\n");
      goto TERMINATE;
   }
   printf ("Solution status %d, solution method %d\n", solnstat, solnmethod);

   if ( solntype == CPX_NO_SOLN ) {
      fprintf (stderr, "Solution not available.\n");
      goto TERMINATE;
   }

   status = CPXgetobjval (env, lp, &objval);
   if ( status ) {
      fprintf (stderr, "Failed to obtain objective value.\n");
      goto TERMINATE;
   }
   printf ("Objective value %.10g.\n", objval);


   /* The size of the problem should be obtained by asking CPLEX what
      the actual size is.  cur_numrows and cur_numcols store the
      current number of rows and columns, respectively.  */

   cur_numcols = CPXgetnumcols (env, lp);
   cur_numrows = CPXgetnumrows (env, lp);

   /* Retrieve basis, if one is available */

   if ( solntype == CPX_BASIC_SOLN ) {
      cstat = (int *) malloc (cur_numcols*sizeof(int));
      rstat = (int *) malloc (cur_numrows*sizeof(int));
      if ( cstat == NULL || rstat == NULL ) {
         fprintf (stderr, "No memory for basis statuses.\n");
         goto TERMINATE;
      }

      status = CPXgetbase (env, lp, cstat, rstat);
      if ( status ) {
         fprintf (stderr, "Failed to get basis; error %d.\n", status);
         goto TERMINATE;
      }
   }
   else {
      printf ("No basis available\n");
   }

   /* Retrieve solution vector */

   x = (double *) malloc (cur_numcols*sizeof(double));
   if ( x == NULL ) {
      fprintf (stderr, "No memory for solution.\n");
      goto TERMINATE;
   }

   status = CPXgetx (env, lp, x, 0, cur_numcols-1);
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

   status = CPXgetdblquality (env, lp, &maxviol, CPX_MAX_PRIMAL_INFEAS);
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
      status = CPXfreeprob (env, &lp);
      if ( status ) {
         fprintf (stderr, "CPXfreeprob failed, error code %d.\n", status);
      }
   }

   /* Free up the CPLEX environment, if necessary */

   if ( env != NULL ) {
      status = CPXcloseCPLEX (&env);

      /* Note that CPXcloseCPLEX produces no output,
         so the only way to see the cause of the error is to use
         CPXgeterrorstring.  For other CPLEX routines, the errors will
         be seen if the CPXPARAM_ScreenOutput indicator is set to CPX_ON. */

      if ( status ) {
         char  errmsg[CPXMESSAGEBUFSIZE];
         fprintf (stderr, "Could not close CPLEX environment.\n");
         CPXgeterrorstring (env, status, errmsg);
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
   fprintf (stderr,"Usage: %s filename optimalitytarget\n", progname);
   fprintf (stderr,"   where filename is a file with extension \n");
   fprintf (stderr,"      MPS, SAV, or LP (lower case is allowed)\n");
   fprintf (stderr,"   optimalitytarget is the optimality target:\n");
   fprintf (stderr,"      c          for a convex qp\n");
   fprintf (stderr,"      f          for a first order solution\n");
   fprintf (stderr,"      g          for the global optimum\n");
   fprintf (stderr," Exiting...\n");
} /* END usage */
