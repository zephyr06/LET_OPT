/* --------------------------------------------------------------------------
 * File: xadmipex9.c
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

/* xadmipex9.c - Inject heuristic solutions from the generic callback
 *               for optimizing an all binary MIP problem using cplexx.h */

/* To run this example, command line arguments are required:
       xadmipex9 filename
   where
       filename  Name of the file, with .mps, .lp, or .sav
                 extension, and a possible additional .gz
                 extension
   Example:
       xadmipex9  sentoy.mps */

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
   rounddownheur (CPXCALLBACKCONTEXTptr context, void *userdata),
   callback (CPXCALLBACKCONTEXTptr context, CPXLONG where, void* userdata);

static void
   free_and_null (char **ptr),
   usage         (char *progname);


/* We define a structure that will store all the data needed within the callback.
 * Callback code only reads from it, so there is no need to make thread-local
 * copies of it.
 */
typedef struct callbackdata {
   CPXDIM ncols;
   const double *obj;
} CALLBACKDATA;


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
   double *obj = NULL;

   CPXENVptr env = NULL;
   CPXLPptr  lp = NULL;

   CPXDIM j;
   CPXDIM ncols;

   CALLBACKDATA cbdata = { 0, NULL };
   CPXLONG where       = 0;

   /* Check the command line arguments */

   if ( argc != 2 ) {
      usage (argv[0]);
      status = -1;
      goto TERMINATE;
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

   lp = CPXXcreateprob (env, &status, argv[1]);

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

   status = CPXXreadcopyprob (env, lp, argv[1], NULL);
   if ( status ) {
      fprintf (stderr,
               "Failed to read and copy the problem data.\n");
      goto TERMINATE;
   }

   if ( CPXXgetnumcols (env, lp) != CPXXgetnumbin (env, lp) ) {
      fprintf (stderr, "Problem contains non-binary variables, exiting\n");
      goto TERMINATE;
   }

   status = CPXXsetdblparam (env, CPXPARAM_MIP_Tolerances_MIPGap,
                             (double) 1e-6);
   if ( status )  goto TERMINATE;

   /* Set up to use generic callback */
   ncols = CPXXgetnumcols (env, lp);
   obj = malloc (ncols * sizeof (*obj));
   if ( obj == NULL ) {
      fprintf (stderr, "No memory for objective coefficients.\n");
      goto TERMINATE;
   }

   status = CPXXgetobj (env, lp, obj, 0, ncols-1);
   if ( status ) {
      fprintf (stderr, "Failed to obtain objective.\n");
      goto TERMINATE;
   }

   where        |= CPX_CALLBACKCONTEXT_RELAXATION;
   cbdata.ncols  = ncols;
   cbdata.obj    = obj;

   status = CPXXcallbacksetfunc (env, lp, where, callback, &cbdata);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to add callback.\n");
      goto TERMINATE;
   }

   /* Switch off regular heuristics to give our heuristic a chance */

   status = CPXXsetintparam (env, CPXPARAM_MIP_Strategy_HeuristicFreq, -1);
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

   ncols = CPXXgetnumcols (env, lp);

   /* Allocate space for solution */

   x = malloc (ncols * sizeof (*x));
   if ( x == NULL ) {
      fprintf (stderr, "No memory for solution values.\n");
      goto TERMINATE;
   }

   status = CPXXgetx (env, lp, x, 0, ncols-1);
   if ( status ) {
      fprintf (stderr, "Failed to obtain solution.\n");
      goto TERMINATE;
   }

   /* Write out the solution */

   for (j = 0; j < ncols; j++) {
      if ( fabs (x[j]) > 1e-9 ) {
         printf ( "Column %d:  Value = %17.10g\n", j, x[j]);
      }
   }

TERMINATE:

   /* Free the solution vector */

   free_and_null ((char **) &x);
   free_and_null ((char **) &obj);

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

   return status;
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
    "Usage: %s filename\n", progname);
   fprintf (stderr,
    "  filename   Name of a file, with .mps, .lp, or .sav\n");
   fprintf (stderr,
    "             extension, and a possible, additional .gz\n");
   fprintf (stderr,
    "             extension\n");
} /* END usage */

#define MIN(a,b) (((a)<(b))?(a):(b))

static int CPXPUBLIC
rounddownheur (CPXCALLBACKCONTEXTptr context,
               void                  *userdata)
{
   int status = 0;
   CPXDIM j, cols;
   CPXDIM *ind = NULL;
   double *x = NULL;
   const double *obj;
   double frac, integral, relobj;
   CALLBACKDATA *cbdata = (CALLBACKDATA *) userdata;

   cols = cbdata->ncols;
   obj = cbdata->obj;

   x   = malloc (cols * sizeof (*x));
   ind = malloc (cols * sizeof (*ind));
   if ( x == NULL || ind == NULL ) {
      fprintf (stderr, "No memory for work data.\n");
      goto TERMINATE;
   }

   /* Get solution to relaxation */
   status = CPXXcallbackgetrelaxationpoint( context, x, 0, cols-1, &relobj);
   if ( status ) {
      fprintf (stderr, "Could not get solution %d\n", status);
      goto TERMINATE;
   }

   /* Heuristic motivated by knapsack constrained problems.
      Rounding down all fractional values will give an integer
      solution that is feasible, since all constraints are <=
      with positive coefficients */

   for (j = 0; j < cols; j++) {
      ind[j] = j;

      if ( x[j] ) {
         /* Set the fractional variables to zero */
         frac = modf(x[j], &integral);

         frac = MIN (1.0-frac, frac);

         if ( frac > 1.0e-6 ) {
            relobj -= x[j]*obj[j];
            x[j]    = 0.0;
         }
      }
   }

   status = CPXXcallbackpostheursoln( context, cols, ind, x, relobj,
                                      CPXCALLBACKSOLUTION_CHECKFEAS );
   if ( status ) {
      fprintf (stderr, "Could not post solution %d\n", status);
      goto TERMINATE;
   }

TERMINATE:

   free_and_null ((char **) &x);
   free_and_null ((char **) &ind);

   return (status);

} /* END rounddown */


/* Now the callback itself. */
static int CPXPUBLIC
callback (CPXCALLBACKCONTEXTptr context, CPXLONG where, void* userdata)
{
   int status = 0;

   if ( where == CPX_CALLBACKCONTEXT_RELAXATION ) {
      /* If we are called with a relaxation solution, we try find a integer
         solution by rounding.
       */
      status = rounddownheur (context,userdata);
   }
   else {
      /* If we are called in another context it is an error */
      fprintf (stderr, "ERROR: Callback called in an unexpected context.\n");
      return 1;
   }
   return status;
}

