/* --------------------------------------------------------------------------
 * File: xadmipex8.c
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

/* xadmipex8.c -- Solve a capacitated facility location problem with cutting
 * planes using the new generic callback interface.
 *
 * We are given a set of candidate locations J and a set of clients C.
 * Facilities should be opened in some of the candidate locations so that the
 * clients' demand can be served. The facility location problem consists of
 * deciding in which locations facilities should be opened and assigning each
 * client to a facility at a minimum cost.
 *
 * A fixed cost is incurred when opening a facility, and a linear cost is
 * associated with the demand supplied from a given facility to a client.
 *
 * Furthermore, each facility has a capacity and can only serve |C| - 1
 * clients. Note that in the variant of the problem considered here, each
 * client is served by only one facility.
 *
 * The problem is formulated as a mixed integer linear program using binary
 * variables: used[j], for all locations j in J, indicating if a facility is
 * opened in location j; supply[c][j], for each client c in C and facility j
 * in J, indicating the demand of client c supplied from facility j.
 *
 * Thus, the following model formulates the facility location problem:
 *
 *  Minimize sum(j in J) fixedCost[j] * used[j] +
 *           sum(j in J) sum(c in C) cost[c][j] * supply[c][j]
 *  Subject to:
 *    sum(j in J) supply[c][j] == 1                    for all c in C,
 *    sum(c in C) supply[c][j] <= (|C| - 1) * used[j]  for all j in J,
 *    supply[c][j] in {0, 1}                   for all c in C, j in J,
 *    used[j] in {0, 1}                                for all j in J.
 *
 * The first set of constraints are the demand constraints ensuring that the
 * demand of each client is satisfied. The second set of constraints are the
 * capacity constraints, ensuring that if a facility is placed in location j
 * the capacity of that facility is not exceeded.
 *
 * The program in this file, formulates a facility location problem and
 * solves it. Furthermore, different cutting planes methods are implemented
 * using the generic callback API to help the solution of the problem:
 *
 * - Disaggregated capacity cuts separated algorithmically (see function
 *   disaggregatecutsep for details),
 *
 * - Disagregated capacity cuts separated using a cut table (see function
 *   cutsfromtable),
 *
 * - Capacity constraints separated as lazy constraints (see function
 *   lazycapacity).
 *
 * Those different methods are invoked using the generic callback API.
 *
 * See the usage message for how to switch between these different options.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ilcplex/cplexx.h>


/* Problem data. */
#define LOCATIONS 5
#define CLIENTS 8
static double const fixedcost[LOCATIONS]     = {   480, 200, 320, 340, 300 };
static double const cost[CLIENTS][LOCATIONS] = { {  24,  74,  31,  51,  84 },
                                                 {  57,  54,  86,  61,  68 },
                                                 {  57,  67,  29,  91,  71 },
                                                 {  54,  54,  65,  82,  94 },
                                                 {  98,  81,  16,  61,  27 },
                                                 {  13,  92,  34,  94,  87 },
                                                 {  54,  72,  41,  12,  78 },
                                                 {  54,  64,  65,  89,  89 } };

/* Some macros to handle variables more easily.
 * used() and supply() map location and client indices to the respective index
 * of the appropriate variable in the model. That is, for an array of size
 * NVARS, the first j elements are for "used" variables, and the remaining
 * c * j elements are for "supply" variables. In comments below "used[j]"
 * and "supply[c][j]" are shorthand for x[used(j)] and x[supply(c, j)],
 * respectively.
 */
#define used(j) (j)
#define supply(c,j) (LOCATIONS + (c) * LOCATIONS + (j))
#define NVARS (LOCATIONS + LOCATIONS * CLIENTS)

/* Value of epsilon used for violation of cuts. */
#define EPS 1e-6

/* Separate the disaggregated capacity constraints.
 * In the model we have for each location j the constraint
 *    sum(c in clients) supply[c][j] <= (|C| -1) * used[j]
 * Clearly, a client can only be serviced from a location that is used,
 * so we also have a constraint
 *    supply[c][j] <= used[j]
 * that must be satisfied by every feasible solution. These constraints tend
 * to be violated in LP relaxation. In this callback we separate them.
 *
 * Note the context argument that is there because this function will
 * be called within a callback.
 */
static int
disaggregatecutsep(CPXCALLBACKCONTEXTptr context, const char *const*cname)
{
   int status;
   int j, c;
   double x[NVARS];

   status = CPXXcallbackgetrelaxationpoint (context, x, 0, NVARS - 1, NULL);
   if ( status != 0 ) return status;

   /* For each j and c check whether in the current solution (obtained by
    * calls to CPXXgetcallbacknodex()) we have supply[c][j] > used[j]. If so,
    * then we have found a violated constraint and add it as a cut.
    */
   for (j = 0; j < LOCATIONS; ++j) {
      for (c = 0; c < CLIENTS; ++c) {
         double s = x[supply(c, j)];
         double o = x[used(j)];
         if ( s > o + EPS) {
            /* We add the cut:
             *    supply(c,j) <= used(j)
             */
            CPXDIM cutind[2] = { supply(c, j), used(j) };
            double cutval[2] = { 1.0, -1.0 };
            CPXCNT beg       = 0;
            char   sense     = 'L';
            double rhs       = 0.0;
            int    purgeable = CPX_USECUT_PURGE;
            int    local     = 0;


            printf("Adding %s <= %s [%f > %f]\n",
                   cname[cutind[0]], cname[cutind[1]], s ,o);

            status = CPXXcallbackaddusercuts(context, 1, 2, &rhs,
                                            &sense, &beg, cutind, cutval, &purgeable, &local);
            if ( status != 0 ) return status;
         }
      }
   }
   return 0;
}

/* A simple structure to represent a cut in the table used with cutsfromtable().
 */
typedef struct {
   CPXDIM ind[2];
   double val[2];
   char   sense;
   double rhs;
} CUT;

/* Variant of the disaggregatecutsep function that does not look for violated
 * cuts dynamically. Instead it uses a static table of cuts and scans this
 * table for violated cuts.
 *
 * Note the context argument that is there because this function will
 * be called within a callback.
 */
static int
cutsfromtable(CPXCALLBACKCONTEXTptr context, CUT const *cuts)
{
   double x[NVARS];
   int status;
   CPXDIM i;

   status = CPXXcallbackgetrelaxationpoint (context, x, 0, NVARS - 1, NULL);
   if ( status != 0 ) return status;

   for (i = 0; i < CLIENTS * LOCATIONS; ++i) {
      double lhs = 0.0;
      int k;

      for (k = 0; k < 2; ++k)
         lhs += cuts[i].val[k] * x[cuts[i].ind[k]];
      if ( lhs > cuts[i].rhs + EPS ) {
         CPXCNT beg       = 0;
         int    purgeable = CPX_USECUT_PURGE;
         int    local     = 0;

         printf("Adding cut number %d from table (violated by %g)\n",
                i, lhs - cuts[i].rhs);

         status = CPXXcallbackaddusercuts(context, 1, 2,
                                         &cuts[i].rhs,
                                         &cuts[i].sense,
                                         &beg,
                                         cuts[i].ind,
                                         cuts[i].val, &purgeable, &local);
         if ( status != 0 )
            return status;
      }
   }
   return 0;
}

/* Lazy constraint callback to enforce the capacity constraints.
 *
 * If used then the callback is invoked for every integer feasible solution
 * CPLEX finds. For each location j it checks whether constraint
 *
 *    sum(c in C) supply[c][j] <= (|C| - 1) * used[j]
 *
 * is satisfied. If not then it adds the violated constraint as lazy
 * constraint.
 *
 * Note the context argument that is there because this function will be called
 * within a callback.
 */
static int
lazycapacity(CPXCALLBACKCONTEXTptr context, const char *const *cname)
{
   double x[NVARS];
   int status;
   CPXDIM j, c, k;
   int point;

   status = CPXXcallbackcandidateispoint (context, &point);
   if ( status != 0 ) return status;

   /* We only work with bounded models, so this cannot happen. */
   if ( !point ) return CPXERR_UNSUPPORTED_OPERATION;

   status = CPXXcallbackgetcandidatepoint (context, x, 0, NVARS - 1, NULL);
   if ( status != 0 ) return status;

   for (j = 0; j < LOCATIONS; ++j) {
      double isused = x[used(j)];
      double served = 0.0;  /* Number of clients currently served from j */
      for (c = 0; c < CLIENTS; ++c)
         served += x[supply(c, j)];
      if ( served > (CLIENTS - 1.0) * isused + EPS ) {
         CPXCNT beg = 0;
         CPXDIM cutind[CLIENTS + 1];
         double cutval[CLIENTS + 1];
         char   sense     = 'L';
         double rhs       = 0.0;

         for (c = 0; c < CLIENTS; ++c) {
            cutind[c] = supply(c, j);
            cutval[c] = 1.0;
         }

         cutind[c] = used(j);
         cutval[c] = -(CLIENTS - 1.0);

         printf("Adding lazy constraint");
         for (k = 0; k < CLIENTS + 1; ++k)
            printf(" %+.0f*%s", cutval[k], cname[cutind[k]]);
         printf(" <= 0\n");

         status = CPXXcallbackrejectcandidate(context, 1, CLIENTS + 1, &rhs,
                                        &sense, &beg, cutind, cutval);
         if ( status != 0 ) return status;
      }
   }
   return 0;
}

/* Now this is the final setup for the callback.
 *
 * We define a structure that will store all the data needed within the callback. */
typedef struct callbackdata {
   const CUT *  cuts; /* The table of cuts */
   const char *const* cname; /* Names of the variables */
   int          disaggregate; /* Do we call disaggregate function or look at the table. */
} CALLBACKDATA;

/* Now the callback itself. */
static int CPXPUBLIC
callback (CPXCALLBACKCONTEXTptr context, CPXLONG contextid, void* userdata)
{
   int status = 0;

   /* Recast our user data. */
   CALLBACKDATA *cbdata = (CALLBACKDATA *) userdata;
   const CUT  *  cuts  = cbdata->cuts;
   const char *const * cname = cbdata->cname;
   int disag           = cbdata->disaggregate;

   if ( contextid == CPX_CALLBACKCONTEXT_CANDIDATE ) {
      /* If we are called at a feasible solution we try to separate the capacity constraint
       * as a lazy constraint.
       */
      status = lazycapacity (context, cname);
      if (status != 0) return status;
   }
   else if ( contextid == CPX_CALLBACKCONTEXT_RELAXATION ) {
      /* If we are called with a relaxation solution, we try to generate disaggregated cuts
       * using one of our two functions.
       */
      if ( disag == 1 ) {
         disaggregatecutsep (context, cname);
         if (status != 0) return status;
      }
      else if (cuts != NULL) {
         status = cutsfromtable (context, cuts);
         if (status != 0) return status;
      }
      else {
         fprintf (stderr, "ERROR: Callback was called for cuts but parameters are inconsistent.\n");
         return 1;
      }
   }
   else {
      /* If we are called in another context it is an error */
      fprintf (stderr, "ERROR: Callback called in an unexpected context.\n");
      return 1;
   }
   return status;
}

/* Print a usage message to stderr and abort. */
static void
usage(const char *progname)
{
   fprintf(stderr, "Usage: %s [options...]\n", progname);
   fprintf(stderr,
           " By default, a user cut callback is used to dynamically\n"
           " separate constraints.\n\n"
           " Supported options are:\n"
           "  -table       Instead of the default behavior, use a\n"
           "               static table that holds all cuts and\n"
           "               scan that table for violated cuts.\n"
           "  -no-cuts     Do not separate any cuts.\n"
           "  -lazy        Do not include capacity constraints in the\n"
           "               model. Instead, separate them from a lazy\n"
           "               constraint callback.\n");
   exit(2);
}

/* Adds supply constraints to the model in a batch.
 * In the model, for every location j and client c we add constraints
 * such that:
 *   sum(j in J) supply[c][j] == 1
 */
static int
addsupplyconstraints(CPXENVptr env, CPXLPptr lp)
{
   int c, j;
   double rhs[CLIENTS];
   char sense[CLIENTS];
   CPXCNT rmatbeg[CLIENTS];
   CPXDIM rmatind[CLIENTS * LOCATIONS];
   double rmatval[CLIENTS * LOCATIONS];

   for (c = 0; c < CLIENTS; ++c) {
      rhs[c] = 1.0;
      sense[c] = 'E';
      rmatbeg[c] = c * LOCATIONS;

      for (j = 0; j < LOCATIONS; ++j) {
         rmatind[c * LOCATIONS + j] = supply(c, j);
         rmatval[c * LOCATIONS + j] = 1.0;
      }
   }

   return CPXXaddrows(env, lp, 0, CLIENTS, CLIENTS * LOCATIONS, rhs, sense,
                     rmatbeg, rmatind, rmatval, NULL, NULL);
}

/* Adds capacity constraints to the model in a batch.
 * In the model, for each location j and client c we add constraints
 * such that:
 *   sum(c in C) supply[c][j] <= (|C| - 1) * used[j]
 */
static int
addcapacityconstraints(CPXENVptr env, CPXLPptr lp)
{
   int c, j;
   double rhs[LOCATIONS];
   char sense[LOCATIONS];
   CPXCNT rmatbeg[LOCATIONS];
   CPXDIM rmatind[(CLIENTS + 1) * LOCATIONS];
   double rmatval[(CLIENTS + 1) * LOCATIONS];

   for (j = 0; j < LOCATIONS; ++j) {
      rhs[j] = 0.0;
      sense[j] = 'L';
      rmatbeg[j] = j * (CLIENTS + 1);

      for (c = 0; c < CLIENTS; ++c) {
         rmatind[j * (CLIENTS + 1) + c] = supply(c, j);
         rmatval[j * (CLIENTS + 1) + c] = 1.0;
      }

      rmatind[j * (CLIENTS + 1) + c] = used(j);
      rmatval[j * (CLIENTS + 1) + c] = -(CLIENTS - 1);
   }

   return CPXXaddrows(env, lp, 0, LOCATIONS, (CLIENTS + 1) * LOCATIONS,
                     rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);
}

/* The main function sets up the model, add the callback function and solves the model. */
int
main(int argc, char **argv)
{
   /* CPLEX Environment, problem, error status, error buffer.*/
   CPXENVptr env = NULL;
   CPXLPptr lp = NULL;
   int status = 0;
   char errbuf[CPXMESSAGEBUFSIZE];

   /* Problem data */
   double lb[NVARS], ub[NVARS], obj[NVARS];
   char ctype[NVARS];
   char cnamebuf[NVARS][32];
   char const *cname[NVARS];

   /* Iterators, solution,... */
   CPXDIM i;
   CPXDIM j, c;
   CPXDIM ncuts;
   double tol, objval;
   double x[NVARS];

   /* Data for the callback */
   CALLBACKDATA cbdata = {NULL, NULL, 0};
   CPXLONG      contextmask = 0;
   CUT table[CLIENTS * LOCATIONS];

   /* Parameters: which part of the callback do we use?
    */
   int fromtable = 0;
   int lazy = 0;
   int usecallback = 1;

   /* Parse command line. */
   for (i = 1; i < argc; ++i) {
      if ( strcmp(argv[i], "-table") == 0 )
         fromtable = 1;
      else if ( strcmp(argv[i], "-lazy") == 0 )
         lazy = 1;
      else if ( strcmp(argv[i], "-no-cuts") == 0 )
         usecallback = 0;
      else {
         fprintf(stderr, "Unknown argument %s\n", argv[i]);
         usage(argv[0]);
      }
   }

   /* Create CPLEX environment and model. */
   env = CPXXopenCPLEX(&status);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to open CPLEX: %s\n",
              CPXXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   status = CPXXsetintparam(env, CPXPARAM_ScreenOutput, CPX_ON);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to enable screen output: %s\n",
              CPXXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   lp = CPXXcreateprob(env, &status, "xadmipex8");
   if ( status != 0 ) {
      fprintf(stderr, "Failed to create problem: %s\n",
              CPXXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   /* Create variables.
    * - used[j]      Binary variable indicating if location j is used.
    * - supply[c][j] Amount shipped from location j to client c. This is a
    *                binary variable. It assigne the demand of c
    *                to be served from location i.
    * Note that at the same time we set up the objective function by
    * setting the objective coefficients in obj[].
    */
   for (j = 0; j < LOCATIONS; ++j) {
      lb[used(j)] = 0.0;
      ub[used(j)] = 1.0;
      ctype[used(j)] = 'B';
      obj[used(j)] = fixedcost[j];
      sprintf(cnamebuf[used(j)], "used(%d)", j);
      cname[used(j)] = cnamebuf[used(j)];
      for (c = 0; c < CLIENTS; ++c) {
         lb[supply(c, j)] = 0.0;
         ub[supply(c, j)] = 1.0;
         ctype[supply(c, j)] = 'B';
         obj[supply(c, j)] = cost[c][j];
         sprintf(cnamebuf[supply(c, j)], "supply(%d)(%d)", c, j);
         cname[supply(c, j)] = cnamebuf[supply(c, j)];
      }
   }

   status = CPXXnewcols(env, lp, NVARS, obj, lb, ub, ctype, cname);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to create variables: %s\n",
              CPXXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   /* The supply for each client must sum to 1, i.e., the demand of each
    * client must be met.
    */
   status = addsupplyconstraints(env, lp);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to add supply constraints: %s\n",
              CPXXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   /* Capacity constraint for each location. We just require that a single
    * location cannot serve all clients, that is, the capacity of each
    * location is |C| -1. This makes the model a little harder to
    * solve and allows us to separate more cuts.
    */
   if ( !lazy ) {
      status = addcapacityconstraints(env, lp);
      if ( status != 0 ) {
         fprintf(stderr, "Failed to add capacity constraints: %s\n",
                 CPXXgeterrorstring(env, status, errbuf));
         goto TERMINATE;
      }
   }

   /* Tweak some CPLEX parameters so that CPLEX has a harder time to
    * solve the model and our cut separators can actually kick in.
    */
   if ( (status = CPXXsetintparam(env, CPXPARAM_MIP_Strategy_HeuristicFreq, -1)) ||
        (status = CPXXsetintparam(env, CPXPARAM_MIP_Cuts_MIRCut, -1)) ||
        (status = CPXXsetintparam(env, CPXPARAM_MIP_Cuts_Implied, -1)) ||
        (status = CPXXsetintparam(env, CPXPARAM_MIP_Cuts_Gomory, -1)) ||
        (status = CPXXsetintparam(env, CPXPARAM_MIP_Cuts_FlowCovers, -1)) ||
        (status = CPXXsetintparam(env, CPXPARAM_MIP_Cuts_PathCut, -1)) ||
        (status = CPXXsetintparam(env, CPXPARAM_MIP_Cuts_LiftProj, -1)) ||
        (status = CPXXsetintparam(env, CPXPARAM_MIP_Cuts_ZeroHalfCut, -1)) ||
        (status = CPXXsetintparam(env, CPXPARAM_MIP_Cuts_Cliques, -1)) ||
        (status = CPXXsetintparam(env, CPXPARAM_MIP_Cuts_Covers, -1)) )
   {
      fprintf(stderr, "Failed to set parameter: %s\n",
              CPXXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   /* Now we set up for the callback. */

   /* Put the variable name in our callback data */
   cbdata.cname = (const char * const *) cname;

   if ( usecallback ) {
      /* Set up the user cuts. We change the contextmask parameter with a bitwise or, we populate
       * the cut table if necessary and otherwise set the disaggregate flag to 1.
       */
      contextmask |= CPX_CALLBACKCONTEXT_RELAXATION;
      if ( fromtable ) {
         /* Generate all disaggregated constraints and put them into a
          * table that is scanned by the callback.
          */
         for (j = 0, i = 0; j < LOCATIONS; ++j)
            for (c = 0; c < CLIENTS; ++c) {
               table[i].ind[0] = supply(c, j);
               table[i].val[0] = 1.0;
               table[i].ind[1] = used(j);
               table[i].val[1] = -1.0;
               table[i].rhs = 0.0;
               table[i].sense = 'L';
               ++i;
            }

         cbdata.cuts = table;
      }
      else {
         cbdata.disaggregate = 1;
      }
   }

   if ( lazy ) {
      /* Setup for the lazy constraint. We just change the contextmask parameter with a bitwise or.
       */
      contextmask |= CPX_CALLBACKCONTEXT_CANDIDATE;
   }

   if ( contextmask != 0 ) {
      /* We are done and now we register our callback function. */
      status = CPXXcallbacksetfunc (env, lp, contextmask, callback, &cbdata);
      if ( status != 0 ) {
         fprintf(stderr, "Failed to add callback: %s\n",
                 CPXXgeterrorstring(env, status, errbuf));
         goto TERMINATE;
      }
   }


   /* Solve the model. */
   status = CPXXmipopt(env, lp);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to optimize: %s\n",
              CPXXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   /* Query solution and some statistics that we want to display. */
   status = CPXXgetnumcuts(env, lp, CPX_CUT_USER, &ncuts);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to query cut counts: %s\n",
              CPXXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   status = CPXXgetobjval(env, lp, &objval);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to query objective: %s\n",
              CPXXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   status = CPXXgetx(env, lp, x, 0, NVARS - 1);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to query solution vector: %s\n",
              CPXXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   status = CPXXgetdblparam(env, CPXPARAM_MIP_Tolerances_Integrality, &tol);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to query tolerance: %s\n",
              CPXXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   /* Dump the solution. */
   printf("Solution status:                   %d\n", CPXXgetstat(env, lp));
   printf("Nodes processed:                   %lld\n", CPXXgetnodecnt(env, lp));
   printf("Active user cuts/lazy constraints: %d\n", ncuts);
   printf("Optimal value:                     %f\n", objval);
   for (j = 0; j < LOCATIONS; ++j) {
      if ( x[used(j)] >= 1 - tol ) {
         printf("Facility %d is used, it serves clients", j);
         for (c = 0; c < CLIENTS; ++c) {
            if ( x[supply(c, j)] >= 1 - tol )
               printf(" %d", c);
         }
         printf("\n");
      }
   }
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
}
