/* --------------------------------------------------------------------------
 * File: FoodManufact.java
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
 * FoodManufact.java - An implementation of an example from H.P.
 *                     Williams' book Model Building in Mathematical
 *                     Programming.  This example solves a
 *                     food production planning problem.  It
 *                     demonstrates the use of CPLEX's
 *                     linearization capability.
 */

import ilog.concert.*;
import ilog.cplex.*;

public class FoodManufact {
   final static int v1 = 0;
   final static int v2 = 1;
   final static int o1 = 2;
   final static int o2 = 3;
   final static int o3 = 4;

   final static double[][] cost = {{110.0, 120.0, 130.0, 110.0, 115.0},
                                   {130.0, 130.0, 110.0,  90.0, 115.0},
                                   {110.0, 140.0, 130.0, 100.0,  95.0},
                                   {120.0, 110.0, 120.0, 120.0, 125.0},
                                   {100.0, 120.0, 150.0, 110.0, 105.0},
                                   { 90.0, 100.0, 140.0,  80.0, 135.0}};

   public static void main (String args[]) {
      int nMonths   = cost.length;
      int nProducts = cost[0].length;

      try (IloCplex cplex = new IloCplex()) {
         IloNumVar[] produce = cplex.numVarArray(nMonths, 0, Double.MAX_VALUE);
         IloNumVar[][] use   = new IloNumVar[nMonths][];
         IloNumVar[][] buy   = new IloNumVar[nMonths][];
         IloNumVar[][] store = new IloNumVar[nMonths][];

         for (int i = 0; i < nMonths; i++) {
            use[i]   = cplex.numVarArray(nProducts, 0, Double.MAX_VALUE);
            buy[i]   = cplex.numVarArray(nProducts, 0, Double.MAX_VALUE);
            store[i] = cplex.numVarArray(nProducts, 0, 1000.0);
         } 

         for (int p = 0; p < nProducts; p++) {
           store[nMonths-1][p].setLB(500.0);
           store[nMonths-1][p].setUB(500.0);
         }

         IloNumExpr profit = cplex.numExpr();
         for (int i = 0; i < nMonths; i++) {
            // Not more than 200 tons of vegetable oil can be refined
            cplex.addLe(cplex.sum(use[i][v1], use[i][v2]), 200.0); 

            // Not more than 250 tons of non-vegetable oil can be refined
            cplex.addLe(cplex.sum(use[i][o1], use[i][o2], use[i][o3]), 250.0); 

            // Constraints on food composition 
            cplex.addLe(cplex.prod(3., produce[i]),
                        cplex.sum(cplex.prod(8.8, use[i][v1]),
                                  cplex.prod(6.1, use[i][v2]),
                                  cplex.prod(2.0, use[i][o1]),
                                  cplex.prod(4.2, use[i][o2]),
                                  cplex.prod(5.0, use[i][o3])));
            cplex.addGe(cplex.prod(6., produce[i]),
                        cplex.sum(cplex.prod(8.8, use[i][v1]),
                                  cplex.prod(6.1, use[i][v2]),
                                  cplex.prod(2.0, use[i][o1]),
                                  cplex.prod(4.2, use[i][o2]),
                                  cplex.prod(5.0, use[i][o3])));
            cplex.addEq(produce[i], cplex.sum(use[i]));

            // Raw oil can be stored for later use
            if (i == 0) {
               for (int p = 0; p < nProducts; p++)
                  cplex.addEq(cplex.sum(500.0, buy[i][p]),
                              cplex.sum(use[i][p], store[i][p]));
            }
            else {
               for (int p = 0; p < nProducts; p++)
                 cplex.addEq(cplex.sum(store[i-1][p], buy[i][p]),
                             cplex.sum(use[i][p], store[i][p]));
            }

            // Logical constraints:
            // The food cannot use more than 3 oils 
            // (or at least two oils must not be used)
            cplex.addGe(cplex.sum(cplex.eq(use[i][v1], 0),
                                  cplex.eq(use[i][v2], 0),
                                  cplex.eq(use[i][o1], 0),
                                  cplex.eq(use[i][o2], 0),
                                  cplex.eq(use[i][o3], 0)), 2);

            // When an oil is used, the quantity must be at least 20 tons
            for (int p = 0; p < nProducts; p++)
               cplex.add(cplex.or(cplex.eq(use[i][p],  0),
                                  cplex.ge(use[i][p], 20)));

            // If products v1 or v2 are used, then product o3 is also used
            cplex.add(cplex.ifThen(cplex.or(cplex.ge(use[i][v1], 20),
                                            cplex.ge(use[i][v2], 20)),
                                   cplex.ge(use[i][o3], 20)));

            // Objective function
            profit = cplex.sum (profit, cplex.prod(150, produce[i]));
            profit = cplex.diff(profit, cplex.scalProd(cost[i], buy[i]));
            profit = cplex.diff(profit, cplex.prod(5, cplex.sum(store[i])));
         }

         cplex.addMaximize(profit);

         if ( cplex.solve() ) {
            System.out.println("Solution status: " + cplex.getStatus());
            System.out.println(" Maximum profit = " + cplex.getObjValue());
            for (int i = 0; i < nMonths; i++) {
               System.out.println(" Month " + i);
               System.out.print("  . buy   ");
               for (int p = 0; p < nProducts; p++)
                  System.out.print(cplex.getValue(buy[i][p]) + "\t ");
               System.out.println();
               System.out.print("  . use   ");
               for (int p = 0; p < nProducts; p++)
                  System.out.print(cplex.getValue(use[i][p]) + "\t ");
               System.out.println();
               System.out.print("  . store ");
               for (int p = 0; p < nProducts; p++)
                  System.out.print(cplex.getValue(store[i][p]) + "\t ");
               System.out.println();
            }
         }

      }
      catch (IloException e) {
         System.err.println("Concert exception caught: " + e);
         System.exit(-1);
      }
   }
}
