/* --------------------------------------------------------------------------
 * File: AdMIPex9.java
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
 * AdMIPex9.java - Inject heuristic solutions from the generic callback.
 *
 * Optimizes an all binary MIP problem.
 *
 * See AdMIPex2.java for an implementation using legacy callbacks.
 *
 * To run this example, the user must specify a problem file.
 *
 * Example:
 *     java AdMIPex9 example.mps
 */

import ilog.concert.*;
import ilog.cplex.*;
import java.util.HashMap;
import java.util.Map;

public class AdMIPex9 {

   /**
    * A generic callback class that injects heuristic solutions.
    */
   private static class HeuristicCallback implements IloCplex.Callback.Function {

      /** All variables in the model. */
      IloNumVar[] _vars;

      /** Dense objective vector (aligned with <code>_vars</code>). */
      double[] _obj;

      /** Keeps track of the number of times the roundDown method is called. */
      int timesCalled;

      public HeuristicCallback(IloNumVar[] vars, IloObjective obj) throws IloException {
         _vars = vars;

         // Generate the objective as a double array for easy look up
         IloLinearNumExpr objexp     = (IloLinearNumExpr)obj.getExpr();
         IloLinearNumExprIterator it = objexp.linearIterator();

         // First create a map with all variables that have non-zero
         // coefficients in the objective.
         Map<IloNumVar,Double> _objmap = new HashMap<IloNumVar,Double>();
         while ( it.hasNext() ) {
            IloNumVar v = it.nextNumVar();
            _objmap.put(v, it.getValue());
         }

         // Now turn the map into a dense array.
         int cols = _vars.length;
         _obj = new double[cols];
         for (int j = 0; j < cols; j++) {
            if ( _objmap.containsKey(_vars[j]) ) {
               _obj[j] = _objmap.get(_vars[j]);
            }
         }
      }

      /**
       * Heuristic motivated by knapsack constrained problems.
       *
       * Rounding down all fractional values will give an integer
       * solution that is feasible, since all constraints are <=
       * with positive coefficients.
       */
      private void roundDown(IloCplex.Callback.Context context) throws IloException {
         double[] x       = context.getRelaxationPoint(_vars);
         int    cols      = _vars.length;
         double objrel    = context.getRelaxationObjective();

         timesCalled++;

         for (int j = 0; j < cols; j++) {
            // Set the fractional variable to zero.
            // Note that we assume all variables to be binary. If the model
            // contains non-binary variable then a different update would
            // be required.
            if ( x[j] > 0.0 ) {
               double frac = Math.abs(Math.round(x[j]) - x[j]);

               if ( frac > 1.0e-6 ) {
                  objrel -= x[j]*_obj[j];
                  x[j]    = 0.0;
               }
            }
         }

         // Post the rounded solution, CPLEX will check feasibility.
         context.postHeuristicSolution(
            _vars, x, 0, cols, objrel,
            IloCplex.Callback.Context.SolutionStrategy.CheckFeasible);
      }

      /**
       * Implements the required invoke method.
       *
       * This is the method that we have to implement to fulfill the
       * generic callback contract. CPLEX will call this method during
       * the solution process at the places that we asked for.
       */
      @Override
      public void invoke (IloCplex.Callback.Context context) throws IloException {
         if ( context.inRelaxation() ) {
            roundDown(context);
         }
      }
   }

   /**
    * Use the generic callback for optimizing a MIP problem.
    */
   public static void main(String[] args) throws IloException {
      if ( args.length != 1 ) {
         System.out.println("Usage: AdMIPex9 filename");
         System.out.println("   where filename is a file with extension ");
         System.out.println("      MPS, SAV, or LP (lower case is allowed)");
         System.out.println(" Exiting...");
         System.exit(-1);
      }

      try (IloCplex cplex = new IloCplex()) {
         cplex.importModel(args[0]);

         if (cplex.getNcols() != cplex.getNbinVars()) {
            System.out.println("Problem contains non-binary variables, exiting");
            return;
         }

         IloLPMatrix lp  = (IloLPMatrix)cplex.LPMatrixIterator().next();
         IloNumVar[] vars = lp.getNumVars();

         // Now we get to setting up the generic callback.
         // We instantiate a HeuristicCallback and set the wherefrom parameter.
         HeuristicCallback cb = new HeuristicCallback(vars,
                                                      cplex.getObjective());
         long wherefrom = 0;
         wherefrom |= IloCplex.Callback.Context.Id.Relaxation;

         // We add the callback.
         cplex.use(cb, wherefrom);

         // Disable heuristics so that our callback has a chance to make a
         // difference.
         cplex.setParam(IloCplex.Param.MIP.Strategy.HeuristicFreq, -1);

         // Optimize the problem and obtain solution.
         if ( cplex.solve() ) {
            System.out.println("Solution status = " + cplex.getStatus());
            System.out.println("Solution value  = " + cplex.getObjValue());
         }

         System.out.printf("roundDown was called %d times%n",
                           cb.timesCalled);
      }
   }
}
