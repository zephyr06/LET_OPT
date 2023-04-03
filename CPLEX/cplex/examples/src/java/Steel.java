/* --------------------------------------------------------------------------
 * File: Steel.java
 * Version 22.1.0  
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of IBM
 * 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
 * Copyright IBM Corporation 2001, 2022. All Rights Reserved.
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with
 * IBM Corp.
 * --------------------------------------------------------------------------
 * 
 * This example is an implementation of the model called "steelT.mod"
 * in the AMPL book by Fourer, Gay and Kernighan.  In the AMPL
 * example, a multiperiod production model is given, with data
 * for 4 weeks.  
 *
 * The parameters for the model are:
 *   nProd              the number of products
 *   nTime              the number of time periods
 *   rate[p]            rate of production for product p
 *   inv0[p]            initial inventoryfor product p
 *   avail[t]           hours available in time period t
 *   market[p][t]       demand for product p in time period t
 *   prodcost[p]        production cost per unit of product p 
 *   invcost[p]         inventory cost per unit of product p
 *   revenue[p][t]      revenue per unit of product p in time period t
 *
 * The decision variables of the model are:
 *   Make[p][t]         amount produced of product p in time period t
 *   Inv[p][t]          amount inventoried of product p in time period t
 *   Sell[p][t]         amount sold of product p in time period t
 * 
 * The objective function is to
 * 
 * maximize  sum(over p,t) (  revenue[p][t] * Sell[p][t]
 *                          - prodcost[p]   * Make[p][t]
 *                          - invcost[p]    * Inv[p][t]  )
 *
 * The constraints are
 * 
 *  For each t:   (time availability constraint)
 *      sum(over p)  ( (1/rate[p]) * Make[p][t] ) <= avail[t]
 * 
 *  For each pair (p,t) (t>0): (balance constraint)
 *      Make[p][t] + Inv[p][t-1] - Sell[p][t] - Inv[p][t] = 0
 *
 *  For each p, (t=0): (balance constraint)
 *      Make[p][0] - Sell[p][0] - Inv[p][0] = -inv0[p]
 *
 *  The bounds on the variables are:
 *    All variables are nonnegative ( >= 0 )
 *    For each (p,t),
 *       Sell[p][t] <= market[p][t]
 *    All other variables have infinite upper bounds.
 */

import ilog.concert.*;
import ilog.cplex.*;

public class Steel {
   static int _nProd;
   static int _nTime;
   
   static double[] _avail;
   static double[] _rate;
   static double[] _inv0;
   static double[] _prodCost;
   static double[] _invCost;
   
   static double[][] _revenue;
   static double[][] _market;

   static void readData(String fileName)
                         throws java.io.IOException,
                                InputDataReader.InputDataReaderException {
      InputDataReader reader = new InputDataReader(fileName);
      try {
	  _avail    = reader.readDoubleArray();
	  _rate     = reader.readDoubleArray();
	  _inv0     = reader.readDoubleArray();
	  _prodCost = reader.readDoubleArray();
	  _invCost  = reader.readDoubleArray();
	  _revenue  = reader.readDoubleArrayArray();
	  _market   = reader.readDoubleArrayArray();
	  
	  _nProd = _rate.length;
	  _nTime = _avail.length;
      }
      finally {
	  reader.close();
      }
   }
   
   public static void main(String[] args) {

      String filename = "../../../examples/data/steel.dat";
      if ( args.length > 0 )
         filename = args[0];

      try {
         readData(filename);
      }
      catch (java.io.IOException|
             InputDataReader.InputDataReaderException ex) {
         System.out.println("Data Error: " + ex);
         System.exit(-1);
      }

      try (IloCplex cplex = new IloCplex()) {

         // VARIABLES
         IloNumVar[][] Make = new IloNumVar[_nProd][];
         for (int p = 0; p < _nProd; p++) {
            Make[p] = cplex.numVarArray(_nTime, 0.0, Double.MAX_VALUE);
         }
       
         IloNumVar[][] Inv = new IloNumVar[_nProd][];
         for (int p = 0; p < _nProd; p++) {
            Inv[p] = cplex.numVarArray(_nTime, 0.0, Double.MAX_VALUE);
         }
       
         IloNumVar[][] Sell = new IloNumVar[_nProd][_nTime];
         for (int p = 0; p < _nProd; p++) {
            for (int t = 0; t < _nTime; t++) {
               Sell[p][t] = cplex.numVar(0.0, _market[p][t]);
            }
         }
       
         // OBJECTIVE
         IloLinearNumExpr TotalRevenue  = cplex.linearNumExpr();
         IloLinearNumExpr TotalProdCost = cplex.linearNumExpr();
         IloLinearNumExpr TotalInvCost  = cplex.linearNumExpr();
         
         for (int p = 0; p < _nProd; p++) {
            for (int t = 1; t < _nTime; t++) {
               TotalRevenue.addTerm (_revenue[p][t], Sell[p][t]);
               TotalProdCost.addTerm(_prodCost[p], Make[p][t]);
               TotalInvCost.addTerm (_invCost[p], Inv[p][t]);
            }
         }
           
         cplex.addMaximize(cplex.diff(TotalRevenue, 
                                      cplex.sum(TotalProdCost, TotalInvCost)));
       
         // TIME AVAILABILITY CONSTRAINTS
       
         for (int t = 0; t < _nTime; t++) {
            IloLinearNumExpr availExpr = cplex.linearNumExpr();
            for (int p = 0; p < _nProd; p++) {
               availExpr.addTerm(1./_rate[p], Make[p][t]);
            }
            cplex.addLe(availExpr, _avail[t]);
         }
       
         // MATERIAL BALANCE CONSTRAINTS
       
         for (int p = 0; p < _nProd; p++) {
            cplex.addEq(cplex.sum(Make[p][0], _inv0[p]), 
                        cplex.sum(Sell[p][0], Inv[p][0]));
            for (int t = 1; t < _nTime; t++) {
               cplex.addEq(cplex.sum(Make[p][t], Inv[p][t-1]), 
                           cplex.sum(Sell[p][t], Inv[p][t]));
            }
         }
       
         cplex.exportModel("steel.lp");
       
         if ( cplex.solve() ) {
            System.out.println("Solution status: " + cplex.getStatus());
            System.out.println();
            System.out.println("Total Profit = " + cplex.getObjValue());
          
            System.out.println();
            System.out.println("\tp\tt\tMake\tInv\tSell");
          
            for (int p = 0; p < _nProd; p++) {
               for (int t = 0; t < _nTime; t++) {
                  System.out.println("\t" + p +
                                     "\t" + t +
                                     "\t" + cplex.getValue(Make[p][t]) +
                                     "\t" + cplex.getValue(Inv[p][t]) +
                                     "\t" + cplex.getValue(Sell[p][t]));
               }
            }
         }
      }
      catch (IloException exc) {
         System.err.println("Concert exception '" + exc + "' caught");
         System.exit(-1);
      }
   }
}

/*
Total Profit = 515033.00000000006

        p       t       Make    Inv     Sell
        0       0       0.0     10.0    0.0
        0       1       5990.0  0.0     6000.0
        0       2       6000.0  0.0     6000.0
        0       3       1400.0  0.0     1400.0
        0       4       2000.0  0.0     2000.0
        1       0       0.0     0.0     0.0
        1       1       1407.0  1100.0  307.0
        1       2       1400.0  0.0     2500.0
        1       3       3500.0  0.0     3500.0
        1       4       4200.0  0.0     4200.0
*/
