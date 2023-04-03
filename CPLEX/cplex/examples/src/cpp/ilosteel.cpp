// -------------------------------------------------------------- -*- C++ -*-
// File: steel.cpp
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
// This example is an implementation of the model called "steelT.mod"
// in the AMPL book by Fourer, Gay and Kernighan.  In the AMPL
// example, a multiperiod production model is given, with data
// for 4 weeks.  
//
// The parameters for the model are:
//   nProd              the number of products
//   nTime              the number of time periods
//   rate[p]            rate of production for product p
//   inv0[p]            initial inventoryfor product p
//   avail[t]           hours available in time period t
//   market[p][t]       demand for product p in time period t
//   prodcost[p]        production cost per unit of product p 
//   invcost[p]         inventory cost per unit of product p
//   revenue[p][t]      revenue per unit of product p in time period t
//
// The decision variables of the model are:
//   Make[p][t]         amount produced of product p in time period t
//   Inv[p][t]          amount inventoried of product p in time period t
//   Sell[p][t]         amount sold of product p in time period t
// 
// The objective function is to
// 
// maximize  sum(over p,t) (  revenue[p][t] * Sell[p][t]
//                          - prodcost[p]   * Make[p][t]
//                          - invcost[p]    * Inv[p][t]  )
//
// The constraints are
// 
//  For each t:   (time availability constraint)
//      sum(over p)  ( (1/rate[p]) * Make[p][t] ) <= avail[t]
// 
//  For each pair (p,t) (t>0): (balance constraint)
//      Make[p][t] + Inv[p][t-1] - Sell[p][t] - Inv[p][t] = 0
//
//  For each p, (t=0): (balance constraint)
//      Make[p][0] - Sell[p][0] - Inv[p][0] = -inv0[p]
//
//  The bounds on the variables are:
//    All variables are nonnegative ( >= 0 )
//    For each (p,t),
//       Sell[p][t] <= market[p][t]
//    All other variables have infinite upper bounds.


#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

typedef IloArray<IloNumVarArray>     IloNumVarArray2;

int
main(int argc, char **argv)
{
   IloEnv env;

   try {
      const char* filename = "../../../examples/data/steel.dat";
      if (argc >= 2) filename = argv[1];
      ifstream file(filename);
      if ( !file ) {
         cerr << "No such file: " << filename << endl;
         throw(-1);
      }

      IloNumArray avail(env), rate(env), inv0(env), prodCost(env), invCost(env);
      IloNumArray2 revenue(env), market(env);
      file >> avail >> rate >> inv0 >> prodCost >> invCost >> revenue >> market;

      IloInt nProd = rate.getSize();
      IloInt nTime = avail.getSize();

      IloInt p, t;
      IloModel mod(env);

      // VARIABLES

      IloNumVarArray2 Make(env);
      for (p = 0; p < nProd; p++) {
         Make.add(IloNumVarArray(env, nTime, 0.0, IloInfinity));
      }

      IloNumVarArray2 Inv(env);
      for (p = 0; p < nProd; p++) {
         Inv.add(IloNumVarArray(env, nTime, 0.0, IloInfinity));
      }

      IloNumVarArray2 Sell(env);
      for (p = 0; p < nProd; p++) {
         Sell.add(IloNumVarArray(env, 0.0, market[p]));
      }

      // OBJECTIVE

      IloExpr TotalRevenue(env), TotalProdCost(env), TotalInvCost(env);
      for (p = 0; p < nProd; p++) {
         for (t = 1; t < nTime; t++) {
            TotalRevenue  += revenue[p][t] * Sell[p][t];
            TotalProdCost += prodCost[p] * Make[p][t];
            TotalInvCost  += invCost[p] * Inv[p][t];
         }
      }
      mod.add(IloMaximize(env, TotalRevenue - TotalProdCost - TotalInvCost));
      TotalRevenue.end();
      TotalProdCost.end();
      TotalInvCost.end();

      // TIME AVAILABILITY CONSTRAINTS

      for (t = 0; t < nTime; t++) {
         IloExpr availExpr(env);
         for (p = 0; p < nProd; p++) {
            availExpr += (1/rate[p]) * Make[p][t];
         }
         mod.add(availExpr <= avail[t]);
         availExpr.end();
      }

      // MATERIAL BALANCE CONSTRAINTS

      for (p = 0; p < nProd; p++) {
         mod.add(Make[p][0] + inv0[p] == Sell[p][0] + Inv[p][0]);
         for (t = 1; t < nTime; t++) {
            mod.add(Make[p][t] + Inv[p][t-1] == Sell[p][t] + Inv[p][t]);
         }
      }

      IloCplex cplex(mod);
      cplex.exportModel("steel.lp");

      cplex.solve();
      env.out() << "Solution status: " << cplex.getStatus() << endl;
      env.out() << endl << "Total Profit = " << cplex.getObjValue() << endl;
      env.out() << endl << "\tp\tt\tMake\tInv\tSell" << endl;

      for (p = 0; p < nProd; p++) {
         for (t = 0; t < nTime; t++) {
            env.out() << '\t' << p
                      << '\t' << t
                      << '\t' << cplex.getValue(Make[p][t]) 
                      << '\t' << cplex.getValue(Inv[p][t])
                      << '\t' << cplex.getValue(Sell[p][t]) << endl;
         }
      }
   }
   catch (IloException& ex) {
      cerr << "Error: " << ex << endl;
   }
   catch (...) {
      cerr << "Error: Unknown exception caught!" << endl;
   }

   env.end();

   return 0;
} // END main
