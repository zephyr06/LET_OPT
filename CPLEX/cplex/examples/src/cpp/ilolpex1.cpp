// -------------------------------------------------------------- -*- C++ -*-
// File: ilolpex1.cpp
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
// ilolpex1.cpp - Entering and optimizing a problem.  Demonstrates different
// methods for creating a problem.  The user has to choose the method
// on the command line:
//
//    ilolpex1  -r     generates the problem by adding rows
//    ilolpex1  -c     generates the problem by adding columns
//    ilolpex1  -n     generates the problem by adding a list of coefficients

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

static void
   usage (const char *progname),
   populatebyrow     (IloModel model, IloNumVarArray var, IloRangeArray con),
   populatebycolumn  (IloModel model, IloNumVarArray var, IloRangeArray con),
   populatebynonzero (IloModel model, IloNumVarArray var, IloRangeArray con);

int
main (int argc, char **argv)
{
   IloEnv   env;
   try {
      IloModel model(env);

      if (( argc != 2 )                         ||
          ( argv[1][0] != '-' )                 ||
          ( strchr ("rcn", argv[1][1]) == NULL )   ) {
         usage (argv[0]);
         throw(-1);
      }

      IloNumVarArray var(env);
      IloRangeArray con(env);

      switch (argv[1][1]) {
         case 'r':
            populatebyrow (model, var, con);
            break;
         case 'c':
            populatebycolumn (model, var, con);
            break;
         case 'n':
            populatebynonzero (model, var, con);
            break;
      }

      IloCplex cplex(model);
      cplex.exportModel("lpex1.lp");

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


static void usage (const char *progname)
{
   cerr << "Usage: " << progname << " -X" << endl;
   cerr << "   where X is one of the following options:" << endl;
   cerr << "      r          generate problem by row" << endl;
   cerr << "      c          generate problem by column" << endl;
   cerr << "      n          generate problem by nonzero" << endl;
   cerr << " Exiting..." << endl;
} // END usage


// To populate by row, we first create the variables, and then use them to
// create the range constraints and objective.

static void
populatebyrow (IloModel model, IloNumVarArray x, IloRangeArray c)
{
   IloEnv env = model.getEnv();

   x.add(IloNumVar(env, 0.0, 40.0));
   x.add(IloNumVar(env));
   x.add(IloNumVar(env));

   model.add(IloMaximize(env, x[0] + 2 * x[1] + 3 * x[2]));

   c.add( - x[0] +     x[1] + x[2] <= 20);
   c.add(   x[0] - 3 * x[1] + x[2] <= 30);

   x[0].setName("x1");
   x[1].setName("x2");
   x[2].setName("x3");

   c[0].setName("c1");
   c[1].setName("c2");
   model.add(c);

}  // END populatebyrow


// To populate by column, we first create the range constraints and the
// objective, and then create the variables and add them to the ranges and
// objective using column expressions.

static void
populatebycolumn (IloModel model, IloNumVarArray x, IloRangeArray c)
{
   IloEnv env = model.getEnv();

   IloObjective obj = IloMaximize(env);
   c.add(IloRange(env, -IloInfinity, 20.0, "c1"));
   c.add(IloRange(env, -IloInfinity, 30.0, "c2"));

   x.add(IloNumVar(obj(1.0) + c[0](-1.0) + c[1]( 1.0), 0.0, 40.0));
   x.add(IloNumVar(obj(2.0) + c[0]( 1.0) + c[1](-3.0)));
   x.add(IloNumVar(obj(3.0) + c[0]( 1.0) + c[1]( 1.0)));

   x[0].setName("x1");
   x[1].setName("x2");
   x[2].setName("x3");

   model.add(obj);
   model.add(c);

}  // END populatebycolumn


// To populate by nonzero, we first create the rows, then create the
// columns, and then change the nonzeros of the matrix 1 at a time.

static void
populatebynonzero (IloModel model, IloNumVarArray x, IloRangeArray c)
{
   IloEnv env = model.getEnv();

   IloObjective obj = IloMaximize(env);
   c.add(IloRange(env, -IloInfinity, 20.0));
   c.add(IloRange(env, -IloInfinity, 30.0));

   x.add(IloNumVar(env, 0.0, 40.0));
   x.add(IloNumVar(env));
   x.add(IloNumVar(env));

   obj.setLinearCoef(x[0], 1.0);
   obj.setLinearCoef(x[1], 2.0);
   obj.setLinearCoef(x[2], 3.0);

   c[0].setLinearCoef(x[0], -1.0);
   c[0].setLinearCoef(x[1],  1.0);
   c[0].setLinearCoef(x[2],  1.0);
   c[1].setLinearCoef(x[0],  1.0);
   c[1].setLinearCoef(x[1], -3.0);
   c[1].setLinearCoef(x[2],  1.0);

   c[0].setName("c1");
   c[1].setName("c2");

   x[0].setName("x1");
   x[1].setName("x2");
   x[2].setName("x3");

   model.add(obj);
   model.add(c);
}  // END populatebynonzero
