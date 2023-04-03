// -------------------------------------------------------------- -*- C++ -*-
// File: ilobendersatsp.cpp
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
// Example ilobendersatsp.cpp solves a flow MILP model for an
// Asymmetric Traveling Salesman Problem (ATSP) instance
// through Benders decomposition.
//
// The arc costs of an ATSP instance are read from an input file.
// The flow MILP model is decomposed into a master ILP and a worker LP.
//
// The master ILP is then solved by adding Benders' cuts during
// the branch-and-cut process via the cut callback functions.
// The cut callback functions add to the master ILP violated Benders' cuts
// that are found by solving the worker LP.
//
// The example allows the user to decide if Benders' cuts have to be separated:
//
// a) Only to separate integer infeasible solutions.
// In this case, Benders' cuts are treated as lazy constraints through the
// class IloCplex::LazyConstraintCallbackI.
//
// b) Also to separate fractional infeasible solutions.
// In this case, Benders' cuts are treated as lazy constraints through the
// class IloCplex::LazyConstraintCallbackI.
// In addition, Benders' cuts are also treated as user cuts through the
// class IloCplex::UserCutCallbackI.
//
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
ILOSTLBEGIN

typedef IloArray<IloIntVarArray> Arcs;


// Declarations for functions in this program

void createMasterILP(IloModel mod, Arcs x, IloNumArray2 arcCost);

void createWorkerLP(IloCplex cplex, IloNumVarArray v, IloNumVarArray u,
                   IloObjective obj, IloInt numNodes);

IloBool separate(const Arcs x, const IloNumArray2 xSol, IloCplex cplex,
                 const IloNumVarArray v, const IloNumVarArray u,
                 IloObjective obj, IloExpr cutLhs, IloNum& cutRhs);

void usage(char *progname);


// Implementation class for the user-defined lazy constraint callback.
// The function BendersLazyCallback allows to add Benders' cuts as lazy constraints.
//
ILOLAZYCONSTRAINTCALLBACK5(BendersLazyCallback, Arcs, x, IloCplex, workerCplex,
                           IloNumVarArray, v, IloNumVarArray, u,
                           IloObjective, workerObj)
{
   IloInt i;
   IloEnv masterEnv = getEnv();
   IloInt numNodes = x.getSize();

   // Get the current x solution
  
   IloNumArray2 xSol(masterEnv, numNodes);
   for (i = 0; i < numNodes; ++i) {
      xSol[i] = IloNumArray(masterEnv);
      getValues(xSol[i], x[i]);
   }

   // Benders' cut separation

   IloExpr cutLhs(masterEnv);
   IloNum cutRhs;
   IloBool sepStat = separate(x, xSol, workerCplex, v, u, workerObj, cutLhs, cutRhs);
   if ( sepStat ) {
      add(cutLhs >= cutRhs).end();
   }

   // Free memory

   cutLhs.end();
   for (i = 0; i < numNodes; ++i)
      xSol[i].end();
   xSol.end();

   return;

} // END BendersLazyCallback


// Implementation class for the user-defined user cut callback.
// The function BendersUserCallback allows to add Benders' cuts as user cuts.
//
ILOUSERCUTCALLBACK5(BendersUserCallback, Arcs, x, IloCplex, workerCplex,
                    IloNumVarArray, v, IloNumVarArray, u,
                    IloObjective, workerObj)
{
   // Skip the separation if not at the end of the cut loop

   if ( !isAfterCutLoop() )
      return;

   IloInt i;
   IloEnv masterEnv = getEnv();
   IloInt numNodes = x.getSize();

   // Get the current x solution

   IloNumArray2 xSol(masterEnv, numNodes);
   for (i = 0; i < numNodes; ++i) {
      xSol[i] = IloNumArray(masterEnv);
      getValues(xSol[i], x[i]);
   }

   // Benders' cut separation

   IloExpr cutLhs(masterEnv);
   IloNum cutRhs;
   IloBool sepStat = separate(x, xSol, workerCplex, v, u, workerObj, cutLhs, cutRhs);
   if ( sepStat ) {
      add(cutLhs >= cutRhs).end();
   }
   
   // Free memory

   cutLhs.end();
   for (i = 0; i < numNodes; ++i)
      xSol[i].end();
   xSol.end();

   return;

} // END BendersUserCallback


int
main(int argc, char **argv)
{
   IloEnv masterEnv;
   IloEnv workerEnv;

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

      // Create worker IloCplex algorithm and worker LP for Benders' cuts separation

      IloCplex workerCplex(workerEnv);
      IloNumVarArray v(workerEnv);
      IloNumVarArray u(workerEnv);
      IloObjective workerObj(workerEnv);
      createWorkerLP(workerCplex, v, u, workerObj, numNodes);

      // Set up the cut callback to be used for separating Benders' cuts

      IloCplex masterCplex(masterMod);
      masterCplex.setParam(IloCplex::Param::Preprocessing::Presolve, IloFalse); 

      // Set the maximum number of threads to 1. 
      // This instruction is redundant: If MIP control callbacks are registered, 
      // then by default CPLEX uses 1 (one) thread only.
      // Note that the current example may not work properly if more than 1 threads 
      // are used, because the callback functions modify shared global data.
      // We refer the user to the documentation to see how to deal with multi-thread 
      // runs in presence of MIP control callbacks. 

      masterCplex.setParam(IloCplex::Param::Threads, 1); 

      // Turn on traditional search for use with control callbacks

      masterCplex.setParam(IloCplex::Param::MIP::Strategy::Search,
                           IloCplex::Traditional);
      
      masterCplex.use(BendersLazyCallback(masterEnv, x, workerCplex, v, u, workerObj));
      if ( separateFracSols )
         masterCplex.use(BendersUserCallback(masterEnv, x, workerCplex, v, u, workerObj));

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
               for(j = 0; j < numNodes; j++) {
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
   workerEnv.end();

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


// This routine set up the IloCplex algorithm to solve the worker LP, and
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
//
void
createWorkerLP(IloCplex cplex, IloNumVarArray v, IloNumVarArray u, 
               IloObjective obj, IloInt numNodes)
{

   IloInt i, j, k;
   IloEnv env = cplex.getEnv();
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

   IloInt numArcs  = numNodes * numNodes;
   IloInt vNumVars = (numNodes-1) * numArcs;
   IloNumVarArray vTemp(env, vNumVars, 0, IloInfinity);
   for (k = 1; k < numNodes; ++k) {
      for (i = 0; i < numNodes; ++i) {
         vTemp[(k-1)*numArcs + i *numNodes + i].setBounds(0, 0);
      }
   }
   v.clear();
   v.add(vTemp);
   vTemp.end();
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

   IloIntArray vIndex(env, vNumVars);
   for (j = 0; j < vNumVars; ++j)
   {
      vIndex[j] = j;
      v[j].setObject(&vIndex[j]);
   }

   // Create variables u(k,i) forall k in V0, i in V

   IloInt uNumVars = (numNodes-1) * numNodes;
   IloNumVarArray uTemp(env, uNumVars, -IloInfinity, IloInfinity);
   u.clear();
   u.add(uTemp);
   uTemp.end();
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

   IloIntArray uIndex(env, uNumVars);
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

}// END createWorkerLP


// This routine separates Benders' cuts violated by the current x solution.
// Violated cuts are found by solving the worker LP
//
IloBool
separate(const Arcs x, const IloNumArray2 xSol, IloCplex cplex,
         const IloNumVarArray v, const IloNumVarArray u,
         IloObjective obj, IloExpr cutLhs, IloNum& cutRhs)
{
   IloBool violatedCutFound = IloFalse;

   IloEnv env = cplex.getEnv();
   IloModel mod = cplex.getModel();

   IloInt numNodes = xSol.getSize();
   IloInt numArcs  = numNodes * numNodes;
   IloInt i, j, k, h;

   // Update the objective function in the worker LP:
   // minimize sum(k in V0) sum((i,j) in A) x(i,j) * v(k,i,j)
   //          - sum(k in V0) u(k,0) + sum(k in V0) u(k,k)
   
   mod.remove(obj);
   IloExpr objExpr = obj.getExpr();
   objExpr.clear();
   for (k = 1; k < numNodes; ++k) {
      for (i = 0; i < numNodes; ++i) {
         for (j = 0; j < numNodes; ++j) {
               objExpr +=  xSol[i][j] * v[(k-1)*numArcs + i*numNodes + j];
         }
      }
   }
   for (k = 1; k < numNodes; ++k) {
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

      IloInt vNumVars = (numNodes-1) * numArcs;
      IloNumVarArray var(env);
      IloNumArray val(env);

      // Get the violated cut as an unbounded ray of the worker LP

      cplex.getRay(val, var);

      // Compute the cut from the unbounded ray. The cut is:
      // sum((i,j) in A) (sum(k in V0) v(k,i,j)) * x(i,j) >=
      // sum(k in V0) u(k,0) - u(k,k)

      cutLhs.clear();
      cutRhs = 0.;

      for (h = 0; h < val.getSize(); ++h) {

         IloInt *index_p = (IloInt*) var[h].getObject();
         IloInt index = *index_p;

         if ( index >= vNumVars ) {
            index -= vNumVars;
            k = index / numNodes + 1;
            i = index - (k-1)*numNodes;
            if ( i == 0 )
               cutRhs += val[h];
            else if ( i == k )
               cutRhs -= val[h];
         }
         else {
            k = index / numArcs + 1;
            i = (index - (k-1)*numArcs) / numNodes;
            j = index - (k-1)*numArcs - i*numNodes;
            cutLhs += val[h] * x[i][j];
         }
      }

      var.end();
      val.end();

      violatedCutFound = IloTrue;
   }

   return violatedCutFound;

} // END separate


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
