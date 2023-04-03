/* --------------------------------------------------------------------------
 * File: Blend.java
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
 * 
 * Problem Description
 * -------------------
 * 
 * Goal is to blend four sources to produce an alloy: pure metal, raw
 * materials, scrap, and ingots.
 * 
 * Each source has a cost.
 * Each source is made up of elements in different proportions.
 * Alloy has minimum and maximum proportion of each element.
 * 
 * Minimize cost of producing a requested quantity of alloy.
 */ 

import ilog.concert.*;
import ilog.cplex.*;


public class Blend {
   static int    _nbElements = 3;
   static int    _nbRaw = 2;
   static int    _nbScrap = 2;
   static int    _nbIngot = 1;
   static double _alloy = 71.0;
   
   static double[] _cm = {22.0, 10.0, 13.0};
   static double[] _cr = {6.0, 5.0};
   static double[] _cs = {7.0, 8.0};
   static double[] _ci = {9.0};
   static double[] _p  = {0.05, 0.30, 0.60};
   static double[] _P  = {0.10, 0.40, 0.80};
   
   static double[][] _PRaw = {{0.20, 0.01},
                              {0.05, 0.00},
                              {0.05, 0.30}};
   static double[][] _PScrap = {{0.00, 0.01},
                                {0.60, 0.00},
                                {0.40, 0.70}};
   static double[][] _PIngot = {{0.10},
                                {0.45},
                                {0.45}};
   

   public static void main( String[] args ) {
      try (IloCplex cplex = new IloCplex()) {
         IloNumVar[] m = cplex.numVarArray(_nbElements, 0.0, Double.MAX_VALUE);
         IloNumVar[] r = cplex.numVarArray(_nbRaw, 0.0, Double.MAX_VALUE);
         IloNumVar[] s = cplex.numVarArray(_nbScrap, 0.0, Double.MAX_VALUE);
         IloNumVar[] i = cplex.numVarArray(_nbIngot, 0.0, Double.MAX_VALUE);
         IloNumVar[] e = new IloNumVar[_nbElements];
       
         // Objective Function: Minimize Cost
         cplex.addMinimize(cplex.sum(cplex.scalProd(_cm, m),
                                     cplex.scalProd(_cr, r),
                                     cplex.scalProd(_cs, s), 
                                     cplex.scalProd(_ci, i)));
       
         // Min and max quantity of each element in alloy
         for (int j = 0; j < _nbElements; j++) {
            e[j] = cplex.numVar(_p[j] * _alloy, _P[j] * _alloy);
         }
       
         // Constraint: produce requested quantity of alloy
         cplex.addEq(cplex.sum(e), _alloy);
       
         // Constraints: Satisfy element quantity requirements for alloy
         for (int j = 0; j < _nbElements; j++) {
            cplex.addEq(e[j],
                        cplex.sum(m[j],
                                  cplex.scalProd(_PRaw[j], r),
                                  cplex.scalProd(_PScrap[j], s),
                                  cplex.scalProd(_PIngot[j], i)));
         }
       
         if (cplex.solve()) {
            if (cplex.getStatus().equals(IloCplex.Status.Infeasible)) {
               System.out.println("No Solution");
               return;
            }
            System.out.println("Solution status: " + cplex.getStatus());        
            double[] mVals = cplex.getValues(m);
            double[] rVals = cplex.getValues(r);
            double[] sVals = cplex.getValues(s);
            double[] iVals = cplex.getValues(i);
            double[] eVals = cplex.getValues(e);
            
            // Print results
            System.out.println("Cost:" + cplex.getObjValue());

            System.out.println("Pure metal:");
            for(int j = 0; j < _nbElements; j++)
               System.out.println("(" + j + ") " + mVals[j]);

            System.out.println("Raw material:");
            for(int j = 0; j < _nbRaw; j++)
               System.out.println("(" + j + ") " + rVals[j]);

            System.out.println("Scrap:");
            for(int j = 0; j < _nbScrap; j++)
               System.out.println("(" + j + ") " + sVals[j]);

            System.out.println("Ingots:");
            for(int j = 0; j < _nbIngot; j++)
               System.out.println("(" + j + ") " + iVals[j]);

            System.out.println("Elements:");
            for(int j = 0; j < _nbElements; j++)
               System.out.println("(" + j + ") " + eVals[j]);
         }
      }
      catch (IloException exc) {
         exc.printStackTrace();
      }
   }
}

/*
Cost:653.554
Pure metal:
0) 0
1) 0
2) 0
Raw material:
0) 0
1) 0
Scrap:
0) 17.059
1) 30.2311
Ingots : 
0) 32.4769
Elements:
0) 3.55
1) 24.85
2) 42.6
*/
