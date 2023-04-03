// -------------------------------------------------------------- -*- C++ -*-
// File: iloindefqpex1.cpp
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
// iloindefqpex1.cpp - Entering and optimizing an indefinite quadratic
// problem.

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

static void
   populatebyrow     (IloModel model, IloNumVarArray var, IloRangeArray con);
static void
   solveanddisplay   (IloEnv env, IloCplex cplex, IloNumVarArray var,
                      IloRangeArray con);

int
main (int argc, char **argv)
{
   IloEnv   env;
   try {
      IloModel model(env);
      IloNumVarArray var(env);
      IloRangeArray con(env);
      IloRange add_con(env, 0.0, IloInfinity);

      populatebyrow (model, var, con);

      IloCplex cplex(model);

      // When a non-convex objective function is present, CPLEX will
      // raise an exception unless the parameter
      // IloCplex::Param::OptimalityTarget is set to accept first-order optimal
      // solutions
      cplex.setParam(IloCplex::Param::OptimalityTarget,
                     IloCplex::OptimalityFirstOrder);

      // CPLEX may converge to either local optimum 
      solveanddisplay(env, cplex, var, con);

      // Add a constraint that cuts off the solution at (-1, 1)
      add_con.setExpr(var[0]);
      model.add(add_con);
      solveanddisplay(env, cplex, var, con);

      // Change the bounds of the newly added constraint to cut off
      // the solution at (1, 1)
      add_con.setBounds(-IloInfinity, 0.0);
      solveanddisplay(env, cplex, var, con);

      cplex.exportModel("indefqpex1.lp");
      
   }
   catch (IloException& e) {
      cerr << "Concert exception caught: " << e << endl;
   }
   catch (...) {
      cerr << "Unknown exception caught" << endl;
   }

   env.end();

   return 0;
}  // END main


// To populate by row, we first create the variables, and then use them to
// create the range constraints and objective.  The model we create is:
//
//    Minimize
//     obj:   - 0.5 (-3 * xˆ2 - 3 * yˆ2 - 1 * x * y)
//
//    Subject To
//     c1: -x + y >= 0
//     c2:  x + y >= 0
//    Bounds
//     -1 <= x <= 1
//      0 <= y <= 1
//    End

static void
populatebyrow (IloModel model, IloNumVarArray x, IloRangeArray c)
{
   IloEnv env = model.getEnv();

   x.add(IloNumVar(env, -1.0, 1.0));
   x.add(IloNumVar(env,  0.0, 1.0));
   model.add(IloMinimize(env, 0.5 * (-3*x[0]*x[0] - 3*x[1]*x[1] +
                                       - 1*x[0]*x[1]               ) ));

   c.add( - x[0] + x[1] >= 0);
   c.add(   x[0] + x[1] >= 0);
   model.add(c);
}  // END populatebyrow

static void
solveanddisplay   (IloEnv env, IloCplex cplex, IloNumVarArray var,
                   IloRangeArray con)
{
      // Optimize the problem and obtain solution.
      if ( !cplex.solve() ) {
         env.error() << "Failed to optimize LP" << endl;
         throw(-1);
      }

      IloNumArray vals(env);
      env.out() << "Solution status = " << cplex.getStatus() << endl;
      env.out() << "Solution value  = " << cplex.getObjValue() << endl;
      cplex.getValues(vals, var);
      env.out() << "Values        = " << vals << endl;
      cplex.getSlacks(vals, con);
      env.out() << "Slacks        = " << vals << endl;
      cplex.getDuals(vals, con);
      env.out() << "Duals         = " << vals << endl;
      cplex.getReducedCosts(vals, var);
      env.out() << "Reduced Costs = " << vals << endl;

}  // END solveanddisplay


