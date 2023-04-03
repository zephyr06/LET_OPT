/* --------------------------------------------------------------------------
 * File: ilogenericbranch.cpp
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
// Demonstrates how to perform customized branching using the generic
// callback.
//
// For any model with integer variables passed on the command line, the
// code will solve the model using a simple customized branching strategy.
// The branching strategy implemented here is most-infeasible branching,
// i.e., the code always picks the integer variable that is most fractional
// and then branches on it. If the biggest fractionality of all integer
// variables is small then the code refrains from custom branching and lets
// CPLEX decide.
//
// See the usage message below.

#include <cmath>
#include <iostream>
#include <ilcplex/ilocplex.h>

using std::cerr;
using std::cout;
using std::endl;

// Print a usage message and exit.
static void usage(const char *progname)
{
   cerr << "Usage: " << progname << " filename..." << endl
        << "  filename   Name of a file, or multiple files, with .mps, .lp," << endl
        << "             or .sav extension, and a possible, additional .gz"  << endl
        << "             extension." << endl;
   exit(2);
}

// Generic callback that implements most infeasible branching.
class BranchCallback : public IloCplex::Callback::Function {
   IloNumVarArray x;
   int calls;
   int branches;
public:
   BranchCallback(IloNumVarArray _x) : x(_x), calls(0) , branches(0) {
   }

   void invoke(IloCplex::Callback::Context const &context) ILO_OVERRIDE {
      // NOTE: Strictly speaking, the increment of calls and branches
      //       should be protected by a lock/mutex/semaphore. However, to keep
      //       the code simple we don't do that here.
      ++calls;

      // For sake of illustration prune every node that has a depth larger
      // than 1000.
      IloInt depth = context.getLongInfo(IloCplex::Callback::Context::Info::NodeDepth);
      if ( depth > 1000 ) {
         context.pruneCurrentNode();
         return;
      }

      // Get the current relaxation.
      // The function not only fetches the objective value but also makes sure
      // the node lp is solved and returns the node lp's status. That status can
      // be used to identify numerical issues or similar
      IloCplex::CplexStatus status = context.getRelaxationStatus(0);
      double obj = context.getRelaxationObjective();

      // Only branch if the current node relaxation could be solved to
      // optimality.
      // If there was any sort of trouble then don't do anything and thus let
      // CPLEX decide how to cope with that.
      if ( status != IloCplex::Optimal       &&
           status != IloCplex::OptimalInfeas   ) {
         return;
      }

      IloNumArray v(context.getEnv());

      // Node lp was solved to optimality. Grab the current relaxation
      // and find the most fractional variable
      context.getRelaxationPoint(x, v);
      IloInt maxVar = -1;
      IloNum maxFrac = 0.0;
      for (IloInt i = 0; i < x.getSize(); ++i) {
         if ( x[i].getType() != IloNumVar::Float ) {
            double const intval = ::round(v[i]);
            double const frac = ::fabs(intval - v[i]);

            if ( frac > maxFrac ) {
               maxFrac = frac;
               maxVar = i;
            }
         }
      }

      // If the maximum fractionality of all integer variables is small then
      // don't create a custom branch. Instead let CPLEX decide how to
      // branch.
      IloNum minFrac = 0.1;
      if ( maxFrac > minFrac ) {
         // There is a variable with a sufficiently fractional value.
         // Branch on that variable.
         CPXLONG upChild, downChild;
         double const up = ::ceil (v[maxVar]);
         double const down = ::floor (v[maxVar]);
         IloNumVar branchVar = x[maxVar];

         // Create UP branch (branchVar >= up)
         upChild = context.makeBranch(branchVar, up, IloCplex::BranchUp, obj);
         ++branches;

         // Create DOWN branch (branchVar <= down)
         downChild = context.makeBranch(branchVar, down, IloCplex::BranchDown, obj);
         ++branches;

         // We don't use the unique ids of the down and up child. We only
         // have them so illustrate how they are returned from
         // CPXcallbackmakebranch().
         (void)downChild;
         (void)upChild;
      }
      v.end();
   }

   int getCalls() const { return calls; }
   int getBranches() const { return branches; }
};

int
main (int argc, char *argv[])
{
   if ( argc <= 1 )
      usage(argv[0]);

   /* Loop over all command line arguments. */
   for (int a = 1; a < argc; ++a) {
      IloEnv env;

      // Read in the model
      IloModel model(env);
      IloCplex cplex(model);
      IloObjective obj(env);
      IloNumVarArray x(env);
      IloRangeArray cons(env);
      cplex.importModel(model, argv[a], obj, x, cons);

      // Register the callback function.
      BranchCallback cb(x);
      cplex.use(&cb, IloCplex::Callback::Context::Id::Branching);

      // Limit the number of nodes.
      // The branching strategy implemented here is not smart so solving even
      // a simple MIP may turn out to take a long time.
      cplex.setParam(IloCplex::Param::MIP::Limits::Nodes, 1000);


      // Solve the model and report some statistics
      bool feasible = cplex.solve();
      cout << "Model " << argv[a] << " solved, status = " << cplex.getStatus()
           << endl;
      if ( feasible )
         cout << "Objective value: " << cplex.getObjValue() << endl;
      cout << "Callback was invoked " << cb.getCalls() << " times and created "
           << cb.getBranches() << " branches" << endl;

      env.end();
   }

   return 0;
}
