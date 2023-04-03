/* --------------------------------------------------------------------------
 * File: admipex5.c
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

/* admipex5.c -- Solve a facility location problem with cut callbacks or
 *               lazy constraints.
 *
 * Given a set of locations J and a set of clients C, the following model is
 * solved:
 *
 *  Minimize
 *   sum(j in J) fixedCost[j]*used[j] +
 *   sum(j in J)sum(c in C) cost[c][j]*supply[c][j]
 *  Subject to
 *   sum(j in J) supply[c][j] == 1                    for all c in C
 *   sum(c in C) supply[c][j] <= (|C| - 1) * used[j]  for all j in J
 *               supply[c][j] in {0, 1}               for all c in C, j in J
 *                    used[j] in {0, 1}               for all j in J
 *
 * In addition to the constraints stated above, the code also separates
 * a disaggregated version of the capacity constraints (see comments for the
 * cut callback) to improve performance.
 *
 * Optionally, the capacity constraints can be separated from a lazy
 * constraint callback instead of being stated as part of the initial model.
 *
 * See the usage message for how to switch between these options.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ilcplex/cplex.h>


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

/* epsilon used for violation of cuts */
#define EPS 1e-6

/* Separate the disaggregated capacity constraints.
 * In the model we have for each location j the constraint
 *    sum(c in clients) supply[c][j] <= (nbClients-1) * used[j]
 * Clearly, a client can only be serviced from a location that is used,
 * so we also have a constraint
 *    supply[c][j] <= used[j]
 * that must be satisfied by every feasible solution. These constraints tend
 * to be violated in LP relaxation. In this callback we separate them.
 */
static int CPXPUBLIC
disaggregated(CPXCENVptr env, void *cbdata, int wherefrom,
              void *cbhandle, int *useraction_p)
{
   char const **cname = cbhandle;
   int status;
   int j, c;
   double x[NVARS];

   status = CPXgetcallbacknodex(env, cbdata, wherefrom, x, 0, NVARS - 1);
   if ( status != 0 )
      return status;

   /* For each j and c check whether in the current solution (obtained by
    * calls to CPXgetcallbacknodex()) we have supply[c][j] > used[j]. If so,
    * then we have found a violated constraint and add it as a cut.
    */
   for (j = 0; j < LOCATIONS; ++j) {
      for (c = 0; c < CLIENTS; ++c) {
         double s = x[supply(c, j)];
         double o = x[used(j)];
         if ( s > o + EPS) {
            int cutind[2];
            double cutval[2];

            cutind[0] = supply(c, j);
            cutval[0] = 1.0;
            cutind[1] = used(j);
            cutval[1] = -1.0;
            printf("Adding %s <= %s [%f > %f]\n",
                   cname[supply(c, j)], cname[used(j)], s ,o);
            status = CPXcutcallbackadd(env, cbdata, wherefrom, 2, 0.0,
                                       'L', cutind, cutval, CPX_USECUT_PURGE);
            if ( status != 0 )
               return status;
            *useraction_p = CPX_CALLBACK_SET;
         }
      }
   }
   return 0;
}

/* A simple structure to represent a cut in the table used with cutsfromtable().
 * The cut is represented with a sense of <=.
 */
typedef struct {
   int ind[2];
   double val[2];
   double rhs;
} CUT;

/* Variant of the disaggregated callback that does not look for violated
 * cuts dynamically. Instead it uses a static table of cuts and scans this
 * table for violated cuts.
 */
static int CPXPUBLIC
cutsfromtable(CPXCENVptr env, void *cbdata, int wherefrom,
              void *cbhandle, int *useraction_p)
{
   CUT const *cuts = cbhandle;
   double x[NVARS];
   int status;
   int i;

   status = CPXgetcallbacknodex(env, cbdata, wherefrom, x, 0, NVARS - 1);
   if ( status != 0 )
      return status;

   for (i = 0; i < CLIENTS * LOCATIONS; ++i) {
      double lhs = 0.0;
      int k;

      for (k = 0; k < 2; ++k)
         lhs += cuts[i].val[k] * x[cuts[i].ind[k]];
      if ( lhs > cuts[i].rhs + EPS ) {
         printf("Adding cut number %d from table (violated by %g)\n",
                i, lhs - cuts[i].rhs);
         status = CPXcutcallbackadd(env, cbdata, wherefrom, 2,
                                    cuts[i].rhs,
                                    'L',
                                    cuts[i].ind,
                                    cuts[i].val, CPX_USECUT_PURGE);
         if ( status != 0 )
            return status;
         *useraction_p = CPX_CALLBACK_SET;
      }
   }
   return 0;
}

/* Lazy constraint callback to enforce the capacity constraints.
 * If used then the callback is invoked for every integer feasible solution
 * CPLEX finds. For each location j it checks whether constraint
 *    sum(c in C) supply[c][j] <= (|C| - 1) * used[j]
 * is satisfied. If not then it adds the violated constraint as lazy constraint.
 */
static int CPXPUBLIC
lazycallback(CPXCENVptr env, void *cbdata, int wherefrom,
             void *cbhandle, int *useraction_p)
{
   char const **cname = cbhandle;
   double x[NVARS];
   int status;
   int j, c, k;

   status = CPXgetcallbacknodex(env, cbdata, wherefrom, x, 0, NVARS - 1);
   if ( status != 0 )
      return status;

   for (j = 0; j < LOCATIONS; ++j) {
      double isused = x[used(j)];
      double served = 0.0;  /* Number of clients currently served from j */
      for (c = 0; c < CLIENTS; ++c)
         served += x[supply(c, j)];
      if ( served > (CLIENTS - 1.0) * isused + EPS ) {
         int cutind[CLIENTS + 1];
         double cutval[CLIENTS + 1];

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
         status = CPXcutcallbackadd(env, cbdata, wherefrom, CLIENTS + 1, 0.0,
                                    'L', cutind, cutval, CPX_USECUT_FORCE);
         if ( status != 0 )
            return status;
         *useraction_p = CPX_CALLBACK_SET;
      }
   }
   return 0;
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
   int rmatbeg[CLIENTS];
   int rmatind[CLIENTS * LOCATIONS];
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

   return CPXaddrows(env, lp, 0, CLIENTS, CLIENTS * LOCATIONS, rhs, sense,
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
   int rmatbeg[LOCATIONS];
   int rmatind[(CLIENTS + 1) * LOCATIONS];
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

   return CPXaddrows(env, lp, 0, LOCATIONS, (CLIENTS + 1) * LOCATIONS,
                     rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);
}

int
main(int argc, char **argv)
{
   int fromtable = 0;
   int lazy = 0;
   int usecallback = 1;
   CPXENVptr env = NULL;
   CPXLPptr lp = NULL;
   int status = 0;
   char errbuf[CPXMESSAGEBUFSIZE];
   double lb[NVARS], ub[NVARS], obj[NVARS];
   char ctype[NVARS];
   char cnamebuf[NVARS][32];
   char *cname[NVARS];
   CUT table[CLIENTS * LOCATIONS];
   int i;
   int j, c;
   int ncuts;
   double tol, objval;
   double x[NVARS];

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
   env = CPXopenCPLEX(&status);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to open CPLEX: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   status = CPXsetintparam(env, CPXPARAM_ScreenOutput, CPX_ON);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to enable screen output: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   lp = CPXcreateprob(env, &status, "admipex5");
   if ( status != 0 ) {
      fprintf(stderr, "Failed to create problem: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   /* Create variables.
    * - used[j]      If location j is used.
    * - supply[c][j] Amount shipped from location j to client c. This is a
    *                number in [0,1] and specifies the percentage of c's
    *                demand that is served from location i.
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

   status = CPXnewcols(env, lp, NVARS, obj, lb, ub, ctype, cname);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to create variables: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   /* The supply for each client must sum to 1, i.e., the demand of each
    * client must be met.
    */
   status = addsupplyconstraints(env, lp);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to add supply constraints: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   /* Capacity constraint for each location. We just require that a single
    * location cannot serve all clients, that is, the capacity of each
    * location is nbClients-1. This makes the model a little harder to
    * solve and allows us to separate more cuts.
    */
   if ( !lazy ) {
      status = addcapacityconstraints(env, lp);
      if ( status != 0 ) {
         fprintf(stderr, "Failed to add capacity constraints: %s\n",
                 CPXgeterrorstring(env, status, errbuf));
         goto TERMINATE;
      }
   }

   /* Tweak some CPLEX parameters so that CPLEX has a harder time to
    * solve the model and our cut separators can actually kick in.
    */
   if ( (status = CPXsetintparam(env, CPXPARAM_Threads, 1)) ||
        (status = CPXsetintparam(env, CPXPARAM_MIP_Strategy_HeuristicFreq, -1)) ||
        (status = CPXsetintparam(env, CPXPARAM_MIP_Cuts_MIRCut, -1)) ||
        (status = CPXsetintparam(env, CPXPARAM_MIP_Cuts_Implied, -1)) ||
        (status = CPXsetintparam(env, CPXPARAM_MIP_Cuts_Gomory, -1)) ||
        (status = CPXsetintparam(env, CPXPARAM_MIP_Cuts_FlowCovers, -1)) ||
        (status = CPXsetintparam(env, CPXPARAM_MIP_Cuts_PathCut, -1)) ||
        (status = CPXsetintparam(env, CPXPARAM_MIP_Cuts_LiftProj, -1)) ||
        (status = CPXsetintparam(env, CPXPARAM_MIP_Cuts_ZeroHalfCut, -1)) ||
        (status = CPXsetintparam(env, CPXPARAM_MIP_Cuts_Cliques, -1)) ||
        (status = CPXsetintparam(env, CPXPARAM_MIP_Cuts_Covers, -1)) )
   {
      fprintf(stderr, "Failed to set parameter: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   /* Setup callbacks. We disable CPXPARAM_MIP_Strategy_CallbackReducedLP so
    * that indices in the callbacks can refer to the original model and we
    * don't have to translate the indices to the presolved model.
    * This also requires us to disable certain reductions so that cuts and
    * lazy constraints can always be crushed.
    */
   status = CPXsetintparam(env, CPXPARAM_MIP_Strategy_CallbackReducedLP, CPX_OFF);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to disable reduced LP in callbacks: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }
   status = CPXsetintparam(env, CPXPARAM_Preprocessing_Reformulations,
                           CPX_PREREFORM_INTERFERE_UNCRUSH);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to disable dual reductions: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   if ( usecallback ) {
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
               ++i;
            }

         status = CPXsetusercutcallbackfunc(env, cutsfromtable, table);
         if ( status != 0 ) {
            fprintf(stderr, "Failed to add callback: %s\n",
                    CPXgeterrorstring(env, status, errbuf));
            goto TERMINATE;
         }
      }
      else {
         status = CPXsetusercutcallbackfunc(env, disaggregated, cname);
         if ( status != 0 ) {
            fprintf(stderr, "Failed to add callback: %s\n",
                    CPXgeterrorstring(env, status, errbuf));
            goto TERMINATE;
         }
      }
   }

   if ( lazy ) {
      status = CPXsetlazyconstraintcallbackfunc(env, lazycallback, cname);
      if ( status != 0 ) {
         fprintf(stderr, "Failed to add callback: %s\n",
                 CPXgeterrorstring(env, status, errbuf));
         goto TERMINATE;
      }
   }

   /* Solve the model. */
   status = CPXmipopt(env, lp);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to optimize: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   /* Query solution and some statistics that we want to display. */
   status = CPXgetnumcuts(env, lp, CPX_CUT_USER, &ncuts);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to query cut counts: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   status = CPXgetobjval(env, lp, &objval);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to query objective: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   status = CPXgetx(env, lp, x, 0, NVARS - 1);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to query solution vector: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   status = CPXgetdblparam(env, CPXPARAM_MIP_Tolerances_Integrality, &tol);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to query tolerance: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   /* Dump the solution. */
   printf("Solution status:                   %d\n", CPXgetstat(env, lp));
   printf("Nodes processed:                   %d\n", CPXgetnodecnt(env, lp));
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

   /* Free the problem as allocated by CPXcreateprob and
      CPXreadcopyprob, if necessary */

   if ( lp != NULL ) {
      int xstatus = CPXfreeprob (env, &lp);

      if ( !status ) status = xstatus;
   }

   /* Free the CPLEX environment, if necessary */

   if ( env != NULL ) {
      int xstatus = CPXcloseCPLEX (&env);

      if ( !status ) status = xstatus;
   }
     
   return (status);
}
