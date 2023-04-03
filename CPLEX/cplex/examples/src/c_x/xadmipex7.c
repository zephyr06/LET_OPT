/* --------------------------------------------------------------------------
 * File: xadmipex7.c
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

/* xadmipex7.c - Use the solve callback to do a combined dual-
                barrier solve.  This example shows how to obtain
                the functionality of CPX_NODEALG_DUAL_HYBBAROPT,
                which has been removed. */

/* To run this example, command line arguments are required:
       xadmipex7 filename
   where 
       filename  Name of the file, with .mps, .lp, or .sav
                 extension, and a possible additional .gz 
                 extension.
   Example:
       xadmipex7  mexample.mps */

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

static void
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
   
   CPXENVptr env = NULL;
   CPXLPptr  lp = NULL;

   int nameind = 1;

   /* Check the command line arguments */

   if ( argc != 2 ) {
      usage (argv[0]);
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


   /* Turn on traditional search for use with control callbacks */

   status = CPXXsetintparam (env, CPXPARAM_MIP_Strategy_Search,
                             CPX_MIPSEARCH_TRADITIONAL);
   if ( status )  goto TERMINATE;

   /* Set up to use MIP callbacks */

   status = CPXXsetsolvecallbackfunc (env, usersolve, NULL);
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



TERMINATE:

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

   (void)cbhandle; /* Avoid compiler warnings about unused parameters. */

   *useraction_p = CPX_CALLBACK_DEFAULT;

   /* Get pointer to LP subproblem */

   status = CPXXgetcallbacknodelp (env, cbdata, wherefrom, &nodelp);
   if ( status )  goto TERMINATE;

   /* Find out what node is being processed */

   status = CPXXgetcallbackinfo (env, cbdata, wherefrom,
                                CPX_CALLBACK_INFO_NODE_COUNT_LONG,
                                &nodecount);
   if ( status )  goto TERMINATE;

   /* Solve initial node with barrier with crossover */

   if ( nodecount < 1 ) {
       status = CPXXhybbaropt (env, nodelp, CPX_ALG_AUTOMATIC);
   }
   else {
      CPXCNT itlimit;
      int solstat;

      /* Limit the number of iterations for dual to 5
         The 'const' on the environment must be cast away
         to change a parameter.  Generally, parameter changes
         in a callback should not be made since they will
         have unpredictable effects.  This one, because it
         only controls a limit on an LP optimizer, is OK. */ 

      status = CPXXgetcntparam (env, CPXPARAM_Simplex_Limits_Iterations,
                                &itlimit);
      if ( status )  goto TERMINATE;

      status = CPXXsetcntparam ((CPXENVptr) env,
                                CPXPARAM_Simplex_Limits_Iterations, 200);
      if ( status )  goto TERMINATE;

      status = CPXXdualopt (env, nodelp);

      CPXXsetcntparam ((CPXENVptr) env, CPXPARAM_Simplex_Limits_Iterations,
                       itlimit);

      if ( !status ) {
         solstat = CPXXgetstat (env, nodelp);
         if ( solstat == CPX_STAT_ABORT_IT_LIM ) {
            int preind;
            int aggind;

            /* Set parameters to get a full presolve on the node lp */

            status = CPXXgetintparam (env, CPXPARAM_Preprocessing_Presolve,
                                      &preind);
            if ( status )  goto TERMINATE;
            status = CPXXgetintparam (env, CPXPARAM_Preprocessing_Aggregator,
                                      &aggind);
            if ( status )  goto TERMINATE;

            status = CPXXsetintparam ((CPXENVptr) env,
                                      CPXPARAM_Preprocessing_Presolve, CPX_ON);
            if ( status )  goto TERMINATE;
            status = CPXXsetintparam ((CPXENVptr) env,
                                      CPXPARAM_Preprocessing_Aggregator, -1);
            if ( status )  goto TERMINATE;

            status = CPXXhybbaropt (env, nodelp, CPX_ALG_AUTOMATIC);

            CPXXsetintparam ((CPXENVptr) env, CPXPARAM_Preprocessing_Presolve,
                             preind);
            CPXXsetintparam ((CPXENVptr) env,
                             CPXPARAM_Preprocessing_Aggregator, aggind);
         }
      }
   }
 
   /* If the solve was OK, set return to say optimization has
      been done in callback, otherwise return the CPLEX error
      code */

   if ( !status )  *useraction_p = CPX_CALLBACK_SET;

TERMINATE:

   return (status);

} /* END usersolve */


