// -------------------------------------------------------------- -*- C++ -*-
// File: fixcost1.cpp
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
// fixcost1.cpp -- A production planning problem with fixed costs 

/* ------------------------------------------------------------

Problem Description
-------------------

A company must produce a product on a set of machines.
Each machine has limited capacity.
Producing a product on a machine has both a fixed cost
and a cost per unit of production.

Minimize the sum of fixed and variable costs so that the
company exactly meets demand.

------------------------------------------------------------ */


#include <ilcplex/ilocplex.h>
ILOSTLBEGIN


int
main(int, char**)
{
   IloEnv env;
   try {
      IloInt nbMachines = 6;
      IloNumArray cost     (env, nbMachines,
                            15.0, 20.0, 45.0, 64.0, 12.0, 56.0);
      IloNumArray capacity (env, nbMachines,
                            100.0, 20.0, 405.0, 264.0, 12.0, 256.0);
      IloNumArray fixedCost(env, nbMachines,
                            1900.0, 820.0, 805.0, 464.0, 3912.00, 556.0);
      IloNum demand = 22.0;

      IloModel model(env);
      IloNumVarArray x(env, nbMachines, 0, IloInfinity);
      IloNumVarArray fused(env, nbMachines, 0, 1, ILOINT);


      // Objective: minimize the sum of fixed and variable costs
      model.add(IloMinimize(env, IloScalProd(cost, x)
                               + IloScalProd(fused, fixedCost)));

      IloInt i;
      for(i =  0; i < nbMachines; i++) {
        // Constraint: respect capacity constraint on machine 'i'
        model.add(x[i] <= capacity[i]);

        // Constraint: only produce product on machine 'i' if it is 'used'
        //             (to capture fixed cost of using machine 'i')
        model.add(x[i] <= capacity[i]*fused[i]);
      }

      // Constraint: meet demand
      model.add(IloSum(x) == demand);

      IloCplex cplex(env);
      cplex.extract(model);
      cplex.solve();

      cout << "Solution status: " << cplex.getStatus() << endl;

      cout << "Obj " << cplex.getObjValue() << endl;
      IloNum eps = cplex.getParam(
         IloCplex::Param::MIP::Tolerances::Integrality);
      for(i = 0; i < nbMachines; i++) {
         if (cplex.getValue(fused[i]) > eps) {
            cout << "E" << i << " is used for ";
            cout << cplex.getValue(x[i]) << endl;
         }
      }
      cout << endl;
      cout << "----------------------------------------" << endl;
   }
   catch (IloException& ex) {
      cerr << "Error: " << ex << endl;
   }

   env.end();
   return 0;
}
  
/* Solution
Obj 1788
E5 is used for 22
*/
