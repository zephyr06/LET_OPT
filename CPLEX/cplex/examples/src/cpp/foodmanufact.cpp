// -------------------------------------------------------------- -*- C++ -*-
// File: foodmanufact.cpp
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
// foodmanufact.cpp -  An implementation of an example from H.P.
//                     Williams' book Model Building in Mathematical
//                     Programming.  This example solves a
//                     food production planning problem.  It
//                     demonstrates the use of CPLEX's
//                     linearization capability.

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray<IloNumArray>    NumMatrix;

typedef enum { v1, v2, o1, o2, o3 } Product;
const IloInt nbMonths   = 6;
const IloInt nbProducts = 5;

int
main()
{
   IloEnv env;
   try {
      NumMatrix cost(env, nbMonths);
      cost[0]=IloNumArray(env, nbProducts, 110.0, 120.0, 130.0, 110.0, 115.0);
      cost[1]=IloNumArray(env, nbProducts, 130.0, 130.0, 110.0,  90.0, 115.0);
      cost[2]=IloNumArray(env, nbProducts, 110.0, 140.0, 130.0, 100.0,  95.0);
      cost[3]=IloNumArray(env, nbProducts, 120.0, 110.0, 120.0, 120.0, 125.0);
      cost[4]=IloNumArray(env, nbProducts, 100.0, 120.0, 150.0, 110.0, 105.0);
      cost[5]=IloNumArray(env, nbProducts,  90.0, 100.0, 140.0,  80.0, 135.0);

      // Variable definitions
      IloNumVarArray produce(env, nbMonths, 0, IloInfinity);
      NumVarMatrix   use(env, nbMonths);
      NumVarMatrix   buy(env, nbMonths);
      NumVarMatrix   store(env, nbMonths);
      IloInt i, p;
      for (i = 0; i < nbMonths; i++) {
         use[i]   = IloNumVarArray(env, nbProducts, 0, IloInfinity);
         buy[i]   = IloNumVarArray(env, nbProducts, 0, IloInfinity);
         store[i] = IloNumVarArray(env, nbProducts, 0, 1000);
      } 
      IloExpr profit(env);

      IloModel model(env);

      // For each type of raw oil we must have 500 tons at the end
      for (p = 0; p < nbProducts; p++) {
        store[nbMonths-1][p].setBounds(500, 500);
      }

      // Constraints on each month 
      for (i = 0; i < nbMonths; i++) {
         // Not more than 200 tons of vegetable oil can be refined
         model.add(use[i][v1] + use[i][v2] <= 200); 

         // Not more than 250 tons of non-vegetable oil can be refined
         model.add(use[i][o1] + use[i][o2] + use[i][o3] <= 250); 

         // Constraints on food composition 
         model.add(3 * produce[i] <=
                   8.8 * use[i][v1] + 6.1 * use[i][v2] +
                   2   * use[i][o1] + 4.2 * use[i][o2] + 5 * use[i][o3]);
         model.add(8.8 * use[i][v1] + 6.1 * use[i][v2] +
                   2   * use[i][o1] + 4.2 * use[i][o2] + 5 * use[i][o3]
                   <= 6 * produce[i]);
         model.add(produce[i] == IloSum(use[i]));

         // Raw oil can be stored for later use
         if (i == 0) {
            for (IloInt p = 0; p < nbProducts; p++)
               model.add(500 + buy[i][p] == use[i][p] + store[i][p]);
         }
         else {
            for (IloInt p = 0; p < nbProducts; p++)
              model.add(store[i-1][p] + buy[i][p] == use[i][p] + store[i][p]);
         }

         // Logical constraints
         // The food cannot use more than 3 oils 
         // (or at least two oils must not be used)
         model.add((use[i][v1] == 0) + (use[i][v2] == 0) + (use[i][o1] == 0) +
                   (use[i][o2] == 0) + (use[i][o3] == 0) >= 2);

         // When an oil is used, the quantity must be at least 20 tons
         for (p = 0; p < nbProducts; p++)
            model.add((use[i][p] == 0) || (use[i][p] >= 20));

         // If products v1 or v2 are used, then product o3 is also used
         model.add(IloIfThen(env, (use[i][v1] >= 20) || (use[i][v2] >= 20),
           use[i][o3] >= 20));

         // Objective function
         profit += 150 * produce[i] - IloScalProd(cost[i], buy[i]) -
                   5 * IloSum(store[i]);
      }

      // Objective function
      model.add(IloMaximize(env, profit));

      IloCplex cplex(model);

      if (cplex.solve()) {
         cout << "Solution status: " << cplex.getStatus() << endl;
         cout << " Maximum profit = " << cplex.getObjValue() << endl;
         for (IloInt i = 0; i < nbMonths; i++) {
            IloInt p;
            cout << " Month " << i << " " << endl;
            cout << "  . buy   ";
            for (p = 0; p < nbProducts; p++) {
               cout << cplex.getValue(buy[i][p]) << "\t ";
            }
            cout << endl;
            cout << "  . use   ";
            for (p = 0; p < nbProducts; p++) {
               cout << cplex.getValue(use[i][p]) << "\t ";
            }
            cout << endl;
            cout << "  . store ";
            for (p = 0; p < nbProducts; p++) {
               cout << cplex.getValue(store[i][p]) << "\t ";
            }
            cout << endl;
         }
      }
      else {
         cout << " No solution found" << endl;
      }
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
