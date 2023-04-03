/* --------------------------------------------------------------------------
 * File: xgenericbranch.c
 * Version 22.1.0
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of IBM
 * 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
 * Copyright IBM Corporation 2019, 2022. All Rights Reserved.
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with
 * IBM Corp.
 * --------------------------------------------------------------------------
 */

/* Demonstrates how to perform customized branching using the generic
   callback.

   For any model with integer variables passed on the command line, the
   code will solve the model using a simple customized branching strategy.
   The branching strategy implemented here is most-infeasible branching,
   i.e., the code always picks the integer variable that is most fractional
   and then branches on it. If the biggest fractionality of all integer
   variables is small then the code refrains from custom branching and lets
   CPLEX decide.

   See the usage message below.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ilcplex/cplexx.h>

/* Data that is used within the callback. */
typedef struct {
   char *ctype;    /* Variable types. */
   CPXDIM cols;    /* Number of variables. */
   int nthreads;   /* Maximum number of threads we can handle. */
   double **relx;  /* Per-thread buffer for reading variable values. */
   int calls;      /* How often was the callback invoked? */
   int branches;   /* How many branches did we create */
} CALLBACKDATA;


/* Print a usage message and exit. */
static void
usage (const char *progname)
{
   fprintf(stderr, "Usage: %s filename...\n", progname);
   fprintf(
      stderr,
      "  filename   Name of a file, or multiple files, with .mps, .lp,\n"
      "             or .sav extension, and a possible, additional .gz\n"
      "             extension.\n");
   exit(2);
}

/* Generic callback that implements most infeasible branching. */
static int CPXPUBLIC
branchcallback (CPXCALLBACKCONTEXTptr context, CPXLONG contextid,
                void *cbhandle)
{
   CALLBACKDATA *data = cbhandle;
   CPXINT threadid;
   CPXLONG depth;
   int status, lpstat;
   double obj;

   (void)contextid; /* unused */

   /* NOTE: Strictly speaking, the increment of data->calls and data->branches
    *       should be protected by a lock/mutex/semaphore. However, to keep
    *       the code simple we don't do that here.
    */
   data->calls++;

   /* Get the id of the thread on which the callback is invoked.
    * We need this to pick the right buffer in data->relx.
    */
   status = CPXXcallbackgetinfoint (context, CPXCALLBACKINFO_THREADID,
                                    &threadid);
   if ( status ) {
      fprintf (stderr, "Failed to get thread id: %d\n", status);
      return status;
   }

   /* For sake of illustration prune every node that has a depth larger
    * than 1000.
    */
   status = CPXXcallbackgetinfolong (context, CPXCALLBACKINFO_NODEDEPTH,
                                     &depth);
   if ( status ) {
      fprintf (stderr, "Failed to get node depth: %d\n", status);
      return status;
   }
   if ( depth > 1000 ) {
      status = CPXXcallbackprunenode (context);
      if ( status ) {
         fprintf (stderr, "Failed to prune node: %d\n", status);
         return status;
      }

      return 0;
   }

   /* Get the solution status of the current relaxation.
    */
   status = CPXXcallbackgetrelaxationstatus (context, &lpstat, 0);
   if ( status ) {
      fprintf (stderr, "Failed to get relaxation status: %d\n", status);
      return status;
   }

   /* Only branch if the current node relaxation could be solved to optimality.
    * If there was any sort of trouble then don't do anything and thus let
    * CPLEX decide how to cope with that.
    */
   if ( lpstat == CPX_STAT_OPTIMAL        ||
        lpstat == CPX_STAT_OPTIMAL_INFEAS   ) {
      /* Relaxation is optimal. Find the integer variable that is most
       * fractional.
       */
      CPXDIM j;
      double maxfrac = 0.0;
      CPXDIM maxvar = -1;

      status = CPXXcallbackgetrelaxationpoint (context, data->relx[threadid],
                                               0, data->cols - 1, &obj);
      if ( status ) {
         fprintf (stderr, "Failed to get relaxation: %d\n", status);
         return status;
      }

      for (j = 0; j < data->cols; ++j) {
         if ( data->ctype[j] != CPX_CONTINUOUS &&
              data->ctype[j] != CPX_SEMICONT )
         {
            double intval = round (data->relx[threadid][j]);
            double frac = fabs (intval - data->relx[threadid][j]);

            if ( frac > maxfrac ) {
               maxfrac = frac;
               maxvar = j;
            }
         }
      }

      /* If the maximum fractionality of all integer variables is small then
       * don't create a custom branch. Instead let CPLEX decide how to branch.
       */
      if ( maxfrac > 0.1 ) {
         /* There is a variable with a sufficiently fractional value.
          * Branch on that variable.
          */
         CPXLONG downchild, upchild;
         double const up = ceil (data->relx[threadid][maxvar]);
         double const down = floor (data->relx[threadid][maxvar]);

         /* Create the UP branch. */
         status = CPXXcallbackmakebranch (context, 1, &maxvar, "L", &up,
                                          0, 0, NULL, NULL, NULL, NULL, NULL,
                                          obj, &upchild);
         if ( status ) {
            fprintf (stderr, "Failed to create up branch: %d\n", status);
            return status;
         }
         data->branches++;

         /* Create the DOWN branch. */
         status = CPXXcallbackmakebranch (context, 1, &maxvar, "U", &down,
                                          0, 0, NULL, NULL, NULL, NULL, NULL,
                                          obj, &downchild);
         if ( status ) {
            fprintf (stderr, "Failed to create down branch: %d\n", status);
            return status;
         }
         data->branches++;

         /* We don't use the unique ids of the down and up child. We only
          * have them so illustrate how they are returned from
          * CPXcallbackmakebranch().
          */
         (void)downchild;
         (void)upchild;
      }
   }

   return 0;
} /* END branchcallback */

int
main (int argc, char *argv[])
{
   int a;

   if ( argc <= 1 )
      usage (argv[0]);

   /* Loop over all command line arguments. */
   for (a = 1; a < argc; ++a) {
      int status = 0;
      CPXENVptr env;
      CPXLPptr lp;
      int t;
      double objval;
      CALLBACKDATA data = { NULL, 0, 0, NULL, 0, 0 };

      /* Create CPLEX environment and immediately enable screen output so that
       * we can see potential error messages etc.
       */
      env = CPXXopenCPLEX (&status);
      if ( env == NULL || status ) {
         fprintf (stderr, "Failed to create environment: %d\n", status);
         goto TERMINATE;
      }
      status = CPXXsetintparam (env, CPXPARAM_ScreenOutput, CPX_ON);
      if ( status ) {
         fprintf  (stderr, "Failed to turn enable screen output: %d\n", status);
         goto TERMINATE;
      }

      /* Read the model file into a newly created problem object. */
      lp = CPXXcreateprob (env, &status, "");
      if ( lp == NULL || status ) {
         fprintf (stderr, "Failed to create problem object: %d\n", status);
         goto TERMINATE;
      }
      status = CPXXreadcopyprob (env, lp, argv[a], NULL);
      if ( status ) {
         fprintf (stderr, "Failed to read %s: %d\n", argv[a], status);
         goto TERMINATE;
      }

      /* Read the column types into the data that we will pass into the
       * callback.
       */
      data.cols = CPXXgetnumcols (env, lp);
      data.ctype = malloc (data.cols * sizeof(*data.ctype));
      if ( data.ctype == NULL ) {
         status = CPXERR_NO_MEMORY;
         fprintf (stderr, "Out of memory\n");
         goto TERMINATE;
      }
      status = CPXXgetctype (env, lp, data.ctype, 0, data.cols - 1);
      if ( status ) {
         fprintf (stderr, "Failed to query variable types: %d\n", status);
         goto TERMINATE;
      }

      /* For each potential thread that CPLEX may use, create a buffer into
       * which the callback can read the current relaxation.
       * We don't change the CPXPARAM_Threads parameter, so CPLEX will use
       * up to as many threads as there are cores.
       */
      status = CPXXgetnumcores (env, &data.nthreads);
      if ( status ) {
         fprintf (stderr, "Failed to get number of cores: %d\n", status);
         goto TERMINATE;
      }
      data.relx = calloc (data.nthreads, sizeof(*data.relx));
      if ( data.relx == NULL ) {
         status = CPXERR_NO_MEMORY;
         fprintf (stderr, "Out of memory\n");
         goto TERMINATE;
      }
      for (t = 0; t < data.nthreads; ++t) {
         data.relx[t] = malloc (data.cols * sizeof(*data.relx[t]));
         if ( data.relx[t] == NULL ) {
            status = CPXERR_NO_MEMORY;
            goto TERMINATE;
         }
      }

      /* Register the callback function. */
      status = CPXXcallbacksetfunc(env, lp, CPX_CALLBACKCONTEXT_BRANCHING,
                                   branchcallback, &data);
      if ( status ) {
         fprintf (stderr, "Failed to set callback: %d\n", status);
         goto TERMINATE;
      }

      /* Limit the number of nodes.
       * The branching strategy implemented here is not smart so solving even
       * a simple MIP may turn out to take a long time.
       */
      status = CPXXsetcntparam (env, CPXPARAM_MIP_Limits_Nodes, 1000);
      if ( status ) {
         fprintf (stderr, "Failed to set node limit: %d\n", status);
         goto TERMINATE;
      }

      /* Solve the model. */
      status = CPXXmipopt (env, lp);
      if ( status ) {
         fprintf (stderr, "Optimization failed: %d\n", status);
         goto TERMINATE;
      }

      /* Report some statistics. */
      printf ("Model %s solved, solution status = %d\n", argv[a], CPXXgetstat (env, lp));
      status = CPXXgetobjval (env, lp, &objval);
      if ( status )
         printf ("No objective value (error = %d\n", status);
      else
         printf ("Objective = %f\n", objval);
      printf ("Callback was invoked %d times and created %d branches\n",
              data.calls, data.branches);

   TERMINATE:
      /* Cleanup */
      if ( data.relx ) {
         for (t = 0; t < data.nthreads; ++t)
            free (data.relx[t]);
         free (data.relx);
      }
      free (data.ctype);
      CPXXfreeprob (env, &lp);
      CPXXcloseCPLEX (&env);
      if ( status )
         return status;
   }

   return 0;
} /* END main */
