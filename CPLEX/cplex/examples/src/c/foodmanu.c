/* --------------------------------------------------------------------------
 * File: foodmanu.c
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

/* A formulation of the food manufacturing problem in H.P. Williams'
   book Model Building in Mathematical Programming.  The formulation
   demonstrates the use indicator constraints and semi-continuous
   variables */

/* Bring in the CPLEX function declarations and the C library
   header file stdio.h with the include of cplex.h. */

#include <ilcplex/cplex.h>

/* Bring in the declarations for the string functions */

#include <string.h>
#include <stdlib.h>

/* Include declaration for functions at end of program */

static int
   buildmodel (CPXENVptr env, CPXLPptr lp),
   varindex   (int month, int product, int whichvar);

static void
   free_and_null (char **ptr);


/* Five products are to be manufactured; define an index for each one */

#define NUMPRODUCTS 5

#define VEGOIL1  0
#define VEGOIL2  1
#define OIL1     2
#define OIL2     3
#define OIL3     4


/* The production is to be planned for 6 months; index from 0 to 5 */

#define NUMMONTHS   6


/* There are four variables to decide for each month and product:
   how much to buy, use and store, and whether to use or not.
   Define index values for buy, use, store, using variables.  */

#define NUMVARS     4

#define BUY         0
#define USE         1
#define STORE       2
#define IS_USED     3


double cost[] = {110.0, 120.0, 130.0, 110.0, 115.0, /* Cost for January */
                 130.0, 130.0, 110.0,  90.0, 115.0, /* Cost for February */
                 110.0, 140.0, 130.0, 100.0,  95.0, /* Cost for March */
                 120.0, 110.0, 120.0, 120.0, 125.0, /* Cost for April */
                 100.0, 120.0, 150.0, 110.0, 105.0, /* Cost for May */
                 90.0, 100.0, 140.0,  80.0, 135.0}; /* Cost for June */

double hardness[] = {8.8, 6.1, 2.0, 4.2, 5.0}; /* Hardness of each product */

int
main (void)
{
   /* Declare variables and arrays where we will store the
      optimization results including the status, objective value,
      and variable values. */

   int       solstat;
   double    objval;

   int       colcnt = 0;
   double    *x = NULL;

   CPXENVptr env = NULL;
   CPXLPptr  lp = NULL;
   int       status;
   int       m, p;

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


   /* Formulate and solve the problem */

   lp = CPXcreateprob (env, &status, "food manufacturing");

   /* A returned pointer of NULL may mean that not enough memory
      was available or there was some other problem.  In the case of
      failure, an error message will have been written to the error
      channel from inside CPLEX.  In this example, the setting of
      the parameter CPXPARAM_ScreenOutput causes the error message to
      appear on stdout.  */

   if ( lp == NULL ) {
      fprintf (stderr, "Failed to create LP.\n");
      goto TERMINATE;
   }

   /* Build the model */

   status = buildmodel (env, lp);
   if ( status ) {
      fprintf (stderr, "Failed to build model.\n");
      goto TERMINATE;
   }

   /* Write a copy of the problem to a file. */

   status = CPXwriteprob (env, lp, "foodmanu.lp", NULL);
   if ( status ) {
      fprintf (stderr, "Failed to write LP to disk.\n");
      goto TERMINATE;
   }

   /* Optimize the problem and obtain solution. */

   status = CPXmipopt (env, lp);
   if ( status ) {
      fprintf (stderr, "Failed to optimize MIP.\n");
      goto TERMINATE;
   }

   solstat = CPXgetstat (env, lp);

   /* Write solution status, objective and solution vector to the screen. */

   printf ("\nSolution status = %d\n", solstat);

   status = CPXgetobjval (env, lp, &objval);
   if ( status ) {
      fprintf (stderr,"No MIP objective value available.  Exiting...\n");
      goto TERMINATE;
   }

   printf ("Solution value (maximum profit) = %f\n\n", objval);

   colcnt = NUMVARS*NUMMONTHS*NUMPRODUCTS;
   x = (double *) malloc (colcnt * sizeof(double));
   if ( x == NULL ) {
      status = CPXERR_NO_MEMORY;
      fprintf (stderr, "Could not allocate memory for solution.\n");
      goto TERMINATE;
   }

   status = CPXgetx (env, lp, x, 0, colcnt - 1);
   if ( status ) {
      fprintf (stderr, "Failed to get optimal integer x.\n");
      goto TERMINATE;
   }

   for (m = 0; m < NUMMONTHS; m++) {
      printf ("Month %d \n", m);

      printf ("  . buy   ");
      for (p = 0; p < NUMPRODUCTS; p++)
         printf ("%f\t", x[varindex(m, p, BUY)]);
      printf ("\n");

      printf ("  . use   ");
      for (p = 0; p < NUMPRODUCTS; p++)
         printf ("%f\t", x[varindex (m, p, USE)]);
      printf ("\n");

      printf ("  . store ");
      for (p = 0; p < NUMPRODUCTS; p++)
         printf ("%f\t", x[varindex (m, p, STORE)]);
      printf ("\n");
   }

   /* Free problem */

   status = CPXfreeprob (env, &lp);
   if ( status ) {
      fprintf (stderr, "CPXfreeprob failed, error code %d.\n", status);
      goto TERMINATE;
   }

 TERMINATE:

   free_and_null ((char **) &x);

   /* Free up the problem as allocated by CPXcreateprob, if necessary */

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


/* Build food manufacturing model using indicator constraints and
   semi-continuous variables */

static int
buildmodel (CPXENVptr env, CPXLPptr lp)
{

   int    colcnt = NUMVARS*NUMMONTHS*NUMPRODUCTS;

   double *obj     = NULL;
   double *lb      = NULL;
   double *ub      = NULL;
   char   *ctype   = NULL;
   int    *rmatind = NULL;
   double *rmatval = NULL;

   int    indicator;
   int    rmatbeg[1];
   double rhs[1];
   char   sense[1];

   int    m, p;
   int    status = 0;

   status = CPXchgobjsen (env, lp, CPX_MAX); /* Maximization problem */
   if ( status ) {
      fprintf (stderr, "Could not change objective sense.\n");
      goto TERMINATE;
   }

   rmatbeg[0] = 0;

   /* Allocate colcnt-sized arrays */

   obj     = (double *) malloc (colcnt * sizeof(double));
   lb      = (double *) malloc (colcnt * sizeof(double));
   ub      = (double *) malloc (colcnt * sizeof(double));
   ctype   = (char *)   malloc (colcnt * sizeof(char));
   rmatind = (int * )   malloc (colcnt * sizeof(int));
   rmatval = (double *) malloc (colcnt * sizeof(double));

   if ( obj     == NULL ||
        lb      == NULL ||
        ub      == NULL ||
        ctype   == NULL ||
        rmatind == NULL ||
        rmatval == NULL   ) {
      fprintf (stderr, "Could not allocate colcnt arrays\n");
      status = CPXERR_NO_MEMORY;
      goto TERMINATE;
   }

   /* Create variables. For each month and each product, we have 3
      variables corresponding to the quantity used (semi-continuous),
      stored (continuous) and bought (continuous) and one binary
      variable indicating whether or not the product is used during
      this month. */

   for (m = 0; m < NUMMONTHS; m++) {
      for (p = 0; p < NUMPRODUCTS; p++) {

         /* The quantity bought is a continuous variable. It has a cost */

         obj[varindex(m, p, BUY)]    = -cost[m*NUMPRODUCTS + p];
         lb[varindex (m, p, BUY)]    = 0.0;
         ub[varindex (m, p, BUY)]    = CPX_INFBOUND;
         ctype[varindex (m, p, BUY)] = 'C';

         /* When an oil is used, the quantity must be at least 20
            tons. This is modeled as a semi-continuous variable. */

         obj[varindex (m, p, USE)]   = 0.0;
         lb[varindex (m, p, USE)]    = 20.0;
         ub[varindex (m, p, USE)]    = CPX_INFBOUND;
         ctype[varindex (m, p, USE)] = 'S';

         /* It is possible to store up to 1000 tons of each
            product. There are storage costs. */

         obj[varindex (m, p, STORE)]   = -5.0;
         lb[varindex (m, p, STORE)]    = 0.0;
         ub[varindex (m, p, STORE)]    = 1000.0;
         ctype[varindex (m, p, STORE)] = 'C';

         /* At the end, we must have exactly 500 tons of each
            product in storage. */

         if ( m == NUMMONTHS - 1 ) {
            lb[varindex (m, p, STORE)] = 500.0;
            ub[varindex (m, p, STORE)] = 500.0;
         }

         /* The variable indicating whether or not a product is
            used during a month is a binary variable. */

         obj[varindex (m, p, IS_USED)]   = 0.0;
         lb[varindex (m, p, IS_USED)]    = 0.0;
         ub[varindex (m, p, IS_USED)]    = 1.0;
         ctype[varindex (m, p, IS_USED)] = 'B';
      }
   }

   status = CPXnewcols (env, lp, colcnt, obj, lb, ub, ctype, NULL);
   if ( status ) {
      fprintf (stderr, "Could not add new columns.\n");
      goto TERMINATE;
   }

   /* Constraints for each month */

   for (m = 0; m < NUMMONTHS; m++) {
      int totalindex;

      /* For each product, create an indicator constraint linking the
         quantity used and the binary variable indicating whether or
         not the product is used */

      for (p = 0; p < NUMPRODUCTS; p++) {
         indicator  = varindex (m, p, IS_USED);
         rmatind[0] = varindex (m, p, USE);
         rmatval[0] = 1.0;
         status = CPXaddindconstr (env, lp, indicator, 1, 1, 0.0, 'L',
                                   rmatind, rmatval, NULL);
         if ( status ) {
            fprintf (stderr, "Could not add new indicator constraint.\n");
            goto TERMINATE;
         }
      }

      /* Not more than 200 tons of vegetable oil can be refined */

      rmatind[0] = varindex (m, VEGOIL1, USE);
      rmatind[1] = varindex (m, VEGOIL2, USE);
      rmatval[0] = 1.0;
      rmatval[1] = 1.0;
      rhs[0]     = 200.0;
      sense[0]   = 'L';
      status = CPXaddrows (env, lp, 0, 1, 2, rhs,
                           sense, rmatbeg, rmatind, rmatval,
                           NULL, NULL);

      /* Not more than 250 tons of non-vegetable oil can be refined */

      rmatind[0] = varindex (m, OIL1, USE);
      rmatind[1] = varindex (m, OIL2, USE);
      rmatind[2] = varindex (m, OIL3, USE);
      rmatval[0] = 1.0;
      rmatval[1] = 1.0;
      rmatval[2] = 1.0;
      rhs[0]     = 250.0;
      sense[0]   = 'L';
      status = CPXaddrows (env, lp, 0, 1, 3, rhs,
                           sense, rmatbeg, rmatind, rmatval,
                           NULL, NULL);
      if ( status ) {
         fprintf (stderr, "Could not add new rows.\n");
         goto TERMINATE;
      }

      /* Constraint on food composition */

      /* Add a variable corresponding to total quantity produced
         in a month */

      obj[0]   = 150.0;
      lb[0]    = 0.0;
      ub[0]    = CPX_INFBOUND;
      ctype[0] = 'C';

      status = CPXnewcols (env, lp, 1, obj, lb, ub, ctype, NULL);
      if ( status ) {
         fprintf (stderr, "Could not add new columns.\n");
         goto TERMINATE;
      }
      totalindex = CPXgetnumcols (env, lp) - 1;

      /* Total quantity = sum (quantities) */

      for (p = 0; p < NUMPRODUCTS; p++) {
         rmatind[p] = varindex (m, p, USE);
         rmatval[p] = 1.0;
      }
      rmatind[NUMPRODUCTS] = totalindex;
      rmatval[NUMPRODUCTS] = -1.0;
      rhs[0]               =  0.0;
      sense[0]             = 'E';
      status = CPXaddrows (env, lp, 0, 1, NUMPRODUCTS + 1, rhs,
                           sense, rmatbeg, rmatind, rmatval,
                           NULL, NULL);
      if ( status ) {
         fprintf (stderr, "Could not add new rows.\n");
         goto TERMINATE;
      }

      /* Hardness constraints
          sum (quantity * hardness) >= 3 * total quantity
          sum (quantity * hardness) <= 6 * total quantity
      */

      for (p = 0; p < NUMPRODUCTS; p++) {
         rmatind[p] = varindex (m, p, USE);
         rmatval[p] = hardness[p];
      }
      rmatind[NUMPRODUCTS] = totalindex;
      rmatval[NUMPRODUCTS] = -3.0;
      rhs[0]               =  0.0;
      sense[0]             = 'G';
      status = CPXaddrows (env, lp, 0, 1, NUMPRODUCTS + 1, rhs,
                           sense, rmatbeg, rmatind, rmatval,
                           NULL, NULL);
      if ( status ) {
         fprintf (stderr, "Could not add new rows.\n");
         goto TERMINATE;
      }

      rmatval[NUMPRODUCTS] = -6.0;
      sense[0]             = 'L';
      status = CPXaddrows (env, lp, 0, 1, NUMPRODUCTS + 1, rhs,
                           sense, rmatbeg, rmatind, rmatval,
                           NULL, NULL);
      if ( status ) {
         fprintf (stderr, "Could not add new rows.\n");
         goto TERMINATE;
      }

      /* The food may never be made up of more than three oils */

      for (p = 0; p < NUMPRODUCTS; p++) {
         rmatind[p] = varindex (m, p, IS_USED);
         rmatval[p] = 1.0;
      }
      rhs[0]   = 3.0;
      sense[0] = 'L';
      status = CPXaddrows (env, lp, 0, 1, NUMPRODUCTS, rhs,
                           sense, rmatbeg, rmatind, rmatval,
                           NULL, NULL);
      if ( status ) {
         fprintf (stderr, "Could not add new rows.\n");
         goto TERMINATE;
      }

      /* If product veg 1 or veg 2 is used then oil 3 must be used */

      indicator  = varindex (m, VEGOIL1, IS_USED);
      rmatind[0] = varindex (m, OIL3, USE);
      rmatval[0] = 1.0;
      status = CPXaddindconstr (env, lp, indicator, 0, 1, 20.0, 'G',
                                rmatind, rmatval, NULL);

      indicator = varindex (m, VEGOIL2, IS_USED);
      status = CPXaddindconstr (env, lp, indicator, 0, 1, 20.0, 'G',
                                rmatind, rmatval, NULL);

      if ( status ) {
         fprintf (stderr, "Could not add new indicator constraint.\n");
         goto TERMINATE;
      }

      /* We can store each product from one month to the next,
         starting with a stock of 500 tons */

      for (p = 0; p < NUMPRODUCTS; p++) {
         int n = 0;
         if ( m != 0 ) {
            rmatind[n]   = varindex (m-1, p, STORE); /* stored last month */
            rmatval[n++] = 1.0;
            rmatind[n]   = varindex (m, p, BUY);     /* bought this month */
            rmatval[n++] = 1.0;
            rmatind[n]   = varindex (m, p, USE);     /* used this month */
            rmatval[n++] = -1.0;
            rmatind[n]   = varindex (m, p, STORE);   /* stored this month */
            rmatval[n++] = -1.0;
            rhs[0]       = 0.0;
            sense[0]     = 'E';
         }
         else {
            rmatind[n]   = varindex (m, p, BUY);   /* bought this month */
            rmatval[n++] = 1.0;
            rmatind[n]   = varindex (m, p, USE);   /* used this month */
            rmatval[n++] = -1.0;
            rmatind[n]   = varindex (m, p, STORE); /* stored this month */
            rmatval[n++] = -1.0;
            rhs[0]       = -500.0;
            sense[0]     = 'E';
         }
         status = CPXaddrows (env, lp, 0, 1, n, rhs,
                              sense, rmatbeg, rmatind, rmatval,
                              NULL, NULL);
         if ( status ) {
            fprintf (stderr, "Could not add new rows.\n");
            goto TERMINATE;
         }
      }
   }

 TERMINATE:

   free_and_null ((char **)&obj);
   free_and_null ((char **)&lb);
   free_and_null ((char **)&ub);
   free_and_null ((char **)&ctype);
   free_and_null ((char **)&rmatind);
   free_and_null ((char **)&rmatval);

   return (status);

}  /* END buildmodel */


/* A function to map month, product and which variable to an index */

static int
varindex (int m, int p, int whichvar)
{
   return (m*NUMVARS*NUMPRODUCTS + p*NUMVARS + whichvar);
} /* END varindex */


/* This simple routine frees up the pointer *ptr, and sets *ptr to NULL */

static void
free_and_null (char **ptr)
{
   if ( *ptr != NULL ) {
      free (*ptr);
      *ptr = NULL;
   }
} /* END free_and_null */


