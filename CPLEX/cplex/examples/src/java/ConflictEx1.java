/* --------------------------------------------------------------------------
 * File: ConflictEx1.java
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
import java.io.BufferedReader;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import ilog.concert.*;
import ilog.cplex.*;


/**
 * Read a problem and use the conflict refiner.
 *
 * See the usage message below for more details.
 */
public class ConflictEx1 {

   /**
    * Display a usage message and exit.
    */
   static void usage() {
      System.out.println("Usage: java ConflictEx1 filename");
      System.out.println("  filename   Name of a file, with .mps, .lp, or .sav");
      System.out.println("             extension, and a possible, additional .gz");
      System.out.println("             extension");
      System.exit(2);
   }

   /**
    * Invoke the conflict refiner on a model and display the results.
    */
   public static void main(String[] args) throws Exception {
      if (args.length != 1) {
         usage();
      }

      // Create the modeler/solver object
      try (IloCplex cplex = new IloCplex()) {
         // Read model from file with name args[0] into cplex optimizer object.
         cplex.importModel(args[0]);

         // A list of constraints to be considered by the conflict refiner.
         List<IloConstraint> constraints = new ArrayList<IloConstraint>();

         // Loop over all objects in the model and gather constraints.
         for (Iterator iter = cplex.iterator(); iter.hasNext(); ) {
            Object o = iter.next();
            if (o instanceof IloLPMatrix) {
               // The method importModel() guarantees that exactly one
               // IloLPMatrix object will exist.
               IloLPMatrix lp = (IloLPMatrix)o;

               // Add linear constraints.
               for (IloRange rng : lp.getRanges())
                  constraints.add(rng);

               // Add variable bounds.
               for (IloNumVar v : lp.getNumVars()) {
                  if (v.getType() != IloNumVarType.Bool) {
                     constraints.add(cplex.lowerBound(v));
                     constraints.add(cplex.upperBound(v));
                  }
               }
            } else if (o instanceof IloConstraint) {
               // Add general constraints (e.g., SOS, PWL, indicator, etc.).
               constraints.add((IloConstraint)o);
            }
         }

         // Define preferences for the constraints. Here, we give all
         // constraints a preference of 1.0, so they will be treated
         // equally.
         double[] prefs = new double[constraints.size()];
         for (int i = 0; i < prefs.length; ++i)
            prefs[i] = 1.0;

         IloConstraint[] cons = constraints.toArray(
            new IloConstraint[constraints.size()]);

         // Run the conflict refiner. As opposed to letting the conflict
         // refiner run to completion (as is done here), the user can set
         // a resource limit (e.g., a time limit, an iteration limit, or
         // node limit) and still potentially get a "possible" conflict.
         if (cplex.refineConflict(cons, prefs)) {
            // Display the solution status.
            System.out.println("Solution status = " + cplex.getCplexStatus());

            // Get the conflict status for the constraints that were specified.
            IloCplex.ConflictStatus[] conflict = cplex.getConflict(cons);

            // Print constraints that participate in the conflict.
            System.out.println("Conflict:");

            for (int i = 0; i < cons.length; ++i) {
               if (conflict[i] == IloCplex.ConflictStatus.Member ||
                   conflict[i] == IloCplex.ConflictStatus.PossibleMember) {
                  System.out.println("  " + cons[i]);
               }
            }

            System.out.println();

            // Write the identified conflict in the LP format.
            final String confFile = "ConflictEx1.java.lp";
            System.out.printf("Writing conflict file to '%s'....%n", confFile);
            cplex.writeConflict(confFile);

            // Display the entire conflict subproblem.
            try (BufferedReader br = new BufferedReader(new FileReader(confFile))) {
               String line = null;
               while ((line = br.readLine()) != null) {
                  System.out.println(line);
               }
            }
         }
         else {
            System.out.println("A conflict was not identified.");
            System.out.println("Exiting....");
            return;
         }
      }

   }
}
