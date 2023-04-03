// -------------------------------------------------------------- -*- C++ -*-
// File: warehouse.cpp
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
// warehouse.cpp - Example that uses the goal API to enforce constraints
//                 dynamically, depending on the relaxation solution at
//                 each MIP node.
//
//                 Given a set of warehouses that each have a
//                 capacity, a cost per unit stored, and a 
//                 minimum usage level, this example find an
//                 assignment of items to warehouses that minimizes
//                 total cost.  The minimum usage levels are
//                 enforced dynamically using the goal API.

#include <ilcplex/ilocplex.h>

ILOSTLBEGIN


typedef IloArray<IloNumVarArray> IloNumVarArrayArray;

ILOCPLEXGOAL2(SemiContGoal,
              IloNumVarArray, scVars,
              IloNumArray,    scLbs) {
   IloInt besti = -1;
   IloNum maxObjCoef = -IloInfinity;
   IloInt i;

   // From among all variables that do not respect their minimum 
   // usage levels, select the one with maximum objective coefficient.

   for (i = 0; i < scVars.getSize(); i++) {
      IloNum val = getValue(scVars[i]);
      if ( val >= 1e-5           &&
           val <= scLbs[i] - 1e-5  ) {
         if ( getObjCoef(scVars[i]) >= maxObjCoef ) {
            besti = i;
            maxObjCoef = getObjCoef(scVars[i]);
         }
      }
   }

   //  If any are found, branch to enforce the condition that
   //  the variable must either be 0.0 or greater than
   //  the minimum usage level.
   if ( besti != -1 ) {
      return AndGoal ( OrGoal ( scVars[besti] <= 0.0,
                                scVars[besti] >= scLbs[besti] ),
                       this );
   }
   else if ( !isIntegerFeasible() ) {
      return AndGoal( BranchAsCplexGoal(getEnv()), this );
   }

   return 0;
}

int
main (void)
{
   IloEnv   env;
   try {
      IloModel model(env);
      IloCplex cplex(env);

      IloInt nbWhouses = 4;
      IloInt nbLoads = 31;
      IloInt w, l;

      IloNumVarArray capVars(env, nbWhouses, 0, 10, ILOINT); // Used capacities
      IloNumArray    capLbs (env, nbWhouses, 2, 3, 5, 7); // Minimum usage level
      IloNumArray    costs  (env, nbWhouses, 1, 2, 4, 6); // Cost per warehouse

      // These variables represent the assigninment of a
      // load to a warehouse.
      IloNumVarArrayArray assignVars(env, nbWhouses); 
      for (w = 0; w < nbWhouses; w++) {
         assignVars[w] = IloNumVarArray(env, nbLoads, 0, 1, ILOINT);

         // Links the number of loads assigned to a warehouse with 
         // the capacity variable of the warehouse.
         model.add(IloSum(assignVars[w]) == capVars[w]);
      }

      // Each load must be assigned to just one warehouse.
      for (l = 0; l < nbLoads; l++) {
         IloNumVarArray aux(env);
         for (w = 0; w < nbWhouses; w++)
            aux.add(assignVars[w][l]);
         model.add(IloSum(aux) == 1);
         aux.end();
      }

      model.add (IloMinimize(env, IloScalProd(costs, capVars)));

      cplex.extract(model);
      cplex.setParam(IloCplex::Param::MIP::Strategy::Search, IloCplex::Traditional);
      if ( cplex.solve(SemiContGoal(env, capVars, capLbs)) ) {
         cout << " --------------------------------------------------" << endl;
         cout << "Solution status: " << cplex.getStatus() << endl;
         cout << endl << "Solution found:" << endl;
         cout << " Objective value = " 
              << cplex.getObjValue() << endl << endl;
         for (w = 0; w < nbWhouses; w++) {
            cout << "Warehouse " << w << ": stored " 
                 << cplex.getValue(capVars[w]) << " loads" << endl;
            for (l = 0; l < nbLoads; l++) {
               if ( cplex.getValue(assignVars[w][l]) > 1e-5 )
                  cout << "Load " << l << " | ";
            }
            cout << endl << endl;
         }
         cout << " --------------------------------------------------" << endl;
      } 
      else {
         cout << " No solution found " << endl;
      }

   }
   catch (IloException& e) {
      cerr << "Concert exception caught: " << e << endl;
   }

   env.end();

   return 0;
}  // END main
