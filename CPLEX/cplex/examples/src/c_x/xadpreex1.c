/* --------------------------------------------------------------------------
 * File: xadpreex1.c
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

/* xadpreex1.c - Reading in and optimizing a problem.
 * The optimization is in two steps:  first optimize on product
 * cost and revenue, then minimize inventory level.
 * This example uses cplexx.h
 */

/* When this example is run the program reads a problem from a
   file named "prod.lp" either from the directory ../../data
   if no argument is passed to the executable or from the directory
   that is specified as the first (and only) argument to the executable. */

/* Bring in the CPLEX function declarations and the C library 
   header file stdio.h with the following single include. */

#include <ilcplex/cplexx.h>

/* Bring in the declarations for the string, math and character functions 
   and malloc */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Include declarations for functions in this program */

static void
   free_and_null (char **ptr);

int
main (int argc, char *argv[])
{
   CPXENVptr env = NULL;
   CPXLPptr  lp = NULL;
   int       status = 0;
   CPXDIM    j;
   CPXDIM    numcols;
   double    totinv; 

   int       solstat;
   double    objval;
   double    *x = NULL;

   double    rrhs[1];
   char      rsense[1];
   CPXNNZ    rmatbeg[1];

   CPXDIM    *indices = NULL;
   double    *values = NULL;
   char      *namestore = NULL;
   char      **nameptr = NULL;
   CPXSIZE   surplus, storespace;

   const char * datadir = argc <= 1 ? "../../../examples/data" : argv[1];
   char *prod = NULL;

   prod = (char *) malloc (strlen (datadir) + 1 + strlen("prod.lp") + 1);
   sprintf (prod, "%s/prod.lp", datadir);

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

   lp = CPXXcreateprob (env, &status, "prod.lp");

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

   status = CPXXreadcopyprob (env, lp, prod, NULL);
   if ( status ) {
      fprintf (stderr, "Failed to read and copy the problem data.\n");
      goto TERMINATE;
   }

   /* Tell presolve to do only primal reductions,
      turn off simplex logging */

   status = CPXXsetintparam (env, CPXPARAM_Preprocessing_Reduce, 1);
   if ( status ) {
      fprintf (stderr, "Failed to set CPXPARAM_Preprocessing_Reduce: %d\n",
               status);
      goto TERMINATE;
   }
   status = CPXXsetintparam (env, CPXPARAM_Simplex_Display, 0);
   if ( status ) {
      fprintf (stderr, "Failed to set CPXPARAM_Simplex_Display: %d\n", status);
      goto TERMINATE;
   } 


   /* Optimize the problem and obtain solution. */

   status = CPXXlpopt (env, lp);
   if ( status ) {
      fprintf (stderr, "Failed to optimize profit LP.\n");
      goto TERMINATE;
   }

   solstat = CPXXgetstat (env, lp);
   status  = CPXXgetobjval (env, lp, &objval);

   if ( status || solstat != CPX_STAT_OPTIMAL ) {
      fprintf (stderr, "Solution failed. Status %d, solstat %d.\n",
               status, solstat);
      goto TERMINATE;
   }
   printf ("Profit objective value is %g\n", objval);


   /* Allocate space for column names */

   numcols = CPXXgetnumcols (env, lp);
   if ( !numcols ) {
      fprintf (stderr, "No columns in problem\n");
      goto TERMINATE;
   }

   CPXXgetcolname (env, lp, NULL, NULL, 0, &surplus, 0, numcols-1);
   storespace = - surplus;

   namestore = malloc (storespace * sizeof(*namestore));
   nameptr   = malloc (numcols * sizeof(*nameptr));
   if ( namestore == NULL  ||  nameptr == NULL ) {
      fprintf (stderr, "No memory for column names\n");
      goto TERMINATE;
   }
 
   status = CPXXgetcolname (env, lp, nameptr, namestore, storespace,
                           &surplus, 0, numcols-1);
   if ( status ) {
      fprintf (stderr, "Failed to get column names\n");
      goto TERMINATE;
   }

   /* Allocate space for solution */

   x = malloc (numcols * sizeof(*x));

   if ( x == NULL ) {
      fprintf (stderr,"No memory for solution.\n");
      goto TERMINATE;
   }

   status = CPXXgetx (env, lp, x, 0, numcols-1);
   if ( status ) {
      fprintf (stderr, "Failed to obtain primal solution.\n");
      goto TERMINATE;
   }

   totinv = 0;
   for (j = 0; j < numcols; j++) {
      if ( !strncmp (nameptr[j], "inv", 3) )  totinv += x[j];
   }
   printf ("Inventory level under profit objective is %g\n", totinv);

   /* Allocate space for a constraint */

   indices = malloc (numcols * sizeof (*indices));
   values  = malloc (numcols * sizeof (*values));

   if ( indices == NULL  ||  values == NULL ) {
      fprintf (stderr, "No memory for constraint\n");
      goto TERMINATE;
   }

   /* Get profit objective and add it as a constraint */

   status = CPXXgetobj (env, lp, values, 0, numcols-1);
   if ( status ) {
      fprintf (stderr,
              "Failed to get profit objective.  Status %d\n", status);
      goto TERMINATE;
   }
   for (j = 0; j < numcols; j++) {
      indices[j] = j;
   }

   rrhs[0]    = objval - fabs (objval) * 1e-6;
   rsense[0]  = 'G';
   rmatbeg[0] = 0;

   status = CPXXpreaddrows (env, lp, 1, numcols, rrhs, rsense,
                           rmatbeg, indices, values, NULL);

   if ( status ) {
      fprintf (stderr,
              "Failed to add objective as constraint.  Status %d\n",
              status);
      goto TERMINATE;
   }

   /* Set up objective to maximize negative of sum of inventory */

   totinv = 0;
   for (j = 0; j < numcols; j++) {
      if ( strncmp (nameptr[j], "inv", 3) ) {
         values[j] = 0.0;
      }
      else {
         values[j] = - 1.0;
      }
   }

   status = CPXXprechgobj (env, lp, numcols, indices, values);

   if ( status ) {
      fprintf (stderr,
              "Failed to change to inventory objective.  Status %d\n",
              status);
      goto TERMINATE;
   }

   status = CPXXlpopt (env, lp);
   if ( status ) {
      fprintf (stderr, "Optimization on inventory level failed. Status %d.\n",
              status);
      goto TERMINATE;
   }

   solstat = CPXXgetstat (env, lp);
   status  = CPXXgetobjval (env, lp, &objval);
   if ( status  ||  solstat != CPX_STAT_OPTIMAL ) {
      fprintf (stderr, "Solution failed. Status %d, solstat %d.\n",
               status, solstat);
      goto TERMINATE;
   }

   printf ("Inventory level after optimization is %g\n", -objval);


   status = CPXXgetx (env, lp, x, 0, numcols-1);
   if ( status ) {
      fprintf (stderr, "Failed to obtain primal solution.\n");
      goto TERMINATE;
   }

   /* Write out the solution */
   printf("Solution status: %d", solstat);
   printf ("\n");
   for (j = 0; j < numcols; j++) {
      printf ( "%s:  Value = %17.10g\n", nameptr[j], x[j]);
   }


TERMINATE:

   /* Free the filename */

   free_and_null ((char **) &prod);

   /* Free up the basis and solution */

   free_and_null ((char **) &indices);
   free_and_null ((char **) &values);
   free_and_null ((char **) &nameptr);
   free_and_null ((char **) &namestore);
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


