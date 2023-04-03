/* --------------------------------------------------------------------------
 * File: netex1.c
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

/* netex1.c - Entering and optimizing a network problem */

/* Import the CPLEX function declarations and the C library 
   header file stdio.h with the include of cplex.h. */

#include <ilcplex/cplex.h>
#include <stdlib.h>

/* Import the declarations for the string functions */

#include <string.h>



/* Forward declaration for function at end of program */

static int
   buildNetwork  (CPXENVptr env, CPXNETptr net);

static void
   free_and_null (char **ptr);


int
main (void)
{
   /* Declare variables and arrays for retrieving problem data and
      solution information later on. */

   int      narcs;
   int      nnodes;
   int      solstat;
   double   objval;
   double   *x     = NULL;
   double   *pi    = NULL;
   double   *slack = NULL;
   double   *dj    = NULL;

   CPXENVptr env = NULL;
   CPXNETptr net = NULL;
   int       status;
   int       i, j;

   /* Initialize the CPLEX environment */

   env = CPXopenCPLEX (&status);

   /* If an error occurs, the status value indicates the reason for
      failure.  A call to CPXgeterrorstring will produce the text of
      the error message.  Note that CPXopenCPLEX produces no
      output, so the only way to see the cause of the error is to use
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

   net = CPXNETcreateprob (env, &status, "netex1");

   /* A returned pointer of NULL may mean that not enough memory
      was available or there was some other problem.  In the case of 
      failure, an error message will have been written to the error 
      channel from inside CPLEX.  In this example, the setting of
      the parameter CPXPARAM_ScreenOutput causes the error message to
      appear on stdout.  */

   if ( net == NULL ) {
      fprintf (stderr, "Failed to create network object.\n");
      goto TERMINATE;
   }

   /* Fill in the data for the problem.  Note that since the space for
      the data already exists in local variables, we pass the arrays
      directly to the routine to fill in the data structures.  */

   status = buildNetwork (env, net);

   if ( status ) {
      fprintf (stderr, "Failed to build network problem.\n");
      goto TERMINATE;
   }


   /* Optimize the problem and obtain solution. */

   status = CPXNETprimopt (env, net);
   if ( status ) {
      fprintf (stderr, "Failed to optimize network.\n");
      goto TERMINATE;
   }

   /* get network dimensions */

   narcs  = CPXNETgetnumarcs  (env, net);
   nnodes = CPXNETgetnumnodes (env, net);

   /* allocate memory for solution data */

   x     = (double *) malloc (narcs  * sizeof (double));
   dj    = (double *) malloc (narcs  * sizeof (double));
   pi    = (double *) malloc (nnodes * sizeof (double));
   slack = (double *) malloc (nnodes * sizeof (double));

   if ( x     == NULL ||
        dj    == NULL ||
        pi    == NULL ||
        slack == NULL   ) {
      fprintf (stderr, "Failed to allocate arrays.\n");
      goto TERMINATE;
   }

   status = CPXNETsolution (env, net, &solstat, &objval, x, pi, slack, dj);
   if ( status ) {
      fprintf (stderr, "Failed to obtain solution.\n");
      goto TERMINATE;
   }

   /* Write the output to the screen. */

   printf ("\nSolution status = %d\n", solstat);
   printf ("Solution value  = %f\n\n", objval);

   for (i = 0; i < nnodes; i++) {
      printf ("Node %2d:  Slack = %10f  Pi = %10f\n", i, slack[i], pi[i]);
   }

   for (j = 0; j < narcs; j++) {
      printf ("Arc  %2d:  Value = %10f  Reduced cost = %10f\n",
              j, x[j], dj[j]);
   }

   /* Finally, write a copy of the problem to a file. */

   status = CPXNETwriteprob (env, net, "netex1.net", NULL);
   if ( status ) {
      fprintf (stderr, "Failed to write network to disk.\n");
      goto TERMINATE;
   }
   
   
TERMINATE:

   /* Free memory for solution data */

   free_and_null ((char **) &x);
   free_and_null ((char **) &dj);
   free_and_null ((char **) &pi);
   free_and_null ((char **) &slack);

   /* Free up the problem as allocated by CPXNETcreateprob, if necessary */

   if ( net != NULL ) {
      status = CPXNETfreeprob (env, &net);
      if ( status ) {
         fprintf (stderr, "CPXNETfreeprob failed, error code %d.\n", status);
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



static int
buildNetwork (CPXENVptr env, CPXNETptr net)
{
   int status = 0;

   /* definitions to improve readability */

#  define NNODES  8
#  define NARCS  14
#  define inf    CPX_INFBOUND

   /* Define list of supply values for the nodes */

   double supply[NNODES] = {20.0, 0.0, 0.0, -15.0, 5.0, 0.0, 0.0, -10.0};

   /* Define list of tail or from-node indices as well as head or
      to-node indices for the arcs.  Notice that according to C
      standard the first node has index 0. */

   int    tail[NARCS] = {   0,    1,    2,    3,    6,    5,    4,
                            4,    2,    3,    3,    5,    5,    1};
   int    head[NARCS] = {   1,    2,    3,    6,    5,    7,    7,
                            1,    1,    4,    5,    3,    4,    5};

   /* Define list of objective values and lower and upper bound values
      for the arcs */

   double obj [NARCS] = { 3.0,  3.0,  4.0,  3.0,  5.0,  6.0,  7.0,
                          4.0,  2.0,  6.0,  5.0,  4.0,  3.0,  6.0};
   double ub  [NARCS] = {24.0, 25.0, 12.0, 10.0,  9.0,  inf, 20.0,
                         10.0,  5.0, 15.0, 10.0, 11.0,  6.0,  inf};
   double lb  [NARCS] = {18.0,  0.0, 12.0,  0.0,  0.0, -inf,  0.0,
                          0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0};

   /* Delete existing network.  This is not necessary in this
      context since we know we have an empty network object.
      Notice that CPXNETdelnodes deletes all arcs incident to
      the deleted nodes as well.  Therefore this one function
      call effectively deletes an existing network problem. */

   if ( CPXNETgetnumnodes (env, net) > 0 ) {
      status = CPXNETdelnodes (env, net, 0,
                               CPXNETgetnumnodes (env, net)-1);
      if ( status ) goto TERMINATE;
   }

   /* Set optimization sense */

   status = CPXNETchgobjsen (env, net, CPX_MIN);
   if ( status ) goto TERMINATE;

   /* Add nodes to network along with their supply values,
      but without any names. */

   status = CPXNETaddnodes (env, net, NNODES, supply, NULL);
   if ( status ) goto TERMINATE;

   /* Add arcs to network along with their objective values and
      bounds, but without any names. */

   status = CPXNETaddarcs (env, net, NARCS, tail, head, lb, ub, obj, NULL);
   if ( status ) goto TERMINATE;

TERMINATE:

   return (status);

}  /* END buildnetwork */


static void
free_and_null (char **ptr)
{
   if ( *ptr != NULL ) {
      free (*ptr);
      *ptr = NULL;
   }
} /* END free_and_null */
