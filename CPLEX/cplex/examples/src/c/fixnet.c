/* --------------------------------------------------------------------------
 * File: fixnet.c
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

/* fixnet.c - Use indicator constraints to avoid numerical trouble in a
              fixed charge network flow problem. */

/*
  Find a minimum cost flow in a fixed charge network flow problem.
  The network is as follows:

       1 -- 3 ---> demand = 1,000,000
      / \  /
     /   \/
    0    /\
     \  /  \
      \/    \
       2 -- 4 ---> demand = 1

  A fixed charge of one is incurred for every edge with non-zero flow,
  with the exception of edge <1,4>, which has a fixed charge of ten.
  The cost per unit of flow on an edge is zero, with the exception of
  edge <2,4>, where the cost per unit of flow is five.
*/


/* Bring in the CPLEX function declarations and the C library 
   header file stdio.h with the include of cplex.h. */

#include <ilcplex/cplex.h>

/* Bring in the declarations for the string functions */

#include <string.h>
#include <stdlib.h>

/* Include declarations for functions at end of program */
static int
   buildnetwork (CPXENVptr env, CPXLPptr lp),
   dumpx (CPXENVptr env, CPXLPptr lp);


/* The network has 5 nodes and 6 edges. */
#define NUMNODES 5
#define NUMEDGES 6

/* Define origin and destination nodes for each edge, as well as
   unit costs and fixed costs for transporting flow on each edge.
   Note that by defining a fixed cost of 0 for each arc you just
   get a regular min-cost flow problem. */
static int const    orig[]      = {0,0,1,1,2,2};
static int const    dest[]      = {1,2,3,4,3,4};
static double const unitcost[]  = {0,0,0,0,0,5};
static double const fixedcost[] = {1,1,1,10,1,1};


/* Define demand (supply) at each node. */
static double const demand[] = {-1000001, 0, 0, 1000000, 1};


int
main (void)
{
   /* Declare variables and arrays where we will store the
      optimization results including the status, objective value,
      and variable values. */

   int      solstat;
   double   objval;
   double   x[2*NUMEDGES]; /* One flow variable and one fixed charge indicator
                              for each edge */

   CPXENVptr env = NULL;
   CPXLPptr  lp = NULL;
   int       status;
   int       j;


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


   /* Create the problem. */

   lp = CPXcreateprob (env, &status, "fixnet");

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

   /* Build the fixed-charge network flow model using
      indicator constraints. */

   status = buildnetwork (env, lp);
   if ( status ) {
      fprintf (stderr, "Failed to build network.\n");
      goto TERMINATE;
   }

   /* Optimize the problem and obtain solution. */

   status = CPXmipopt (env, lp);
   if ( status ) {
      fprintf (stderr, "Failed to optimize MIP.\n");
      goto TERMINATE;
   }

   solstat = CPXgetstat (env, lp);


   /* Write solution status and objective to the screen. */

   printf ("\nSolution status = %d\n", solstat);
 
   status = CPXgetobjval (env, lp, &objval);
   if ( status ) {
      fprintf (stderr, "No MIP objective value available.  Exiting...\n");
      goto TERMINATE;
   }

   printf ("Solution value  = %f\n", objval);
   printf ("Solution vector:\n");
   dumpx (env, lp);

   status = CPXgetx (env, lp, x, 0, 2*NUMEDGES-1);
   if ( status ) {
      fprintf (stderr, "Failed to get optimal integer x.\n");
      goto TERMINATE;
   }

   /* Make sure flow satisfies fixed-charge constraints */

   for (j = 0; j < NUMEDGES; j++) {
      if ( x[j] > 0.0001 && x[NUMEDGES+j] < 0.9999 ) {
         printf ("WARNING : Edge from %d to %d has non-zero flow %.3f\n",
                 orig[j], dest[j], x[j]);
         printf ("        : fixed-charge indicator has value %.6f.\n",
                 x[NUMEDGES+j]);
      }
   }
   printf("\n");

   /* Finally, write a copy of the problem to a file. */

   status = CPXwriteprob (env, lp, "fixnet.lp", NULL);
   if ( status ) {
      fprintf (stderr, "Failed to write LP to disk.\n");
      goto TERMINATE;
   }

   /* Free problem */

   status = CPXfreeprob (env, &lp);
   if ( status ) {
      fprintf (stderr, "CPXfreeprob failed, error code %d.\n", status);
      goto TERMINATE;
   }

TERMINATE:

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


/*  Build the fixed-charge network flow problem using indicator constraints. */

static int
buildnetwork (CPXENVptr env, CPXLPptr lp)
{
   char     sense[NUMNODES];
   double   rhs[NUMNODES];
   int      ind[2];
   double   val[2];
   char     *name[1];
   char     buffer[100];

   int      i, j, varindex;
   int      zero = 0;
   double   dblzero = 0.0;
   double   dblone  = 1.0;
   char     binary = 'B';
   int      status = 0;

   /* Create constraint placeholders ---
        One constraint for each node (flow constraints)
   */

   for (i = 0; i < NUMNODES; i++) {
      rhs[i]   = demand[i];
      sense[i] = 'G';
   }
   status = CPXnewrows (env, lp, NUMNODES, rhs, sense, NULL, NULL);
   if ( status ) {
      fprintf (stderr, "Could not create new rows.\n");
      goto TERMINATE;
   }

   /* Add flow variables */

   for (j = 0; j < NUMEDGES; j++) {
      ind[0] = orig[j];  /* Flow leaves origin */
      val[0] = -1.0;
      ind[1] = dest[j];  /* Flow arrives at destination */
      val[1] =  1.0;

      name[0] = buffer;
      sprintf(buffer, "x%d%d", orig[j], dest[j]);

      status = CPXaddcols (env, lp, 1, 2, &unitcost[j], &zero, ind, val,
                           NULL, NULL, name);
      if ( status ) {
         fprintf (stderr, "Failed to add flow edge.\n");
         goto TERMINATE;
      }
   }

   /* Add fixed charge variables */
   for (j = 0; j < NUMEDGES; j++) {
      name[0] = buffer;
      sprintf(buffer, "f%d%d", orig[j], dest[j]);

      status = CPXaddcols (env, lp, 1, 0, &fixedcost[j], &zero, ind, val,
                           &dblzero, &dblone, name);
      if ( status ) {
         fprintf (stderr, "Failed to add fixed charge variable.\n");
         goto TERMINATE;
      }

      varindex = NUMEDGES+j;
      status = CPXchgctype (env, lp, 1, &varindex, &binary);
      if ( status ) {
         fprintf (stderr, "Failed to change variable type.\n");
         goto TERMINATE;
      }
      
   }

   /* Add indicator constraints --
        f = 0 -> x <= 0 */

   for (j = 0; j < NUMEDGES; j++) {
      varindex = j;
      sprintf(buffer, "indicator%d", j);

      status = CPXaddindconstr (env, lp, NUMEDGES+j, 1, 1, 
                                0.0, 'L', &varindex, &dblone, buffer);
      if ( status ) {
         fprintf (stderr, "Failed to add indicator constraint.");
         goto TERMINATE;
      }
   }
   
TERMINATE:

   return (status);

}  /* END buildnetwork */


/* Dump the x vector to stdout. */

static int
dumpx (CPXENVptr env, CPXLPptr lp)
{
   int cols, c, surplus, status = 0;
   char *name, buffer[8];
   double x;


   cols = CPXgetnumcols (env, lp);
   for (c = 0; c < cols; ++c) {
      status = CPXgetx (env, lp, &x, c, c);
      if ( status ) {
         fprintf (stderr,
                  "Failed to read value for column %d: %d\n", c, status);
         goto TERMINATE;
      }

      status = CPXgetcolname (env, lp, &name, buffer, sizeof (buffer),
                              &surplus, c, c);
      if ( status ) {
         fprintf (stderr,
                  "Failed to read name for column %d: %d\n", c, status);
         goto TERMINATE;
      }

      printf ("%8s: %15.6f\n", name, x);
   }

 TERMINATE:

   return status;
} /* END dumpx */
