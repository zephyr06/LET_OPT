// -------------------------------------------------------------- -*- C++ -*-
// File: ilogoalex1.cpp
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
// ilogoalex1.cpp - A simple goal example.
//
//                  Create a branch goal that chooses the variable
//                  with the largest objective from amongst those
//                  with the largest integer infeasibility.
//

#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

static void usage (const char *progname);



// Branch on var with largest objective coefficient
// among those with largest infeasibility

ILOCPLEXGOAL1(MyBranchGoal, IloNumVarArray, vars) {
   IloNumArray x;
   IloNumArray obj;
   IntegerFeasibilityArray feas;

   x    = IloNumArray(getEnv());
   obj  = IloNumArray(getEnv());
   feas = IntegerFeasibilityArray(getEnv());
   getValues(x, vars);
   getObjCoefs(obj, vars);
   getFeasibilities(feas, vars);

   IloInt bestj  = -1;
   IloNum maxinf = 0.0;
   IloNum maxobj = 0.0;
   IloInt cols = vars.getSize();
   for (IloInt j = 0; j < cols; j++) {
      if ( feas[j] == Infeasible ) {
         IloNum xj_inf = x[j] - IloFloor (x[j]);
         if ( xj_inf > 0.5 )
            xj_inf = 1.0 - xj_inf;
         if ( xj_inf >= maxinf                             &&
             (xj_inf > maxinf || IloAbs (obj[j]) >= maxobj)  ) {
            bestj  = j;
            maxinf = xj_inf;
            maxobj = IloAbs (obj[j]);
         }
      }
   }

   IloCplex::Goal res;
   if ( bestj >= 0 ) {
      res = AndGoal(OrGoal(vars[bestj] >= IloFloor(x[bestj])+1,
                           vars[bestj] <= IloFloor(x[bestj])),
                    this);
   }

   x.end();
   obj.end();
   feas.end();

   return res;
}


int
main (int argc, char **argv)
{
   IloEnv   env;
   try {
      IloModel model(env);
      IloCplex cplex(env);

      if ( argc != 2 ) {
         usage (argv[0]);
         throw(-1);
      }

      IloObjective   obj;
      IloNumVarArray var(env);
      IloRangeArray  rng(env);
      cplex.importModel(model, argv[1], obj, var, rng);

      cplex.extract(model);
      cplex.setParam(IloCplex::Param::MIP::Strategy::Search,
                     IloCplex::Traditional);
      cplex.solve(MyBranchGoal(env, var));

      IloNumArray vals(env);
      cplex.getValues(vals, var);
      cout << "Solution status = " << cplex.getStatus() << endl;
      cout << "Solution value  = " << cplex.getObjValue() << endl;
      cout << "Values          = " << vals << endl;
   }
   catch (IloException& e) {
      cerr << "Concert exception caught: " << e << endl;
   }

   env.end();

   return 0;
}


static void usage (const char *progname)
{
   cerr << "Usage: " << progname << " filename" << endl;
   cerr << "   where filename is a file with extension " << endl;
   cerr << "      MPS, SAV, or LP (lower case is allowed)" << endl;
   cerr << " Exiting..." << endl;
}

