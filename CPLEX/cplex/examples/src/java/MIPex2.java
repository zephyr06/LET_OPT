/* --------------------------------------------------------------------------
 * File: MIPex2.java
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
 * MIPex2.java - Reading in and optimizing a MIP problem.  In fact, this
 *               program also works for LP or QP problems, but is different
 *               from LPex2 in that no dual solution information is queried.
 *
 * To run this example, command line arguments are required.
 * i.e.,   java MIPex2  filename
 * where 
 *     filename is the name of the file, with .mps, .lp, or .sav extension
 * Example:
 *     java MIPex2  mexample.mps
 */

import ilog.concert.*;
import ilog.cplex.*;


public class MIPex2 {
   static void usage() {
      System.out.println("usage:  MIPex2 <filename>");
   }

   public static void main(String[] args) {
      if ( args.length != 1 ) {
         usage();
         return;
      }
      try (IloCplex cplex = new IloCplex()) {
         cplex.importModel(args[0]);
       
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
