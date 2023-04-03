/* --------------------------------------------------------------------------
 * File: GenericBranch.java
 * Version 22.1.0
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of IBM
 * 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
 * Copyright IBM Corporation 2019, 2022. All Rights Reserved.
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with
 * IBM Corp.
 * --------------------------------------------------------------------------
 */

import ilog.concert.IloException;
import ilog.concert.IloLPMatrix;
import ilog.concert.IloNumVar;
import ilog.concert.IloNumVarType;
import ilog.concert.IloRange;

import ilog.cplex.IloCplex;

/**
 * Demonstrates how to perform customized branching using the generic
 * callback.
 *
 * For any model with integer variables passed on the command line, the
 * code will solve the model using a simple customized branching strategy.
 * The branching strategy implemented here is most-infeasible branching,
 * i.e., the code always picks the integer variable that is most fractional
 * and then branches on it. If the biggest fractionality of all integer
 * variables is small then the code refrains from custom branching and lets
 * CPLEX decide.
 *
 * See usage message below.
 */
public final class GenericBranch {

   /** Generic callback that implements most infeasible branching. */
   private static final class BranchCallback implements IloCplex.Callback.Function {
      private final IloNumVar[] x;
      private int calls = 0;
      private int branches = 0;

      public BranchCallback(IloNumVar[] x) { this.x = x; }

      public void invoke(IloCplex.Callback.Context context) throws IloException {
         // NOTE: Strictly speaking, the increment of calls and branches
         //       should be protected by a lock/mutex/semaphore. However, to keep
         //       the code simple we don't do that here.
         ++calls;

         // For sake of illustration prune every node that has a depth larger
         // than 1000.
         long depth = context.getLongInfo(IloCplex.Callback.Context.Info.NodeDepth);
         if ( depth > 1000 ) {
            context.pruneCurrentNode();
            return;
         }

         // Get the current relaxation.
         // The function not only fetches the objective value but also makes sure
         // the node lp is solved and returns the node lp's status. That status can
         // be used to identify numerical issues or similar
         final IloCplex.CplexStatus status = context.getRelaxationStatus();

         // Only branch if the current node relaxation could be solved to
         // optimality.
         // If there was any sort of trouble then don't do anything and thus let
         // CPLEX decide how to cope with that.
         if ( !status.equals(IloCplex.CplexStatus.Optimal)       &&
              !status.equals(IloCplex.CplexStatus.OptimalInfeas)   ) {
            return;
         }

         final double obj = context.getRelaxationObjective();

         // Node lp was solved to optimality. Grab the current relaxation
         // and find the most fractional variable
         final double[] v = context.getRelaxationPoint(x);
         int maxVar = -1;
         double maxFrac = 0.0;
         for (int i = 0; i < x.length; ++i) {
            if ( !x[i].getType().equals(IloNumVarType.Float) ) {
               double intval = Math.round(v[i]);
               double frac = Math.abs(intval - v[i]);

               if ( frac > maxFrac ) {
                  maxFrac = frac;
                  maxVar = i;
               }
            }
         }

         // If the maximum fractionality of all integer variables is small then
         // don't create a custom branch. Instead let CPLEX decide how to
         // branch.
         double minFrac = 0.1;

         if ( maxFrac > minFrac ) {
            long upChild, downChild;
            // There is a variable with a sufficiently fractional value.
            // Branch on that variable.
            double up = Math.ceil(v[maxVar]);
            double down = Math.floor(v[maxVar]);
            IloNumVar branchVar = x[maxVar];

            // Create UP branch (branchVar >= up)
            upChild = context.makeBranch(branchVar, up, IloCplex.BranchDirection.Up, obj);
            ++branches;

            // Create DOWN branch (branchVar <= down)
            downChild = context.makeBranch(branchVar, down, IloCplex.BranchDirection.Down, obj);
            ++branches;

         }
      }

      int getCalls() { return calls; }
      int getBranches() { return branches; }
   }

   /**
    * Print a usage message and exit.
    */
   private static void usage() {
      System.out.println(
         "Usage: GenericBranch filename...");
      System.out.println(
         "  filename   Name of a file, or multiple files, with .mps, .lp,");
      System.out.println(
         "             or .sav extension, and a possible, additional .gz");
      System.out.println(
         "             extension.");
      System.exit(2);
   }

   public static void main(String[] args) throws IloException {
      if (args.length <= 0)
         usage();

      // Loop over all command line arguments.
      for (String modelName : args) {
         try (IloCplex cplex = new IloCplex()) {
            // Read in the model
            cplex.importModel(modelName);
            final IloLPMatrix matrix = (IloLPMatrix)cplex.LPMatrixIterator().next();
            final IloNumVar[] x = matrix.getNumVars();

            // Register the callback function.
            BranchCallback cb = new BranchCallback(x);
            cplex.use(cb, IloCplex.Callback.Context.Id.Branching);

            // Limit the number of nodes.
            // The branching strategy implemented here is not smart so solving even
            // a simple MIP may turn out to take a long time.
            cplex.setParam(IloCplex.Param.MIP.Limits.Nodes, 1000);


            // Solve the model and report some statistics
            boolean feasible = cplex.solve();
            System.out.println("Model " + modelName + " solved, status = " + cplex.getStatus());
            if ( feasible )
               System.out.println("Objective value: " + cplex.getObjValue());
            System.out.println("Callback was invoked " + cb.getCalls() +  " times and created " + cb.getBranches() + " branches");
         }
      }
   }
}
