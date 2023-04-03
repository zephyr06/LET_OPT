// -------------------------------------------------------------- -*- C++ -*-
// File: rates.cpp
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
// rates.cpp -- modeling with semi-continuous variables
//
// Problem Description:
// 
// Assume you run a power supply company.  You have several power generators
// available, each of which has a minimum and maximum production level and a
// cost per unit output.  The question is which generators to use in order to
// minimize the overall operation cost while satisfying the demand.
// 
 
#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

int
main(int argc, char** argv)
{
   IloEnv env;
   try {
      IloNumArray minArray(env), maxArray(env), cost(env);
      IloNum demand;
      if ( argc < 2 ) {
         env.warning() 
           << "Default data file : ../../../examples/data/rates.dat" << endl;
         ifstream in("../../../examples/data/rates.dat");
         in >> minArray >> maxArray >> cost >> demand;
      } else {
         ifstream in(argv[1]);
         in >> minArray >> maxArray >> cost >> demand;
      }

      IloModel mdl(env);

      IloNumVarArray production(env);
      IloInt generators = minArray.getSize();
      for (IloInt j = 0; j < generators; j++) {
         production.add(IloSemiContVar(env, minArray[j], maxArray[j]));
      }

      mdl.add(IloMinimize(env, IloScalProd(cost, production)));
      mdl.add(IloSum(production) >= demand);

      IloCplex cplex(mdl);
      cplex.exportModel("rates.lp");
      if (cplex.solve()) {
         cplex.out() << "Solution status: " << cplex.getStatus() << endl;
         for (IloInt j = 0; j < generators; j++) {
            cplex.out() << "   generator " << j << ": "
                        << cplex.getValue(production[j]) << endl;
         }
         cplex.out() << "Total cost = " << cplex.getObjValue() << endl;
      }
      else cplex.out()<< "No solution" << endl;
      cplex.printTime();
   }
   catch (IloException& ex) {
      cerr << "Error: " << ex << endl;
   }
   catch (...) {
      cerr << "Error" << endl;
   }
   env.end();
   return 0;
}

/*
   generator 0: 15.6
   generator 1: 0
   generator 2: 0
   generator 3: 27.8
   generator 4: 27.8
   generator 5: 28.8
   generator 6: 29
   generator 7: 29
   generator 8: 29
Total cost = 1625.24
Elapsed time since last reset : 0.01
*/
