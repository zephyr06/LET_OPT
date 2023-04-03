// -------------------------------------------------------------- -*- C++ -*-
// File: ilomipex3.cpp 
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
// ilomipex3.cpp - Entering and optimizing a MIP problem with SOS sets
//                 and priority orders.  Is a modification of ilomipex1.cpp.
//                 Note that the problem solved is slightly different than
//                 ilomipex1.cpp so that the output is interesting.

#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

static void
   populatebyrow(IloModel model, IloNumVarArray var, IloRangeArray con);

int
main (void) {
   IloEnv env;
   try {
      IloModel model(env);

      IloNumVarArray var(env);
      IloRangeArray con(env);
      populatebyrow (model, var, con);

      IloCplex cplex(model);
      IloNumVarArray ordvar(env, 2);
      IloNumArray    ordpri(env, 2);
      ordvar[0] = var[1]; ordvar[1] = var[3];
      ordpri[0] = 8.0;    ordpri[1] = 7.0;
      cplex.setPriorities (ordvar, ordpri);
      cplex.setDirection(var[1], IloCplex::BranchUp);
      cplex.setDirection(var[3], IloCplex::BranchDown);
      cplex.solve();

      env.out() << "Solution status = " << cplex.getStatus() << endl;
      env.out() << "Solution value  = " << cplex.getObjValue() << endl;

      IloNumArray vals(env);
      cplex.getValues(vals, var);
      env.out() << "Values        = " << vals << endl;
      cplex.getSlacks(vals, con);
      env.out() << "Slacks        = " << vals << endl;

      cplex.exportModel("mipex3.lp");
      cplex.writeOrder("mipex3.ord");
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
populatebyrow (IloModel model, IloNumVarArray x, IloRangeArray c)
{
   IloEnv env = model.getEnv();

   x.add(IloNumVar(env, 0.0, 40.0));
   x.add(IloNumVar(env, 0.0, IloInfinity, ILOINT));
   x.add(IloNumVar(env, 0.0, IloInfinity, ILOINT));
   x.add(IloNumVar(env, 2.0, 3.0, ILOINT));
   model.add(IloMaximize(env, x[0] + 2 * x[1] + 3 * x[2] + x[3]));

   c.add( - x[0] +     x[1] + x[2] + 10 * x[3] <= 20);
   c.add(   x[0] - 3 * x[1] + x[2]             <= 30);
   c.add(              x[1]        - 3.5* x[3] == 0);
   model.add(c);

   IloNumVarArray sosvar(env, 2);
   IloNumArray    sosval(env, 2);
   sosvar[0] = x[2]; sosvar[1] = x[3];
   sosval[0] = 25.0; sosval[1] = 18.0;

   model.add(IloSOS1(model.getEnv(), sosvar, sosval));

}  // END populatebyrow
