/* --------------------------------------------------------------------------
 * File: bendersatsp2.c
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

/* Example bendersatsp2.c solves a flow MILP model for an
   Asymmetric Traveling Salesman Problem (ATSP) instance
   through Benders decomposition, via the generic callback API.

   The arc costs of an ATSP instance are read from an input file.
   The flow MILP model is decomposed into a master ILP and a worker LP.

   The master ILP is then solved by adding Benders' cuts via the new generic
   callback function benders_callback during the branch-and-cut process.

   The callback benders_callback adds to the master ILP violated Benders'
   cuts that are found by solving the worker LP.

   The example allows the user to decide if Benders' cuts have to be separated
   just as lazy constraints or also as user cuts. In particular:

   a) Only to separate integer infeasible solutions.
   In this case, benders_callback is called with
   contextid=CPX_CALLBACKCONTEXT_CANDIDATE. The current candidate integer
   solution can be queried with CPXcallbackgetcandidatepoint, and it can be
   rejected by the user, optionally providing a list of lazy constraints, with
   the function CPXcallbackrejectcandidate.

   b) Also to separate fractional infeasible solutions.
   In this case, benders_callback is called with
   contextid=CPX_CALLBACKCONTEXT_RELAXATION. The current fractional solution
   can be queried with CPXcallbackgetrelaxationpoint. Cutting planes can then
   be added via CPXcallbackaddusercuts.

   The example shows how to properly support deterministic parallel search
   with a user callback (there a significant departure here frome the legacy
   control callbacks):

   a) To avoid race conditions (as the callback is called simultaneously by
   multiple threads), each thread has its own working copy of the data
   structures needed to separate cutting planes. Access to global data
   is read-only.

   b) Thread-local data for all threads is created on THREAD_UP
   and destroyed on THREAD_DOWN. This guarantees determinism.
 */


/* To run this example, command line arguments are required:
       bendersatsp2 {0|1} [filename]
   where
       0         Indicates that Benders' cuts are only used as lazy constraints,
                 to separate integer infeasible solutions.
       1         Indicates that Benders' cuts are also used as user cuts,
                 to separate fractional infeasible solutions.

       filename  Is the name of the file containing the ATSP instance (arc
                 costs).
                 If filename is not specified, the instance
                 ../../../examples/data/atsp.dat is read */


/* ATSP instance defined on a directed graph G = (V, A)
   - V = {0, ..., n-1}, V0 = V \ {0}
   - A = {(i,j) : i in V, j in V, i != j }
   - forall i in V: delta+(i) = {(i,j) in A : j in V}
   - forall i in V: delta-(i) = {(j,i) in A : j in V}
   - c(i,j) = traveling cost associated with (i,j) in A

   Flow MILP model

   Modeling variables:
   forall (i,j) in A:
      x(i,j) = 1, if arc (i,j) is selected
             = 0, otherwise
   forall k in V0, forall (i,j) in A:
      y(k,i,j) = flow of the commodity k through arc (i,j)

   Objective:
   minimize sum((i,j) in A) c(i,j) * x(i,j)

   Degree constraints:
   forall i in V: sum((i,j) in delta+(i)) x(i,j) = 1
   forall i in V: sum((j,i) in delta-(i)) x(j,i) = 1

   Binary constraints on arc variables:
   forall (i,j) in A: x(i,j) in {0, 1}

   Flow constraints:
   forall k in V0, forall i in V:
      sum((i,j) in delta+(i)) y(k,i,j) - sum((j,i) in delta-(i)) y(k,j,i) = q(k,i)
      where q(k,i) =  1, if i = 0
                   = -1, if k == i
                   =  0, otherwise

   Capacity constraints:
   forall k in V0, for all (i,j) in A: y(k,i,j) <= x(i,j)

   Nonnegativity of flow variables:
   forall k in V0, for all (i,j) in A: y(k,i,j) >= 0

   See functions init_user_localdata and create_master_ILP for how this model
   is decomposed.
 */


#include <ilcplex/cplex.h>

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Declaration of the thread-local data structure
 * for the function benders_callback
 */

typedef struct {
   /* Environment for the worker LP used to generate Benders' cuts */
   CPXENVptr env;
   /* Worker LP used to generate Benders' cuts */
   CPXLPptr  lp;
   /* Number of columns in the worker LP */
   int num_v_cols, num_u_cols;
   /* Work arrays to:
      -- read the solution from the master ILP
      -- update the objective function of the worker LP
      -- read an unbounded direction (ray) from the worker LP */
   double *x;
   int *indices;
   double *ray;
   /* Work arrays to add a Benders' cut to the master ILP */
   double *cutval;
   int *cutind;
} USERLOCALDATA;


/* Declaration of the global data structure
 * for the function benders_callback
 */
typedef struct {
   /* number of nodes in the ATSP instance */
   int num_nodes;
   /* Number of columns in the master ILP */
   int num_x_cols;
   /* Size of array DATA. Must be >= than the number of threads used
    * used when solving the model.
    */
   int num_threads;
   USERLOCALDATA *data;
} USERDATA;


/* Declarations for functions in this program */

static int CPXPUBLIC
   benders_callback  (CPXCALLBACKCONTEXTptr context,
                      CPXLONG contextid, void *cbhandle);

static int
   set_benders_callback  (CPXENVptr env, CPXLPptr lp,
                          int separate_fractional_solutions,
                          USERDATA *userdata),
   create_master_ILP     (CPXENVptr env, CPXLPptr lp, double **arc_cost,
                          int num_nodes),
   init_user_data        (USERDATA *globaldata,
                          int num_nodes, int num_x_cols, int num_cores),
   init_user_localdata   (USERLOCALDATA *localdata,
                          int num_nodes, int num_x_cols);

static void
   free_user_data        (USERDATA *globaldata),
   free_user_localdata   (USERLOCALDATA *localdata);

static int
   read_array (FILE *in, int *num_p, double **data_p),
   read_ATSP  (const char* file, double ***arc_cost_p, int *num_nodes_p);

static void
   free_and_null  (char **ptr),
   usage          (char *progname);



int
main (int  argc, char *argv[])
{
   int status = 0;
   int solstat;

   /* 9 city problem */

   const char* filename = "../../../examples/data/atsp.dat";

   /* ATSP instance */

   double **arc_cost = NULL;
   int num_nodes;

   /* data required to print the optimal ATSP tour */

   double objval;
   int num_x_cols;
   double *x = NULL;
   int i, j;
   int *succ = NULL;

   /* Cplex environment and master ILP */

   CPXENVptr env = NULL;
   CPXLPptr  lp = NULL;

   /* Decide when Benders' cuts are going to be separated:
      0: only when an integer solution if found
         (i.e., contextid == CPX_CALLBACKCONTEXT_CANDIDATE )
      1: even to cut-off fractional solutions
         (i.e., contextid == CPX_CALLBACKCONTEXT_RELAXATION )
    */

   int separate_fractional_solutions;

   int num_cores = 1;

   /* Global callback data structure */

   USERDATA cbhandle;
   cbhandle.num_nodes   = 0;
   cbhandle.num_x_cols  = 0;
   cbhandle.num_threads = 0;
   cbhandle.data        = NULL;

   /* Check the command line arguments */

   if ( argc != 2 && argc != 3) {
      usage (argv[0]);
      goto TERMINATE;
   }

   if ( (argv[1][0] != '1' && argv[1][0] != '0') ||
        argv[1][1] != '\0' ) {
      usage (argv[0]);
      goto TERMINATE;
   }

   separate_fractional_solutions = ( argv[1][0] == '0' ? 0 : 1 );

   printf ("Benders' cuts separated to cut off: ");
   if ( separate_fractional_solutions ) {
      printf ("Integer and fractional infeasible solutions.\n");
   }
   else {
      printf ("Only integer infeasible solutions.\n");
   }
   fflush (stdout);

   if ( argc == 3 )  filename = argv[2];

   /* Read the ATSP instance */

   status = read_ATSP (filename, &arc_cost, &num_nodes);
   if ( status ) {
      fprintf (stderr, "Error in read_ATSP, status = %d\n", status);
      goto TERMINATE;
   }

   /* Init the CPLEX environment */

   env = CPXopenCPLEX (&status);
   if ( env == NULL ) {
      fprintf (stderr, "Failure in CPXopenCPLEX, status = %d.\n", status);
      goto TERMINATE;
   }

   /* Turn on output to the screen */

   status = CPXsetintparam (env, CPXPARAM_ScreenOutput, CPX_ON);
   if ( status ) {
      fprintf (stderr, "Failed to turn on screen indicator, status = %d.\n",
               status);
      goto TERMINATE;
   }

   /* Set MIP log interval to 1 */

   status = CPXsetintparam (env, CPXPARAM_MIP_Interval, 1);
   if ( status )  {
      fprintf (stderr,
             "Failed to set CPXPARAM_MIP_Interval, status = %d.\n", status);
      goto TERMINATE;
   }

   /* Create the master ILP */

   lp = CPXcreateprob (env, &status, "master_ILP.lp");
   if ( lp == NULL ) {
      fprintf (stderr, "Failure in CPXcreateprob, status = %d.\n", status);
      goto TERMINATE;
   }

   status = create_master_ILP (env, lp, arc_cost, num_nodes);
   if ( status ) {
      fprintf (stderr,
               "Failed to create the master ILP.\n");
      goto TERMINATE;
   }

   /* Set up global user data */

   status = CPXgetnumcores (env, &num_cores);
   if ( status ) {
      fprintf (stderr,
               "Failed to get the number of cores.\n");
      goto TERMINATE;
   }

   status = init_user_data (&cbhandle, num_nodes,
                            num_nodes * num_nodes, num_cores);
   if ( status ) {
      fprintf (stderr,
               "Failed to init the callback data structure, status = %d.\n",
               status);
      goto TERMINATE;
   }

   /* Set up environment parameters to use the function benders_callback
      as callback function */

   status = set_benders_callback (env, lp,
                                  separate_fractional_solutions,
                                  &cbhandle);
   if ( status ) {
      fprintf (stderr,
               "Failure in function set_benders_callback: status = %d.\n",
               status);
      goto TERMINATE;
   }

   /* Optimize the problem and obtain solution status */

   status = CPXmipopt (env, lp);
   if ( status ) {
      fprintf (stderr, "Failed to optimize MIP, status = %d.\n", status);
      goto TERMINATE;
   }

   solstat = CPXgetstat (env, lp);
   printf ("\nSolution status: %d\n", solstat);

   /* Write out the objective value */

   if ( CPXgetobjval (env, lp, &objval) ) {
      printf ("Failed to obtain objective value.\n");
   }
   else {
      printf ("Objective value: %17.10e\n", objval);
   }

   if ( solstat == CPXMIP_OPTIMAL ) {

      /* Write out the optimal tour */

      num_x_cols = CPXgetnumcols (env, lp);
      x = malloc (num_x_cols * sizeof(*x));
      if ( x == NULL ) {
         fprintf (stderr, "No memory for x array.\n");
         status = -1;
         goto TERMINATE;
      }
      status = CPXgetx (env, lp, x, 0, num_x_cols-1);
      if ( status ) {
         fprintf (stderr, "Failed to obtain solution, status = %d.\n", status);
         goto TERMINATE;
      }

      succ = malloc (num_nodes * sizeof(*succ));
      if ( succ == NULL ) {
         fprintf (stderr, "No memory for succ array.\n");
         status = -1;
         goto TERMINATE;
      }
      for (j = 0; j < num_nodes; ++j)
         succ[j] = -1;
      for (i = 0; i < num_nodes; ++i) {
         for (j = 0; j < num_nodes; ++j) {
            if ( fabs (x[i * num_nodes + j]) > 1e-03 )
               succ[i] = j;
         }
      }
      printf ("Optimal tour:\n");
      i = 0;
      while ( succ[i] != 0 ) {
         printf ("%d, ", i);
         i = succ[i];
      }
      printf ("%d\n", i);

   }
   else {
      printf ("Solution status is not CPX_STAT_OPTIMAL\n");
   }

TERMINATE:
   /* Free the allocated memory if necessary */

   free_and_null ((char **) &x);
   free_and_null ((char **) &succ);

   if ( arc_cost != NULL ) {
      for (i = 0; i < num_nodes; ++i) {
         free_and_null ((char **) &(arc_cost[i]));
      }
   }
   free_and_null ((char **) &arc_cost);

   free_user_data (&cbhandle);

   if ( lp != NULL ) {
      int local_status = CPXfreeprob (env, &lp);
      if ( local_status ) {
         fprintf (stderr, "CPXfreeprob failed, error code %d.\n",
                  local_status);
         status = local_status;
      }
   }

   /* Free the CPLEX environment, if necessary */

   if ( env != NULL ) {
      int local_status = CPXcloseCPLEX (&env);
      if ( local_status ) {
         fprintf (stderr,
                  "Could not close CPLEX environment, status = %d.\n",
                  local_status);
         status = local_status;
      }
   }

   return status;

} /* END main */


static int
init_user_data (USERDATA *globaldata,
                int num_nodes, int num_x_cols, int num_cores)
{
   int status = 0;
   int t;

   globaldata->data = malloc (num_cores * sizeof(*(globaldata->data)));
   if ( globaldata->data == NULL ) {
      fprintf (stderr, "No memory for x array.\n");
      goto TERMINATE;
   }

   globaldata->num_nodes = num_nodes;
   globaldata->num_x_cols = num_x_cols;
   globaldata->num_threads = num_cores;

   for (t = 0; t < num_cores; t++) {
      USERLOCALDATA *item = &(globaldata->data[t]);
      item->num_v_cols = 0;
      item->num_u_cols = 0;
      item->env     = NULL;
      item->lp      = NULL;
      item->x       = NULL;
      item->indices = NULL;
      item->ray     = NULL;
      item->cutind  = NULL;
      item->cutval  = NULL;
   }

TERMINATE:

   return status;
} /* END init_user_data */


static void
free_user_data (USERDATA *globaldata)
{
   if (globaldata == NULL)  return;

   if (globaldata->data) {
      int t;
      for (t = 0; t < globaldata->num_threads; t++) {
         free_user_localdata (&(globaldata->data[t]));
      }
   }
   free_and_null ((char **) &globaldata->data);
} /* END free_user_localdata */



/* This routine initializes the data structure for
   the user callback function benders_callback.
   In particular, it creates the worker LP that will be used by
   the function benders_callback to separate Benders' cuts.

   Worker LP model (dual of flow constraints and
   capacity constraints of the flow MILP)

   Modeling variables:
   forall k in V0, i in V:
      u(k,i) = dual variable associated with flow constraint (k,i)

   forall k in V0, forall (i,j) in A:
      v(k,i,j) = dual variable associated with capacity constraint (k,i,j)

   Objective:
   minimize sum(k in V0) sum((i,j) in A) x(i,j) * v(k,i,j)
            - sum(k in V0) u(k,0) + sum(k in V0) u(k,k)

   Constraints:
   forall k in V0, forall (i,j) in A: u(k,i) - u(k,j) <= v(k,i,j)

   Nonnegativity on variables v(k,i,j)
   forall k in V0, forall (i,j) in A: v(k,i,j) >= 0 */

static int
init_user_localdata  (USERLOCALDATA *localdata,
                      int num_nodes, int num_x_cols)
{
   int i, j, k;
   int status = 0;

   /* Data structures to create columns and add rows */

   int num_rows;
   int nzcnt;
   char *sense = NULL;
   double *rhs = NULL;
   int *rmatbeg = NULL;
   int *rmatind = NULL;
   double *rmatval = NULL;
   char colnamebuf[100];
   char *colname = colnamebuf;

   /* Init user_cbhandle */
   free_user_localdata(localdata);

   localdata->num_v_cols = (num_nodes - 1) * num_x_cols;
   localdata->num_u_cols = (num_nodes - 1) * num_nodes;

   localdata->x = malloc (num_x_cols * sizeof(*localdata->x));
   if ( localdata->x == NULL ) {
      fprintf (stderr, "No memory for x array.\n");
      goto TERMINATE;
   }
   localdata->indices = malloc (num_x_cols * sizeof(*localdata->indices));
   if ( localdata->indices == NULL ) {
      fprintf (stderr, "No memory for indices array.\n");
      goto TERMINATE;
   }
   localdata->ray = malloc ((localdata->num_v_cols +
                                 localdata->num_u_cols) *
                                sizeof(*localdata->ray));
   if ( localdata->ray == NULL ) {
      fprintf (stderr, "No memory for ray array.\n");
      goto TERMINATE;
   }
   localdata->cutval = malloc (num_x_cols * sizeof(*localdata->cutval));
   if ( localdata->cutval == NULL ) {
      fprintf (stderr, "No memory for cutval array.\n");
      goto TERMINATE;
   }
   localdata->cutind = malloc (num_x_cols * sizeof(*localdata->cutind));
   if ( localdata->cutind == NULL ) {
      fprintf (stderr, "No memory for cutind array.\n");
      goto TERMINATE;
   }

   /* Create the environment for the worker LP */

   localdata->env = CPXopenCPLEX (&status);
   if ( localdata->env == NULL ) {
      fprintf (stderr,
         "Could not open CPLEX environment for the worker LP: status = %d.\n",
         status);
      goto TERMINATE;
   }

   /* Turn off the presolve reductions */

   status = CPXsetintparam (localdata->env, CPXPARAM_Preprocessing_Reduce,
                             0);
   if ( status ) {
      fprintf(stderr,
         "Failed to set CPXPARAM_Preprocessing_Reduce, status = %d.\n", status);
      goto TERMINATE;
   }

   /* Create the worker LP */

   localdata->lp = CPXcreateprob (localdata->env, &status,
                                       "atsp_worker.lp");
   if ( localdata->lp == NULL ) {
      fprintf (stderr, "Failed to create the worker LP: status = %d\n", status);
      goto TERMINATE;
   }

   /* Create variables v(k,i,j), one at a time
      For simplicity, also dummy variables v(k,i,i) are created.
      Those variables are fixed to 0 and do not participate in
      the constraints */

   for (k = 1; k < num_nodes; ++k) {
      for (i = 0; i < num_nodes; ++i) {
         for (j = 0; j < num_nodes; ++j) {
            double ub = ( i == j ? 0. : CPX_INFBOUND );
            sprintf (colnamebuf, "v.%d.%d.%d", k, i, j);
            status = CPXnewcols (localdata->env, localdata->lp, 1,
                                  NULL, NULL, &ub, NULL, &colname);
            if ( status ) {
               fprintf (stderr, "Error in CPXnewcols, status = %d.\n", status);
               goto TERMINATE;
            }
         }
      }
   }

   /* Create variables u(k,i), one at a time */

   for (k = 1; k < num_nodes; ++k) {
      for (i = 0; i < num_nodes; ++i) {
         double obj = 0.;
         double lb = -CPX_INFBOUND;
         double ub = CPX_INFBOUND;
         sprintf (colnamebuf, "u.%d.%d", k, i);
         if ( i == 0 )
            obj = -1.;
         else if ( i == k )
            obj = 1.;
         status = CPXnewcols (localdata->env, localdata->lp, 1,
                              &obj, &lb, &ub, NULL, &colname);
         if ( status ) {
            fprintf (stderr, "Error in CPXnewcols, status = %d.\n", status);
            goto TERMINATE;
         }
      }
   }

   /* Init data structures for CPXaddrows */

   num_rows = num_x_cols * (num_nodes - 1);

   rhs = malloc (num_rows * sizeof (*rhs));
   if ( rhs == NULL ) {
      fprintf (stderr, "No memory for rhs array.\n");
      status = -1;
      goto TERMINATE;
   }
   sense = malloc (num_rows * sizeof (*sense));
   if ( sense == NULL ) {
      fprintf (stderr, "No memory for sense array.\n");
      status = -1;
      goto TERMINATE;
   }
   rmatbeg = malloc ( (num_rows + 1) * sizeof (*rmatbeg));
   if ( rmatbeg == NULL ) {
      fprintf (stderr, "No memory for rmatbeg array.\n");
      status = -1;
      goto TERMINATE;
   }
   rmatind = malloc (3 * num_rows * sizeof (*rmatind));
   if ( rmatind == NULL ) {
      fprintf (stderr, "No memory for rmatind array.\n");
      status = -1;
      goto TERMINATE;
   }
   rmatval = malloc (3 * num_rows * sizeof (*rmatval));
   if ( rmatval == NULL ) {
      fprintf (stderr, "No memory for rmatval array.\n");
      status = -1;
      goto TERMINATE;
   }

   /* Populate data structures for CPXaddrows and add all the constraints:
      forall k in V0, forall (i,j) in A: u(k,i) - u(k,j) <= v(k,i,j) */

   num_rows = 0;
   nzcnt = 0;
   for (k = 1; k < num_nodes; ++k) {
      for (i = 0; i < num_nodes; ++i) {
         for (j = 0; j < num_nodes; ++j) {
            if ( i != j ) {
               rhs[num_rows]     =  0.;
               sense[num_rows]   =  'L';
               rmatbeg[num_rows] =  nzcnt;
               rmatind[nzcnt]    =  (k-1) * num_x_cols + i * num_nodes + j;
               rmatval[nzcnt++]  = -1.;
               rmatind[nzcnt]    =  localdata->num_v_cols +
                                    (k-1) * num_nodes + i;
               rmatval[nzcnt++]  =  1.;
               rmatind[nzcnt]    =  localdata->num_v_cols +
                                    (k-1) * num_nodes + j;
               rmatval[nzcnt++]  = -1.;
               ++num_rows;
            }
         }
      }
   }
   rmatbeg[num_rows] = nzcnt;

   status = CPXaddrows (localdata->env, localdata->lp,
                        0, num_rows, nzcnt, rhs, sense,
                        rmatbeg, rmatind, rmatval, NULL, NULL);
   if ( status ) {
      fprintf (stderr, "Error in CPXaddrows: status = %d\n", status);
      goto TERMINATE;
   }

TERMINATE:

   free_and_null ((char **) &sense);
   free_and_null ((char **) &rhs);
   free_and_null ((char **) &rmatbeg);
   free_and_null ((char **) &rmatind);
   free_and_null ((char **) &rmatval);

   return status;

} /* END init_user_localdata */


/*
This routine frees up the data structure for the user cutcallback
created by init_user_localdata
*/

static void
free_user_localdata  (USERLOCALDATA *localdata)
{
   if (localdata == NULL)  return;

   free_and_null ((char **) &localdata->x);
   free_and_null ((char **) &localdata->indices);
   free_and_null ((char **) &localdata->ray);
   free_and_null ((char **) &localdata->cutval);
   free_and_null ((char **) &localdata->cutind);

   if ( localdata->lp != NULL ) {
      int status = CPXfreeprob (localdata->env, &(localdata->lp) );
      if ( status ) {
         fprintf (stderr, "CPXfreeprob failed, error code %d.\n", status);
      }
      else
         localdata->lp = NULL;
   }

   if ( localdata->env != NULL ) {
      int status = CPXcloseCPLEX ( &(localdata->env) );
      if ( status ) {
         fprintf (stderr, "CPXcloseCPLEX failed, error code %d.\n", status);
      }
      else
         localdata->env = NULL;
   }
} /* END free_user_localdata */


/* This routine sets up the environment parameters to use the function
   benders_callback to separate and add Bender's cut during the branch-and-cut.
 */

static int
set_benders_callback  (CPXENVptr env, CPXLPptr lp,
                       int separate_fractional_solutions,
                       USERDATA *cbhandle)
{
   int status = 0;

   CPXLONG where = CPX_CALLBACKCONTEXT_THREAD_UP
                   | CPX_CALLBACKCONTEXT_THREAD_DOWN
                   | CPX_CALLBACKCONTEXT_CANDIDATE;
   if ( separate_fractional_solutions ) {
      where |= CPX_CALLBACKCONTEXT_RELAXATION;
   }

   /* Set up to use the function benders_callback */

   status = CPXcallbacksetfunc (env, lp, where, benders_callback, cbhandle);
   if ( status )  {
      fprintf (stderr,
         "Error in CPXcallbacksetfunc, status = %d.\n",
         status);
      goto TERMINATE;
   }

TERMINATE:

   return status;

} /* END set_benders_callback */


/* This function separate Benders' cuts violated by the current solution. */

static int CPXPUBLIC
benders_callback  (CPXCALLBACKCONTEXTptr context,
                   CPXLONG contextid, void *cbhandle)
{
   int status = 0;
   int me;
   USERDATA *globaldata = (USERDATA *) cbhandle;
   USERLOCALDATA *localdata = NULL;


   /* Data structures to add the Benders' cut */

   int k, nzcnt, cur_x_col, cur_v_col, cur_u_col, num_x_cols, num_nodes;
   int beg = 0;
   int worker_lp_sol_stat, purgeable, local, point;
   double rhs;
   double eps_ray = 1e-03;
   char sense;

   num_x_cols = globaldata->num_x_cols;
   num_nodes = globaldata->num_nodes;

   status = CPXcallbackgetinfoint (context, CPXCALLBACKINFO_THREADID, &me);
   if ( status ) {
      fprintf (stderr, "Error in CPXcallbackgetinfoint: status = %d\n", status);
      goto TERMINATE;
   }

   /* Get thread-local copy */
   localdata = &(globaldata->data[me]);

   if ( contextid == CPX_CALLBACKCONTEXT_THREAD_UP ) {
      /* Setup */
      status = init_user_localdata (localdata, globaldata->num_nodes,
                                    globaldata->num_x_cols);
      if ( status ) {
         fprintf (stderr,
                  "Error initializing local data for thread %i: status = %d\n",
                  me, status);
      }
      goto TERMINATE;
   }

   if ( contextid == CPX_CALLBACKCONTEXT_THREAD_DOWN ) {
      /* Teardown */
      free_user_localdata (localdata);
      goto TERMINATE;
   }

   /* Get the current x solution */

   switch (contextid) {
   case CPX_CALLBACKCONTEXT_CANDIDATE:
      status = CPXcallbackcandidateispoint (context, &point);
      if ( status ) {
         fprintf (stderr,
                  "Error in CPXXcallbackcandidateispoint: status = %d\n",
                  status);
         goto TERMINATE;
      }
      if ( !point ) {
         /* The model is bounded, so this cannot happen. */
         fprintf (stderr, "Unbounded solution");
         status = CPXERR_UNSUPPORTED_OPERATION;
         goto TERMINATE;
      }
      status = CPXcallbackgetcandidatepoint (context, localdata->x,
                                             0, num_x_cols - 1, NULL);
      if ( status ) {
         fprintf (stderr,
                  "Error in CPXcallbackgetcandidate: status = %d\n", status);
         goto TERMINATE;
      }
      break;
   case CPX_CALLBACKCONTEXT_RELAXATION:
      status = CPXcallbackgetrelaxationpoint (context, localdata->x,
                                              0, num_x_cols - 1, NULL);
      if ( status ) {
         fprintf (stderr,
                  "Error in CPXcallbackgetrelaxationpoint: status = %d\n",
                  status);
         goto TERMINATE;
      }
      break;
   default:
      fprintf (stderr, "Unexpected value of contextid: %lld\n", contextid);
      status = -1;
      goto TERMINATE;
   }

   /* Update the objective function in the worker LP:
      minimize sum(k in V0) sum((i,j) in A) x(i,j) * v(k,i,j)
               - sum(k in V0) u(k,0) + sum(k in V0) u(k,k)    */

   for (k = 1; k < num_nodes; ++k) {
      for (cur_x_col = 0; cur_x_col < num_x_cols; ++cur_x_col) {
         localdata->indices[cur_x_col] = (k-1) * num_x_cols + cur_x_col;
      }
      status = CPXchgobj (localdata->env, localdata->lp,
                          num_x_cols,
                          localdata->indices,
                          localdata->x);
      if ( status ) {
         fprintf (stderr, "Error in CPXchgobj: status = %d\n", status);
         goto TERMINATE;
      }
   }

   /* Solve the worker LP and look for a violated cut
      A violated cut is available iff
      worker_lp_sol_stat == CPX_STAT_UNBOUNDED */

   status = CPXprimopt (localdata->env, localdata->lp);
   if ( status ) {
      fprintf (stderr, "Error in CPXprimopt: status = %d\n", status);
      goto TERMINATE;
   }
   worker_lp_sol_stat = CPXgetstat (localdata->env, localdata->lp);
   if ( worker_lp_sol_stat != CPX_STAT_UNBOUNDED)
      goto TERMINATE;

   /* Get the violated cut as an unbounded ray of the worker LP */

   status = CPXgetray (localdata->env, localdata->lp, localdata->ray);
   if ( status ) {
      fprintf (stderr, "Error in CPXgetray: status = %d\n", status);
      goto TERMINATE;
   }

   /* Compute the cut from the unbounded ray. The cut is:
      sum((i,j) in A) (sum(k in V0) v(k,i,j)) * x(i,j) >=
      sum(k in V0) u(k,0) - u(k,k)  */

   nzcnt = 0;
   for (cur_x_col = 0; cur_x_col < num_x_cols; ++cur_x_col) {
      localdata->cutind[nzcnt] = cur_x_col;
      localdata->cutval[nzcnt] = 0.;
      for (k = 1; k < num_nodes; ++k) {
         cur_v_col = (k-1) * num_x_cols + cur_x_col;
         if ( localdata->ray[cur_v_col] > eps_ray ) {
            localdata->cutval[nzcnt] += localdata->ray[cur_v_col];
         }
      }
      if ( localdata->cutval[nzcnt] > eps_ray ) {
         ++nzcnt;
      }
   }

   sense = 'G';
   rhs = 0.0;
   for (k = 1; k < num_nodes; ++k) {
      cur_u_col = localdata->num_v_cols + (k-1) * num_nodes;
      if ( fabs (localdata->ray[cur_u_col]) > eps_ray ) {
         rhs += localdata->ray[cur_u_col];
      }
      cur_u_col = localdata->num_v_cols + (k-1) * num_nodes + k;
      if ( fabs (localdata->ray[cur_u_col]) > eps_ray ) {
         rhs -= localdata->ray[cur_u_col];
      }
   }

   /* Add the cut to the master ILP */

   switch (contextid) {
   case CPX_CALLBACKCONTEXT_CANDIDATE:
      status = CPXcallbackrejectcandidate (context, 1, nzcnt,
                                           &rhs, &sense, &beg,
                                           localdata->cutind,
                                           localdata->cutval);
      if ( status ) {
         fprintf (stderr,
                  "Error in CPXcallbackrejectcandidate: status = %d\n", status);
         goto TERMINATE;
      }
      break;
   case CPX_CALLBACKCONTEXT_RELAXATION:
      purgeable = CPX_USECUT_FORCE;
      local = 0;
      status = CPXcallbackaddusercuts (context, 1, nzcnt, &rhs, &sense, &beg,
                                       localdata->cutind,
                                       localdata->cutval,
                                       &purgeable, &local);
      if ( status ) {
         fprintf (stderr,
                  "Error in CPXcallbackaddusercuts: status = %d\n", status);
         goto TERMINATE;
      }
      break;
   default:
      fprintf (stderr, "Unexpected value of contextid: %lld\n", contextid);
      status = -1;
      goto TERMINATE;
   }

TERMINATE:

   return status;
} /* END benders_callback */


/* This routine creates the master ILP (arc variables x and degree constraints).

   Modeling variables:
   forall (i,j) in A:
      x(i,j) = 1, if arc (i,j) is selected
             = 0, otherwise

   Objective:
   minimize sum((i,j) in A) c(i,j) * x(i,j)

   Degree constraints:
   forall i in V: sum((i,j) in delta+(i)) x(i,j) = 1
   forall i in V: sum((j,i) in delta-(i)) x(j,i) = 1

   Binary constraints on arc variables:
   forall (i,j) in A: x(i,j) in {0, 1} */

static int
create_master_ILP (CPXENVptr env, CPXLPptr lp, double **arc_cost,
                   int num_nodes)
{
   int i, j;
   int status = 0;

   char sense;
   char colnamebuf[100];
   char *colname = colnamebuf;
   int nzcnt, rmatbeg;
   int *rmatind = NULL;
   double rhs, *rmatval = NULL;


   /* Change problem type */

   status = CPXchgprobtype (env, lp, CPXPROB_MILP);
   if ( status ) {
      fprintf (stderr, "Error in CPXchgprobtype, status = %d.\n", status);
      goto TERMINATE;
   }

   /* Create arc variables x(i,j), one at a time
      For simplicity, also dummy variables x(i,i) are created.
      Those variables are fixed to 0 and do not partecipate to
      the constraints */

   for (i = 0; i < num_nodes; ++i) {
      for (j = 0; j < num_nodes; ++j) {
         double cost = ( i == j ? 0. : arc_cost[i][j] );
         double lb   = 0.;
         double ub   = ( i == j ? 0. : 1. );
         char type   = 'B';
         sprintf (colnamebuf, "x.%d.%d", i, j);
         status = CPXnewcols (env, lp, 1, &cost, &lb, &ub, &type, &colname);
         if ( status ) {
            fprintf (stderr, "Error in CPXnewcols, status = %d.\n", status);
            goto TERMINATE;
         }
      }
   }

   /* Init data structures to add degree constraints */

   rhs = 1.;
   sense = 'E';
   rmatbeg = 0;
   rmatind = malloc ((num_nodes-1) * sizeof (*rmatind));
   if ( rmatind == NULL ) {
      fprintf (stderr, "No memory for rmatind array.\n");
      status = -1;
      goto TERMINATE;
   }
   rmatval = malloc ((num_nodes-1) * sizeof (*rmatval));
   if ( rmatval == NULL ) {
      fprintf (stderr, "No memory for rmatval array.\n");
      status = -1;
      goto TERMINATE;
   }

   /* Add the out degree constraints, one at a time:
      forall i in V: sum((i,j) in delta+(i)) x(i,j) = 1 */

   for (i = 0; i < num_nodes; ++i) {
      nzcnt = 0;
      for (j = 0; j < num_nodes; ++j) {
         if ( i != j ) {
            rmatind[nzcnt]   = i * num_nodes + j;
            rmatval[nzcnt++] = 1.;
         }
      }
      status = CPXaddrows (env, lp, 0, 1, num_nodes-1, &rhs, &sense,
                           &rmatbeg, rmatind, rmatval, NULL, NULL);
      if ( status ) {
         fprintf (stderr, "Error in CPXaddrows, status = %d.\n", status);
         goto TERMINATE;
      }
   }

   /* Add the in degree constraints, one at a time:
      forall i in V: sum((j,i) in delta-(i)) x(j,i) = 1 */

   for (i = 0; i < num_nodes; ++i) {
      nzcnt = 0;
      for (j = 0; j < num_nodes; ++j) {
         if (i != j ) {
            rmatind[nzcnt]   = j * num_nodes + i;
            rmatval[nzcnt++] = 1.;
         }
      }
      status = CPXaddrows (env, lp, 0, 1, num_nodes-1, &rhs, &sense,
                           &rmatbeg, rmatind, rmatval, NULL, NULL);
      if ( status ) {
         fprintf (stderr, "Error in CPXaddrows, status = %d.\n", status);
         goto TERMINATE;
      }
   }

TERMINATE:

   free_and_null ((char **) &rmatind);
   free_and_null ((char **) &rmatval);

   return status;

} /* END create_master_ILP */


/* This routine read an array of doubles from an input file  */

static int
read_array (FILE *in, int *num_p, double **data_p)
{
   int  status = 0;
   int  max, num;
   char ch;

   num = 0;
   max = 10;

   *data_p = malloc(max * sizeof(**data_p));
   if ( *data_p == NULL ) {
      status = CPXERR_NO_MEMORY;
      goto TERMINATE;
   }

   for (;;) {
      int read = fscanf (in, "%c", &ch);
      if ( read == 0 ) {
         status = -1;
         goto TERMINATE;
      }
      if ( ch == '\t' ||
           ch == '\r' ||
           ch == ' '  ||
           ch == '\n'   ) continue;
      if ( ch == '[' ) break;
      status = -1;
      goto TERMINATE;
   }

   for(;;) {
      int read = fscanf (in, "%lg", (*data_p)+num);
      if ( read == 0 ) {
         status = -1;
         goto TERMINATE;
      }
      num++;
      if ( num >= max ) {
         max *= 2;
         *data_p = realloc(*data_p, max * sizeof(**data_p));
         if ( *data_p == NULL ) {
            status = CPXERR_NO_MEMORY;
            goto TERMINATE;
         }
      }
      do {
         read = fscanf (in, "%c", &ch);
         if ( read == 0 ) {
            status = -1;
            goto TERMINATE;
         }
      } while (ch == ' ' || ch == '\n' || ch == '\t'  || ch == '\r');
      if ( ch == ']' ) break;
      else if ( ch != ',' ) {
         status = -1;
         goto TERMINATE;
      }
   }

   *num_p = num;

TERMINATE:

   return status;

} /* END read_array */


/* This routine read an ATSP instance from an input file  */

static int
read_ATSP (const char* file, double ***arc_cost_p, int *num_nodes_p)
{
   int status = 0;

   int  i, n;
   char ch;
   FILE *in = NULL;

   *arc_cost_p = NULL;
   *num_nodes_p = 0;

   in = fopen(file, "r");
   if ( in == NULL ) {
      fprintf (stderr, "Unable to open file %s.\n", file);
      status = -1;
      goto TERMINATE;
   }

   *arc_cost_p = malloc(1 * sizeof(**arc_cost_p));
   if ( *arc_cost_p == NULL ) {
      status = CPXERR_NO_MEMORY;
      goto TERMINATE;
   }
   *num_nodes_p = 1;
   (*arc_cost_p)[0] = NULL;

   for (;;) {
      int read = fscanf (in, "%c", &ch);
      if ( read == 0 ) {
         status = -1;
         goto TERMINATE;
      }
      if ( ch == '\t' ||
           ch == '\r' ||
           ch == ' '  ||
           ch == '\n'   ) continue;
      if ( ch == '[' ) break;
      status = -1;
      goto TERMINATE;
   }

   status = read_array(in, &n, *arc_cost_p);
   if (status ) goto TERMINATE;

   for (;;) {
      int read = fscanf (in, "%c", &ch);
      if ( read == 0 ) {
         status = -1;
         goto TERMINATE;
      }
      if ( ch == '\t' ||
           ch == '\r' ||
           ch == ' '  ||
           ch == '\n'   ) continue;
      if ( ch == ',' ) break;
      status = -1;
      goto TERMINATE;
   }

   *arc_cost_p = realloc(*arc_cost_p, n * sizeof(**arc_cost_p));
   if ( *arc_cost_p == NULL ) {
      status = CPXERR_NO_MEMORY;
      *num_nodes_p = 0;
      goto TERMINATE;
   }
   *num_nodes_p = n;
   for (i = 1; i < *num_nodes_p; ++i) {
      (*arc_cost_p)[i] = NULL;
   }

   for (i = 1; i < *num_nodes_p; ++i) {
      if ( (status = read_array(in, &n, (*arc_cost_p)+i)) ) goto TERMINATE;
      if ( n != *num_nodes_p ) {
         status = -1;
         goto TERMINATE;
      }
      for (;;) {
         int read = fscanf (in, "%c", &ch);
         if ( read == 0 ) {
            status = -1;
            goto TERMINATE;
         }
         if ( ch == '\t' ||
              ch == '\r' ||
              ch == ' '  ||
              ch == '\n'   ) continue;
         if ( ch == ',' && i <  *num_nodes_p - 1) break;
         if ( ch == ']' && i == *num_nodes_p - 1) break;
         status = -1;
         goto TERMINATE;
      }
   }

TERMINATE:

   if ( in != NULL) fclose (in);

   return status;

} /* END read_ATSP */


/* This routine frees up the pointer *ptr, and sets *ptr to
   NULL */

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
      "Usage:     %s {0|1} [filename]\n", progname);
   fprintf (stderr,
      " 0:        Benders' cuts only used as lazy constraints,\n");
   fprintf (stderr,
      "           to separate integer infeasible solutions.\n");
   fprintf (stderr,
      " 1:        Benders' cuts also used as user cuts,\n");
   fprintf (stderr,
      "           to separate fractional infeasible solutions.\n");
   fprintf (stderr,
      " filename: ATSP instance file name.\n");
   fprintf (stderr,
      "           File ../../../examples/data/atsp.dat used if no name is provided.\n");

} /* END usage */

