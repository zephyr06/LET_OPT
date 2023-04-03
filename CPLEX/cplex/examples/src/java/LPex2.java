/* --------------------------------------------------------------------------
 * File: LPex2.java
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
 * LPex2.java - Reading in and optimizing an LP problem
 *
 *
 * To run this example, command line arguments are required.
 * i.e.,   java LPex2   filename   method
 * where
 *     filename is the name of the file, with .mps, .lp, or .sav extension
 *     method   is the optimization method
 *                 o          default
 *                 p          primal simplex
 *                 d          dual   simplex
 *                 h          barrier with crossover
 *                 b          barrier without crossover
 *                 n          network with dual simplex cleanup
 *                 s          sifting
 *                 c          concurrent optimization
 * Example:
 *     java LPex2  example.mps  o
 */

import ilog.concert.*;
import ilog.cplex.*;


public class LPex2 {
   static void usage() {
      System.out.println("usage:  LPex2 <filename> <method>");
      System.out.println("          o       default");
      System.out.println("          p       primal simplex");
      System.out.println("          d       dual   simplex");
      System.out.println("          h       barrier with crossover");
      System.out.println("          b       barrier without crossover");
      System.out.println("          n       network with dual simplex cleanup");
      System.out.println("          s       sifting");
      System.out.println("          c       concurrent optimization");
   }

   public static void main(String[] args) {
      if ( args.length != 2 ) {
         usage();
         return;
      }

      // Create the modeler/solver object
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
         case 'h': cplex.setParam(IloCplex.Param.RootAlgorithm,
                                  IloCplex.Algorithm.Barrier);
                   break;
         case 'b': cplex.setParam(IloCplex.Param.RootAlgorithm,
                                  IloCplex.Algorithm.Barrier);
                   cplex.setParam(IloCplex.Param.SolutionType,
                                  IloCplex.SolutionType.NonBasicSolution);
                   break;
         case 'n': cplex.setParam(IloCplex.Param.RootAlgorithm,
                                  IloCplex.Algorithm.Network);
                   break;
         case 's': cplex.setParam(IloCplex.Param.RootAlgorithm,
                                  IloCplex.Algorithm.Sifting);
                   break;
         case 'c': cplex.setParam(IloCplex.Param.RootAlgorithm,
                                  IloCplex.Algorithm.Concurrent);
                   break;
         default:  usage();
                   return;
         }

         // Read model from file with name args[0] into cplex optimizer object
         cplex.importModel(args[0]);

         // Solve the model and display the solution if one was found
         if ( cplex.solve() ) {
            System.out.println("Solution status = " + cplex.getStatus());
            System.out.println("Solution value  = " + cplex.getObjValue());

            // Access the IloLPMatrix object that has been read from a file in
            // order to access variables which are the columns of the LP.  The
            // method importModel() guarantees that exactly one IloLPMatrix
            // object will exist, which is why no tests or iterators are
            // needed in the following line of code.
            IloLPMatrix lp = (IloLPMatrix)cplex.LPMatrixIterator().next();

            double[] x = cplex.getValues(lp);
            for (int j = 0; j < x.length; ++j)
               System.out.println("Variable " + j + ": Value = " + x[j]);

            try {     // basis may not exist
               IloCplex.BasisStatus[] cstat =
                     cplex.getBasisStatuses(lp.getNumVars());
               for (int j = 0; j < x.length; ++j)
                  System.out.println("Variable " + j +
                                     ": Basis Status = " + cstat[j]);
            }
            catch (Exception e) {
            }

            System.out.println("Maximum bound violation = " +
                   cplex.getQuality(IloCplex.QualityType.MaxPrimalInfeas));
         }
      }
      catch (IloException e) {
         System.err.println("Concert exception caught: " + e);
      }
   }
}
