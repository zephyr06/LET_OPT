/* --------------------------------------------------------------------------
 * File: MultiObjEx1.java
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
 */
import java.util.Arrays;

import ilog.concert.IloException;
import ilog.cplex.IloCplex;
import ilog.cplex.IloCplex.MultiObjIntInfo;
import ilog.cplex.IloCplex.MultiObjNumInfo;
import ilog.cplex.IloCplex.ParameterSet;

/**
 * Reading and optimizing a multiobjective problem.
 *
 * Demonstrates specifying parameter sets and accessing the results of
 * the various objectives.
 *
 * See the usage message for more details.
 */
public class MultiObjEx1 {

   /**
    *  Print a usage message to stderr and abort.
    */
   static void usage() {
      System.err.println();
      System.err.println("Usage: MultiObjEx1 <filename> [<prmfile1> <prmfile1> ...]");
      System.err.println();
      System.err.println("   The files prmfile1, ... are applied as paramsets for");
      System.err.println("   the different objective priorities.");
      System.err.println();
      System.exit(2);
   }

   public static void main(String[] args) throws IloException {
      // Handle command line arguments.
      if (args.length < 1)
         usage();

      String filename = args[0];
      String[] paramfiles = Arrays.copyOfRange(args, 1, args.length);

      try (IloCplex cpx = new IloCplex()) {
         cpx.importModel(filename);

         // Read in all of the parameter sets.
         ParameterSet[] paramsets = new ParameterSet[paramfiles.length];
         for (int i = 0; i < paramsets.length; ++i)
            paramsets[i] = cpx.readParameterSet(paramfiles[i]);

         // If we failed to optimize, then exit early.
         if (!cpx.solve(paramsets)) {
            throw new IloException(String.format(
               "Failed to optimize. CPLEX status = %s",
               cpx.getCplexStatus().toString()));
         }

         System.out.println();
         System.out.printf("Solution status = %s%n", cpx.getStatus());
         System.out.println();

         // Print the values of the various objectives.
         System.out.println("Objective values...");
         for (int i = 0; i < cpx.getMultiObjNsolves(); ++i) {
            System.out.printf("Objective priority %d value = %f%n",
                              cpx.getMultiObjInfo(MultiObjIntInfo.MultiObjPriority, i),
                              cpx.getMultiObjInfo(MultiObjNumInfo.MultiObjObjValue, i));
         }
      }
   }

}
