// -------------------------------------------------------------- -*- C++ -*-
// File: ilolpex6.cpp 
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
// ilolpex6.cpp - Illustrates that optimal basis can be copied and
//                used to start an optimization.

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN


static void
   populatebycolumn (IloModel model, IloNumVarArray var, IloRangeArray rng);

int
main (int argc, char **argv)
{
   IloEnv   env;
   try {
      IloModel model(env, "example");

      IloNumVarArray var(env);
      IloRangeArray  rng(env);
      populatebycolumn (model, var, rng);

      IloCplex cplex(model);

      IloCplex::BasisStatusArray cstat(env), rstat(env);
      cstat.add(IloCplex::AtUpper);
      cstat.add(IloCplex::Basic);
      cstat.add(IloCplex::Basic);
      rstat.add(IloCplex::AtLower);
      rstat.add(IloCplex::AtLower);
      cplex.setBasisStatuses(cstat, var, rstat, rng);
      cplex.solve();

      cplex.out() << "Solution status = " << cplex.getStatus() << endl;
      cplex.out() << "Solution value  = " << cplex.getObjValue() << endl;
      cplex.out() << "Iteration count = " << cplex.getNiterations() << endl;

      IloNumArray vals(env);
      cplex.getValues(vals, var);
      env.out() << "Values        = " << vals << endl;
      cplex.getSlacks(vals, rng);
      env.out() << "Slacks        = " << vals << endl;
      cplex.getDuals(vals, rng);
      env.out() << "Duals         = " << vals << endl;
      cplex.getReducedCosts(vals, var);
      env.out() << "Reduced Costs = " << vals << endl;

      cplex.exportModel("lpex6.lp");
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


static void
populatebycolumn (IloModel model, IloNumVarArray x, IloRangeArray c)
{
   IloEnv env = model.getEnv();

   IloObjective obj = IloMaximize(env);
   c.add(IloRange(env, -IloInfinity, 20.0));
   c.add(IloRange(env, -IloInfinity, 30.0));

   x.add(IloNumVar(obj(1.0) + c[0](-1.0) + c[1]( 1.0), 35.0, 40.0));
   x.add(obj(2.0) + c[0]( 1.0) + c[1](-3.0));
   x.add(obj(3.0) + c[0]( 1.0) + c[1]( 1.0));

   model.add(obj);
   model.add(c);
}  // END populatebycolumn
