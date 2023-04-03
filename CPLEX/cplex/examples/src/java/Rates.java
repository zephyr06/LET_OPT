/* --------------------------------------------------------------------------
 * File: Rates.java
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
 * Rates.java -- modeling with semi-continuous variables
 *
 * Problem Description:
 * 
 * Assume you run a power supply company.  You have several power generators
 * available, each of which has a minimum and maximum production level and
 * a cost per unit output.  The question is which generators to use in order
 * to minimize the overall operation cost while satisfying the demand.
 */

import ilog.concert.*;
import ilog.cplex.*;
import java.io.*;

public class Rates {
   static int _generators;
   
   static double[] _minArray;
   static double[] _maxArray;
   static double[] _cost;
   static double   _demand;
   
   static void readData(String fileName)
                         throws IOException,
                                InputDataReader.InputDataReaderException {
      InputDataReader reader = new InputDataReader(fileName);
      try {
	  _minArray = reader.readDoubleArray();
	  _maxArray = reader.readDoubleArray();
	  _cost     = reader.readDoubleArray();
	  _demand   = reader.readDouble();
	  
	  _generators = _minArray.length;
      }
      finally {
	  reader.close();
      }
   }
   
   public static void main( String[] args ) {

      String filename = "../../../examples/data/rates.dat";
      if (args.length > 0)
         filename = args[0];

      try {
         readData(filename);
      }
      catch (IOException|
             InputDataReader.InputDataReaderException ex) {
         System.out.println("Data Error: " + ex);
         System.exit(-1);
      }

      try (IloCplex cplex = new IloCplex()) {
         IloNumVar[] production = new IloNumVar[_generators];
         for (int j = 0; j < _generators; ++j) {
            production[j] = cplex.semiContVar(_minArray[j], _maxArray[j],
                                              IloNumVarType.Float);
         }
       
         cplex.addMinimize(cplex.scalProd(_cost, production));
         cplex.addGe(cplex.sum(production), _demand);
       
         cplex.exportModel("rates.lp");
         if ( cplex.solve() ) {
            System.out.println("Solution status: " + cplex.getStatus());
            for (int j = 0; j < _generators; ++j) {
               System.out.println("   generator " + j + ": " +
                                  cplex.getValue(production[j]));
            }
            System.out.println("Total cost = " + cplex.getObjValue());
         }
         else
            System.out.println("No solution");
       
      }
      catch (IloException exc) {
         System.err.println("Concert exception '" + exc + "' caught");
         System.exit(-1);
      }
   }
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
*/
