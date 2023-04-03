// -------------------------------------------------------------- -*- C++ -*-
// File: inout3.cpp
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
// inout3.cpp -- A production planning problem


/* ------------------------------------------------------------

Problem Description
-------------------

A company has to produce 3 products, using 2 resources.
Each resource has a limited capacity.
Each product consumes a given number of machines.
Each product has a production cost (the inside cost).
Both products can also be purchased outside the company at a given
cost (the outside cost).

Minimize external production given product demand, a cost
constraint, and minimum internal production constraints.

------------------------------------------------------------ */


#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

const IloInt nbProds = 3;
const IloInt nbResources = 2;

IloNumArray consumption[nbResources];
IloNumArray demand;
IloNumArray capacity;
IloNumArray insideCost, outsideCost;

void setData(const IloEnv env) {
   consumption[0] = IloNumArray(env, nbProds, 0.5, 0.4, 0.3);
   consumption[1] = IloNumArray(env, nbProds, 0.2, 0.4, 0.6);
   capacity       = IloNumArray(env, nbResources, 20.0, 40.0);
   demand         = IloNumArray(env, nbProds, 100.0, 200.0, 300.0);
   insideCost     = IloNumArray(env, nbProds, 0.6, 0.8, 0.3);
   outsideCost    = IloNumArray(env, nbProds, 0.8, 0.9, 0.4);
}

void displayResults(IloCplex& cplex,
                    IloNumVar costVar,
                    IloNumVarArray inside,
                    IloNumVarArray outside) {
   cout << "cost: " << cplex.getValue(costVar) << endl;
   for(IloInt p = 0; p < nbProds; p++) {
      cout << "P" << p << endl;
      cout << "inside:  " << cplex.getValue(inside[p]) << endl;
      cout << "outside: " << cplex.getValue(outside[p]) << endl;
   }
}
 
int
main()
{
   IloEnv env;
   try {
      IloModel model(env);

      setData(env);
      IloNumVarArray inside(env, nbProds, 10.0, IloInfinity);
      IloNumVarArray outside(env, nbProds, 0.0,  IloInfinity);
      IloNumVar      costVar(env, 0.0, IloInfinity);  

      model.add(costVar ==   IloScalProd(insideCost, inside) 
                           + IloScalProd(outsideCost, outside));

      IloObjective fn = IloAdd(model, IloMinimize(env, costVar)); 

      // Must respect capacity constraint for each resource
      for(IloInt r = 0; r < nbResources; r++) {
         model.add(IloScalProd(consumption[r], inside) <= capacity[r]);
      }

      // Must meet demand for each product
      for(IloInt p = 0; p < nbProds; p++) {
         model.add(inside[p] + outside[p] == demand[p]); 
      }

      IloCplex cplex(env);
      cplex.extract(model);

      // Find minimum cost solution
      cplex.solve();

      if (cplex.getStatus() != IloAlgorithm::Optimal)
        cout << "No optimal solution" << endl;

      cout << "Solution status: " << cplex.getStatus() << endl;

      // New constraint: cost must be no more than 10% over minimum
      IloNum cost = cplex.getObjValue();
      costVar.setUB(1.1 * cost);

      // New objective: minimize outside production
      model.remove(fn);
      model.add(IloMinimize(env, IloSum(outside)));

      cplex.solve();
      
      cout << "Solution status: " << cplex.getStatus() << endl;

      displayResults(cplex, costVar, inside, outside);
      cout << "----------------------------------------" << endl;
   }
   catch (IloException& e) {
      cerr << "Error: " << e.getMessage() << endl;
   }
   catch (...) {
      cerr << "Error" << endl;
   }
   env.end();
   return 0;
}
  

