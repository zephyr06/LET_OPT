// -------------------------------------------------------------- -*- C++ -*-
// File: iloadmipex9.cpp
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
// iloadmipex9.cpp -  Inject heuristic solutions from the generic callback.
//
// Optimizes an all binary MIP problem.
//
// See iloadmipex2.cpp for an implementation using legacy callbacks.
//
// To run this example, the user must specify a problem file.
//
// Example:
//     iloadmipex9 example.mps

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN
#include <math.h>
#include <map>
#include <algorithm>


static void usage (const char *progname);

// A generic callback class that injects heuristic solutions.
class HeuristicCallback: public IloCplex::Callback::Function {
   public:
      // Keeps track of the number of times the roundDown method is called.
      int timesCalled;

      // Constructor with data
      HeuristicCallback(IloCplex cplex, IloNumVarArray _vars) :
         timesCalled(0),
         vars(_vars),
         obj(cplex.getEnv(), vars.getSize())
      {
         // Generate the objective as a double array for easy look up.
         IloObjective objexpr = cplex.getObjective();
         std::map<IloInt, double> objmap;

         // First create a map with all variables that have non-zero
         // coefficients in the objective.
         for (IloExpr::LinearIterator it = IloExpr(objexpr.getExpr()).getLinearIterator(); it.ok(); ++it) {
            objmap[it.getVar().getId()] = it.getCoef();
         }

         // Now turn the map into a dense array.
         IloInt cols = vars.getSize();
         for (IloInt j = 0; j  <  cols; ++j) {
            std::map<IloInt, double>::iterator it = objmap.find(vars[j].getId());
            if ( it != objmap.end() ) {
               obj[j] = it->second;
            }
         }
      }

      // Heuristic motivated by knapsack constrained problems.
      //
      // Rounding down all fractional values will give an integer
      // solution that is feasible, since all constraints are <=
      // with positive coefficients.
      void roundDown (const IloCplex::Callback::Context& context) {
         IloNumArray x(context.getEnv());
         try {
            context.getRelaxationPoint(vars, x);
            double relobj = context.getRelaxationObjective();
            IloInt cols = vars.getSize();

            timesCalled++;

            for (IloInt j = 0; j < cols; j++) {
               // Set the fractional variable to zero.
               // Note that we assume all-binary variables. If there are
               // non-binary variables then the update must of course be
               // different.
               if ( x[j] ) {
                  double integral;
                  double frac = modf(x[j], &integral);
                  frac        = (std::min) (1.0-frac,frac);

                  if ( frac > 1.0e-6 ) {
                     relobj -= obj[j]*x[j];
                     x[j]    = 0.0;
                  }
               }
            }

            // Post the rounded solution, CPLEX will check feasibility.
            context.postHeuristicSolution(
               vars, x, relobj,
               IloCplex::Callback::Context::SolutionStrategy::CheckFeasible);
            x.end();
         }
         catch (...) {
            x.end();
            throw;
         }
      }

      // This is the function that we have to implement and that CPLEX will call
      // during the solution process at the places that we asked for.
      virtual void invoke (const IloCplex::Callback::Context& context) ILO_OVERRIDE;

   private:
      // All variables in the model.
      IloNumVarArray vars;

      // Dense objective vector (aligned with vars).
      IloNumArray obj;
};

// Implementation of the invoke method.
//
// This is the method that we have to implement to fulfill the
// generic callback contract. CPLEX will call this method during
// the solution process at the places that we asked for.
void
HeuristicCallback::invoke (const IloCplex::Callback::Context &context)
{
   if ( context.inRelaxation() ) {
      // Call rounding heuristic
      roundDown (context);
   }
}

// Use the generic callback for optimizing a MIP problem.
int
main (int argc, char **argv)
{
   IloEnv env;
   try {
      IloModel model(env);
      IloCplex cplex(env);

      if ( argc != 2 ) {
         usage (argv[0]);
         throw(-1);
      }

      IloObjective   obj;
      IloNumVarArray vars(env);
      IloRangeArray  rng(env);
      cplex.importModel(model, argv[1], obj, vars, rng);

      cplex.extract(model);

      if (cplex.getNcols() != cplex.getNbinVars()) {
         env.out() << "Problem contains non-binary variables, exiting" << endl;
         env.end();
         return 0;
      }

      // Now we get to setting up the callback.
      // We instantiate a HeuristicCallback and set the wherefrom parameter.
      HeuristicCallback cb(cplex, vars);
      CPXLONG wherefrom = 0;

      wherefrom |= IloCplex::Callback::Context::Id::Relaxation;

      // We add the callback.
      cplex.use(&cb, wherefrom);

      // Disable heuristics so that our callback has a chance to make a
      // difference.
      cplex.setParam(IloCplex::Param::MIP::Strategy::HeuristicFreq, -1);

      // Optimize the problem and obtain solution.
      if ( !cplex.solve() ) {
         cerr << "No solution found! Status = " << cplex.getStatus() << endl;
         throw(-1);
      }

      IloNumArray vals(env);
      cplex.getValues(vals, vars);
      env.out() << "Solution status = " << cplex.getStatus() << endl;
      env.out() << "Solution value  = " << cplex.getObjValue() << endl;
      env.out() << "Values          = " << vals << endl;

      env.out() << "roundDown was called " << cb.timesCalled
                << " times" << endl;

   }
   catch (IloException& e) {
      cerr << "Concert exception caught: " << e << endl;
      env.end();
      throw;
   }
   catch (...) {
      cerr << "Unknown exception caught" << endl;
      env.end();
      throw;
   }

   env.end();
   return 0;
}  // END main


static void usage (const char *progname)
{
   cerr << "Usage: " << progname << " filename" << endl;
   cerr << "   where filename is a file with extension " << endl;
   cerr << "      MPS, SAV, or LP (lower case is allowed)" << endl;
   cerr << " Exiting..." << endl;
} // END usage

