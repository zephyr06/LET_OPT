// -------------------------------------------------------------- -*- C++ -*-
// File: ilolpex4.cpp
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
// ilolpex4.cpp - Illustrating the CPLEX callback functionality.
// 
// This is a modification of ilolpex1.c, where we use a callback
// function to print the iteration info, rather than have CPLEX
// do it.

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN


ILOSIMPLEXCALLBACK0(MyCallback) {
  cout << "Iteration " << getNiterations() << ": ";
  if ( isFeasible() ) {
     cout << "Objective = " << getObjValue() << endl;
  } else {
     cout << "Infeasibility measure = " << getInfeasibility() << endl;
  }
}


static void
   populatebycolumn (IloModel model, IloNumVarArray var, IloRangeArray rng);

int
main (int argc, char **argv)
{
   IloEnv env;
   try {
      IloModel model(env, "example");

      IloNumVarArray var(env);
      IloRangeArray  rng(env);
      populatebycolumn (model, var, rng);

      IloCplex cplex(model);
      cplex.setOut(env.getNullStream());
      cplex.setParam(IloCplex::Param::RootAlgorithm, IloCplex::Primal);
      cplex.use(MyCallback(env));
      cplex.solve();

      env.out() << "Solution status = " << cplex.getStatus() << endl;
      env.out() << "Solution value  = " << cplex.getObjValue() << endl;

      IloNumArray vals(env);
      cplex.getValues(vals, var);
      env.out() << "Values        = " << vals << endl;
      cplex.getSlacks(vals, rng);
      env.out() << "Slacks        = " << vals << endl;
      cplex.getDuals(vals, rng);
      env.out() << "Duals         = " << vals << endl;
      cplex.getReducedCosts(vals, var);
      env.out() << "Reduced Costs = " << vals << endl;

      cplex.exportModel("lpex4.lp");
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


// To populate by column, we first create the rows, and then add the
// columns.

static void
populatebycolumn (IloModel model, IloNumVarArray x, IloRangeArray c)
{
   IloEnv env = model.getEnv();

   IloObjective obj = IloMaximize(env);
   c.add(IloRange(env, -IloInfinity, 20.0));
   c.add(IloRange(env, -IloInfinity, 30.0));

   x.add(IloNumVar(obj(1.0) + c[0](-1.0) + c[1]( 1.0), 0.0, 40.0));
   x.add(obj(2.0) + c[0]( 1.0) + c[1](-3.0));
   x.add(obj(3.0) + c[0]( 1.0) + c[1]( 1.0));

   model.add(obj);
   model.add(c);
}  // END populatebycolumn
