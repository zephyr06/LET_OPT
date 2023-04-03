// -------------------------------------------------------------- -*- C++ -*-
// File: ilobendersatsp2.cpp
// Version 22.1.0
// --------------------------------------------------------------------------
// Licensed Materials - Property of IBM
// 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
// Copyright IBM Corporation 2000, 2022. All Rights Reserved.
//
// US Government Users Restricted Rights - Use, duplication or
// disclosure restricted by GSA ADP Schedule Contract with
// IBM Corp.
// --------------------------------------------------------------------------
//
//
// Example ilobendersatsp2.cpp solves a flow MILP model for an
// Asymmetric Traveling Salesman Problem (ATSP) instance
// through Benders decomposition.
//
// The arc costs of an ATSP instance are read from an input file.
// The flow MILP model is decomposed into a master ILP and a worker LP.
//
// The master ILP is then solved by adding Benders' cuts via the generic
// callback function benders_callback during the branch-and-cut process.
//
// The callback benders_callback adds to the master ILP violated Benders'
// cuts that are found by solving the worker LP.
//
// The example allows the user to decide if Benders' cuts have to be separated
// just as lazy constraints or also as user cuts. In particular:
//
// a) Only to separate integer infeasible solutions.
// In this case, benders_callback is called with
// contextid=CPX_CALLBACKCONTEXT_CANDIDATE. The current candidate integer
// solution can be queried with CPXXcallbackgetcandidatepoint, and it can be rejected
// by the user, optionally providing a list of lazy constraints, with the
// function CPXXcallbackrejectcandidate.
//
// b) Also to separate fractional infeasible solutions.
// In this case, benders_callback is called with
// contextid=CPX_CALLBACKCONTEXT_RELAXATION. The current fractional solution
// can be queried with CPXXcallbackgetrelaxationpoint. Cutting planes can then
// be added via CPXXcallbackaddusercuts.
//
// The example shows how to properly support deterministic parallel search
// with a user callback (there a significant departure here frome the legacy
// control callbacks):
//
// a) To avoid race conditions (as the callback is called simultaneously by
// multiple threads), each thread has its own working copy of the data
// structures needed to separate cutting planes. Access to global data
// is read-only.
//
// b) Thread-local data for all threads is created on THREAD_UP
// and destroyed on THREAD_DOWN. This guarantees determinism.
//
// To run this example, command line arguments are required:
//     ilobendersatsp.cpp {0|1} [filename]
// where
//     0         Indicates that Benders' cuts are only used as lazy constraints,
//               to separate integer infeasible solutions.
//     1         Indicates that Benders' cuts are also used as user cuts,
//               to separate fractional infeasible solutions.
//
//     filename  Is the name of the file containing the ATSP instance (arc costs).
//               If filename is not specified, the instance
//               ../../../examples/data/atsp.dat is read
//
//
// ATSP instance defined on a directed graph G = (V, A)
// - V = {0, ..., n-1}, V0 = V \ {0}
// - A = {(i,j) : i in V, j in V, i != j }
// - forall i in V: delta+(i) = {(i,j) in A : j in V}
// - forall i in V: delta-(i) = {(j,i) in A : j in V}
// - c(i,j) = traveling cost associated with (i,j) in A
//
// Flow MILP model
//
// Modeling variables:
// forall (i,j) in A:
//    x(i,j) = 1, if arc (i,j) is selected
//           = 0, otherwise
// forall k in V0, forall (i,j) in A:
//    y(k,i,j) = flow of the commodity k through arc (i,j)
//
// Objective:
// minimize sum((i,j) in A) c(i,j) * x(i,j)
//
// Degree constraints:
// forall i in V: sum((i,j) in delta+(i)) x(i,j) = 1
// forall i in V: sum((j,i) in delta-(i)) x(j,i) = 1
//
// Binary constraints on arc variables:
// forall (i,j) in A: x(i,j) in {0, 1}
//
// Flow constraints:
// forall k in V0, forall i in V:
//    sum((i,j) in delta+(i)) y(k,i,j) - sum((j,i) in delta-(i)) y(k,j,i) = q(k,i)
//    where q(k,i) =  1, if i = 0
//                 = -1, if k == i
//                 =  0, otherwise
//
// Capacity constraints:
// forall k in V0, for all (i,j) in A: y(k,i,j) <= x(i,j)
//
// Nonnegativity of flow variables:
// forall k in V0, for all (i,j) in A: y(k,i,j) >= 0
//

#include <ilcplex/ilocplex.h>
#include <string>
#include <vector>

ILOSTLBEGIN

typedef IloArray<IloIntVarArray> Arcs;


// Declarations for functions in this program

void createMasterILP(IloModel mod, Arcs x, IloNumArray2 arcCost);

void usage(char *progname);


/** The BendersATSP thread-local class. */

class Worker
{
public:
   // The constructor sets up the IloCplex algorithm to solve the worker LP, and
   // creates the worker LP (i.e., the dual of flow constraints and
   // capacity constraints of the flow MILP)
   //
   // Modeling variables:
   // forall k in V0, i in V:
   //    u(k,i) = dual variable associated with flow constraint (k,i)
   //
   // forall k in V0, forall (i,j) in A:
   //    v(k,i,j) = dual variable associated with capacity constraint (k,i,j)
   //
   // Objective:
   // minimize sum(k in V0) sum((i,j) in A) x(i,j) * v(k,i,j)
   //          - sum(k in V0) u(k,0) + sum(k in V0) u(k,k)
   //
   // Constraints:
   // forall k in V0, forall (i,j) in A: u(k,i) - u(k,j) <= v(k,i,j)
   //
   // Nonnegativity on variables v(k,i,j)
   // forall k in V0, forall (i,j) in A: v(k,i,j) >= 0
   Worker(IloInt _numNodes)
      : numNodes(_numNodes),
      numArcs(numNodes * numNodes),
      vNumVars((numNodes-1) * numArcs),
      uNumVars((numNodes-1) * numNodes),
      cplex(env),
      v(env, vNumVars, 0, IloInfinity),
      vIndex(env, vNumVars),
      u(env, uNumVars, -IloInfinity, IloInfinity),
      uIndex(env, uNumVars),
      obj(env)
   {
      IloInt i, j, k;
      IloModel mod(env, "atsp_worker");

      // Set up IloCplex algorithm to solve the worker LP
      cplex.extract(mod);
      cplex.setOut(env.getNullStream());

      // Turn off the presolve reductions and set the CPLEX optimizer
      // to solve the worker LP with primal simplex method.
      cplex.setParam(IloCplex::Param::Preprocessing::Reduce, 0);
      cplex.setParam(IloCplex::Param::RootAlgorithm, IloCplex::Primal);

      // Create variables v(k,i,j) forall k in V0, (i,j) in A
      // For simplicity, also dummy variables v(k,i,i) are created.
      // Those variables are fixed to 0 and do not partecipate to
      // the constraints.
      for (k = 1; k < numNodes; ++k) {
         for (i = 0; i < numNodes; ++i) {
            v[(k-1)*numArcs + i *numNodes + i].setBounds(0, 0);
         }
      }
      mod.add(v);

      // Set names for variables v(k,i,j)
      for (k = 1; k < numNodes; ++k) {
         for(i = 0; i < numNodes; ++i) {
            for(j = 0; j < numNodes; ++j) {
               char varName[100];
               sprintf(varName, "v.%d.%d.%d", (int) k, (int) i, (int) j);
               v[(k-1)*numArcs + i*numNodes + j].setName(varName);
            }
         }
      }

      // Associate indices to variables v(k,i,j)
      for (j = 0; j < vNumVars; ++j)
      {
         vIndex[j] = j;
         v[j].setObject(&vIndex[j]);
      }

      // Create variables u(k,i) forall k in V0, i in V
      mod.add(u);

      // Set names for variables u(k,i)
      for (k = 1; k < numNodes; ++k) {
         for(i = 0; i < numNodes; ++i) {
            char varName[100];
            sprintf(varName, "u.%d.%d", (int) k, (int) i);
            u[(k-1)*numNodes + i].setName(varName);
         }
      }

      // Associate indices to variables u(k,i)
      for (j = 0; j < uNumVars; ++j)
      {
         uIndex[j] = vNumVars + j;
         u[j].setObject(&uIndex[j]);
      }

      // Initial objective function is empty
      obj.setSense(IloObjective::Minimize);
      mod.add(obj);

      // Add constraints:
      // forall k in V0, forall (i,j) in A: u(k,i) - u(k,j) <= v(k,i,j)
      for (k = 1; k < numNodes; ++k) {
         for(i = 0; i < numNodes; ++i) {
            for(j = 0; j < numNodes; ++j) {
               if ( i != j ) {
                  IloExpr expr(env);
                  expr -= v[(k-1)*numArcs + i*(numNodes) + j];
                  expr += u[(k-1)*numNodes + i];
                  expr -= u[(k-1)*numNodes + j];
                  mod.add(expr <= 0);
                  expr.end();
               }
            }
         }
      }
   }
   ~Worker()
   {
      // Free all memory associated to env
      env.end();
   }

   // This routine separates Benders' cuts violated by the current x solution.
   // Violated cuts are found by solving the worker LP
   IloBool separate(const Arcs& x, const IloNumArray2& xSol, IloExpr& cutLhs, IloNum& cutRhs)
   {
      IloBool violatedCutFound = IloFalse;
      IloModel mod = cplex.getModel();

      // Update the objective function in the worker LP:
      // minimize sum(k in V0) sum((i,j) in A) x(i,j) * v(k,i,j)
      //          - sum(k in V0) u(k,0) + sum(k in V0) u(k,k)
      mod.remove(obj);
      IloExpr objExpr = obj.getExpr();
      objExpr.clear();
      for (IloInt k = 1; k < numNodes; ++k) {
         for (IloInt i = 0; i < numNodes; ++i) {
            for (IloInt j = 0; j < numNodes; ++j) {
               objExpr +=  xSol[i][j] * v[(k-1)*numArcs + i*numNodes + j];
            }
         }
      }
      for (IloInt k = 1; k < numNodes; ++k) {
         objExpr += u[(k-1)*numNodes + k];
         objExpr -= u[(k-1)*numNodes];
      }
      obj.setExpr(objExpr);
      mod.add(obj);
      objExpr.end();

      // Solve the worker LP
      cplex.solve();

      // A violated cut is available iff the solution status is Unbounded
      if ( cplex.getStatus() == IloAlgorithm::Unbounded ) {

         IloNumVarArray var(env);
         IloNumArray val(env);

         // Get the violated cut as an unbounded ray of the worker LP
         cplex.getRay(val, var);

         // Compute the cut from the unbounded ray. The cut is:
         // sum((i,j) in A) (sum(k in V0) v(k,i,j)) * x(i,j) >=
         // sum(k in V0) u(k,0) - u(k,k)
         cutLhs.clear();
         cutRhs = 0.0;

         for (IloInt h = 0; h < val.getSize(); ++h) {
            IloInt *index_p = (IloInt*) var[h].getObject();
            IloInt index = *index_p;

            if ( index >= vNumVars ) {
               index -= vNumVars;
               IloInt k = index / numNodes + 1;
               IloInt i = index - (k-1)*numNodes;
               if ( i == 0 )
                  cutRhs += val[h];
               else if ( i == k )
                  cutRhs -= val[h];
            }
            else {
               IloInt k = index / numArcs + 1;
               IloInt i = (index - (k-1)*numArcs) / numNodes;
               IloInt j = index - (k-1)*numArcs - i*numNodes;
               cutLhs += val[h] * x[i][j];
            }
         }
         var.end();
         val.end();

         violatedCutFound = IloTrue;
      }

      return violatedCutFound;
   } // END separate
private:
   IloInt numNodes;
   IloInt numArcs;
   IloInt vNumVars;
   IloInt uNumVars;
   IloEnv env;
   IloCplex cplex;
   IloNumVarArray v;
   IloIntArray vIndex;
   IloNumVarArray u;
   IloIntArray uIndex;
   IloObjective obj;
};


class BendersATSPCallback : public IloCplex::Callback::Function
{
public:
   BendersATSPCallback(Arcs _x, IloInt numWorkers=1)
      : x(_x), workers(numWorkers, 0) {}

   ~BendersATSPCallback()
   {
      IloInt numWorkers = workers.size();
      for (IloInt w = 0; w < numWorkers; w++) {
         delete workers[w];
      }
      workers.clear();
   }

   void invoke(const IloCplex::Callback::Context& context) ILO_OVERRIDE
   {
      int const threadNo = context.getIntInfo(IloCplex::Callback::Context::Info::ThreadId);
      IloInt numNodes = x.getSize();

      // setup
      if (context.inThreadUp()) {
         delete workers[threadNo];
         workers[threadNo] = new Worker(numNodes);
         return;
      }

      // teardown
      if (context.inThreadDown()) {
         delete workers[threadNo];
         workers[threadNo] = 0;
         return;
      }

      IloEnv env = context.getEnv();
      IloNumArray2 xSol(env, numNodes);

      // Get the current x solution
      switch (context.getId()) {
      case IloCplex::Callback::Context::Id::Candidate:
         if ( !context.isCandidatePoint() ) // The model is always bounded
            throw IloCplex::Exception(-1, "Unbounded solution");
         for (IloInt i = 0; i < numNodes; ++i) {
            xSol[i] = IloNumArray(env);
            context.getCandidatePoint(x[i], xSol[i]);
         }
         break;
      case IloCplex::Callback::Context::Id::Relaxation:
         for (IloInt i = 0; i < numNodes; ++i) {
            xSol[i] = IloNumArray(env);
            context.getRelaxationPoint(x[i], xSol[i]);
         }
         break;
      default:
         // Free memory
         for (IloInt i = 0; i < numNodes; ++i) xSol[i].end();
         xSol.end();
         throw IloCplex::Exception(-1, "Unexpected contextID");
      }

      // Get the right worker
      Worker* worker = workers[threadNo];

      // Separate cut
      IloExpr cutLhs(env);
      IloNum cutRhs;
      IloBool sepStat = worker->separate(x, xSol, cutLhs, cutRhs);

      // Free memory
      for (IloInt i = 0; i < numNodes; ++i) xSol[i].end();
      xSol.end();

      if (sepStat) {
         // Add the cut
         IloRange r(env, cutRhs, cutLhs, IloInfinity);

         switch (context.getId()) {
            case IloCplex::Callback::Context::Id::Candidate:
               context.rejectCandidate(r);
               break;
            case IloCplex::Callback::Context::Id::Relaxation:
               context.addUserCut(r,
                                  IloCplex::UseCutPurge,
                                  IloFalse);
               break;
            default:
               r.end();
               throw IloCplex::Exception(-1, "Unexpected contextID");
         }
         r.end();
      }
   }
private:
   Arcs x;
   std::vector<Worker*> workers;
};



int
main(int argc, char **argv)
{
   IloEnv masterEnv;

   try {
      const char* fileName = "../../../examples/data/atsp.dat";

       // Check the command line arguments
      if ( argc != 2 && argc != 3) {
         usage (argv[0]);
         throw (-1);
      }

      if ( (argv[1][0] != '1' && argv[1][0] != '0') ||
           argv[1][1] != '\0' ) {
         usage (argv[0]);
         throw (-1);
      }

      IloBool separateFracSols = ( argv[1][0] == '0' ? IloFalse : IloTrue );

      masterEnv.out() << "Benders' cuts separated to cut off: ";
      if ( separateFracSols ) {
         masterEnv.out() << "Integer and fractional infeasible solutions." << endl;
      }
      else {
         masterEnv.out() << "Only integer infeasible solutions." << endl;
      }

      if ( argc == 3 )  fileName = argv[2];

      // Read arc_costs from data file (9 city problem)
      IloNumArray2 arcCost(masterEnv);
      ifstream data(fileName);
      if ( !data ) throw(-1);
      data >> arcCost;
      data.close();

      // create master ILP
      IloModel masterMod(masterEnv, "atsp_master");
      IloInt numNodes = arcCost.getSize();
      Arcs x(masterEnv, numNodes);
      createMasterILP(masterMod, x, arcCost);
      IloCplex masterCplex(masterMod);

      int numThreads = masterCplex.getNumCores();

      // Set up the callback to be used for separating Benders' cuts
      BendersATSPCallback cb(x, numThreads);
      CPXLONG contextmask = IloCplex::Callback::Context::Id::Candidate
                            | IloCplex::Callback::Context::Id::ThreadUp
                            | IloCplex::Callback::Context::Id::ThreadDown;
      if ( separateFracSols )
         contextmask |= IloCplex::Callback::Context::Id::Relaxation;
      masterCplex.use(&cb, contextmask);

      // Solve the model and write out the solution
      if ( masterCplex.solve() ) {

         IloAlgorithm::Status solStatus= masterCplex.getStatus();
         masterEnv.out() << endl << "Solution status: " << solStatus << endl;

         masterEnv.out() << "Objective value: "
                         << masterCplex.getObjValue() << endl;

         if ( solStatus == IloAlgorithm::Optimal ) {

            // Write out the optimal tour
            IloInt i, j;
            IloNumArray2 sol(masterEnv, numNodes);
            IloIntArray succ(masterEnv, numNodes);
            for (j = 0; j < numNodes; ++j)
               succ[j] = -1;

            for (i = 0; i < numNodes; i++) {
               sol[i] = IloNumArray(masterEnv);
               masterCplex.getValues(sol[i], x[i]);
               for (j = 0; j < numNodes; j++) {
                  if ( sol[i][j] > 1e-03 ) succ[i] = j;
               }
            }

            masterEnv.out() << "Optimal tour:" << endl;
            i = 0;
            while ( succ[i] != 0 ) {
               masterEnv.out() << i << ", ";
               i = succ[i];
            }
            masterEnv.out() << i << endl;
         }
         else {
            masterEnv.out() << "Solution status is not Optimal" << endl;
         }
      }
      else {
         masterEnv.out() << "No solution available" << endl;
      }

   }
   catch (const IloException& e) {
      cerr << "Exception caught: " << e << endl;
   }
   catch (...) {
      cerr << "Unknown exception caught!" << endl;
   }

   // Close the environments
   masterEnv.end();

   return 0;
} // END main


// This routine creates the master ILP (arc variables x and degree constraints).
//
// Modeling variables:
// forall (i,j) in A:
//    x(i,j) = 1, if arc (i,j) is selected
//           = 0, otherwise
//
// Objective:
// minimize sum((i,j) in A) c(i,j) * x(i,j)
//
// Degree constraints:
// forall i in V: sum((i,j) in delta+(i)) x(i,j) = 1
// forall i in V: sum((j,i) in delta-(i)) x(j,i) = 1
//
// Binary constraints on arc variables:
// forall (i,j) in A: x(i,j) in {0, 1}
//
void
createMasterILP(IloModel mod, Arcs x, IloNumArray2 arcCost)
{
   IloInt i, j;
   IloEnv env = mod.getEnv();
   IloInt numNodes = x.getSize();

   // Create variables x(i,j) for (i,j) in A
   // For simplicity, also dummy variables x(i,i) are created.
   // Those variables are fixed to 0 and do not partecipate to
   // the constraints.

   char varName[100];
   for (i = 0; i < numNodes; ++i) {
      x[i] = IloIntVarArray(env, numNodes, 0, 1);
      x[i][i].setBounds(0, 0);
      for (j = 0; j < numNodes; ++j) {
         sprintf(varName, "x.%d.%d", (int) i, (int) j);
         x[i][j].setName(varName);
      }
      mod.add(x[i]);
   }

   // Create objective function: minimize sum((i,j) in A ) c(i,j) * x(i,j)

   IloExpr obj(env);
   for (i = 0; i < numNodes; ++i) {
      arcCost[i][i] = 0;
      obj += IloScalProd(x[i], arcCost[i]);
   }
   mod.add(IloMinimize(env, obj));
   obj.end();

   // Add the out degree constraints.
   // forall i in V: sum((i,j) in delta+(i)) x(i,j) = 1

   for (i = 0; i < numNodes; ++i) {
      IloExpr expr(env);
      for (j = 0;   j < i; ++j)  expr += x[i][j];
      for (j = i+1; j < numNodes; ++j)  expr += x[i][j];
      mod.add(expr == 1);
      expr.end();
   }

   // Add the in degree constraints.
   // forall i in V: sum((j,i) in delta-(i)) x(j,i) = 1

   for (i = 0; i < numNodes; i++) {
      IloExpr expr(env);
      for (j = 0;   j < i; j++)  expr += x[j][i];
      for (j = i+1; j < numNodes; j++)  expr += x[j][i];
      mod.add(expr == 1);
      expr.end();
   }

}// END createMasterILP


void usage (char *progname)
{
   cerr << "Usage:     " << progname << " {0|1} [filename]"                 << endl;
   cerr << " 0:        Benders' cuts only used as lazy constraints,"        << endl;
   cerr << "           to separate integer infeasible solutions."           << endl;
   cerr << " 1:        Benders' cuts also used as user cuts,"               << endl;
   cerr << "           to separate fractional infeasible solutions."        << endl;
   cerr << " filename: ATSP instance file name."                            << endl;
   cerr << "           File ../../../examples/data/atsp.dat "
        << "used if no name is provided."                                   << endl;

} // END usage

