/* --------------------------------------------------------------------------
 * File: QPex2.java
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
 * QPex2.java - Reading in and optimizing a QP problem
 *
 * To run this example, command line arguments are required.
 * i.e.,   java QPex2  filename method
 * where 
 *     filename is the name of the file, with .mps, .lp, or .sav extension
 *     method   is the optimization method
 *                 o          default
 *                 p          primal simplex
 *                 d          dual   simplex
 *                 b          barrier without crossover
 *                 n          network with dual simplex cleanup
 *
 * Example:
 *     java QPex2  qpex.lp o
 */

import ilog.concert.*;
import ilog.cplex.*;


public class QPex2 {
   static void usage() {
      System.out.println("usage:  QPex2 <filename> <method>");
      System.out.println("          o       default");
      System.out.println("          p       primal simplex");
      System.out.println("          d       dual   simplex");
      System.out.println("          b       barrier without crossover");
      System.out.println("          n       network with dual simplex cleanup");
   }

   public static void main(String[] args) {
      if ( args.length != 2 ) {
         usage();
         return;
      }
      try (IloCplex cplex = new IloCplex()) {
         // Evaluate command line option and set optimization method accordingly.
         switch ( args[1].charAt(0) ) {
         case 'o': cplex.setParam(IloCplex.Param.RootAlgorithm,
                                  IloCplex.Algorithm.Auto); 
                   break;
         case 'p': cplex.setParam(IloCplex.Param.RootAlgorithm,
                                  IloCplex.Algorithm.Primal);
                   break;
         case 'd': cplex.setParam(IloCplex.Param.RootAlgorithm,
                                  IloCplex.Algorithm.Dual);
                   break;
         case 'b': cplex.setParam(IloCplex.Param.RootAlgorithm,
                                  IloCplex.Algorithm.Barrier);
                   cplex.setParam(IloCplex.Param.SolutionType,
                                  IloCplex.SolutionType.NonBasicSolution);
                   break;
         case 'n': cplex.setParam(IloCplex.Param.RootAlgorithm,
                                  IloCplex.Algorithm.Network);
                   break;
         default:  usage();
                   return;
         }
       
         cplex.importModel(args[0]);

       
         if ( cplex.solve() ) {
            System.out.println("Solution status = " + cplex.getStatus());
            System.out.println("Solution value  = " + cplex.getObjValue());
          
            IloLPMatrix lp = (IloLPMatrix)cplex.LPMatrixIterator().next();
          
            double[] x = cplex.getValues(lp);
            for (int j = 0; j < x.length; ++j) {
               System.out.println("Variable " + j + ": Value = " + x[j]);
            }
         }
      }
      catch (IloException e) {
         System.err.println("Concert exception caught: " + e);
         System.exit(-1);
      }
   }
}
