/* --------------------------------------------------------------------------
 * File: AdMIPex1.java
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
 * AdMIPex1.java -- Use the node and branch callbacks to optimize
 *                  a MIP problem
 *
 * To run this example, command line arguments are required.
 * i.e.,   java AdMIPex1   filename
 *
 * Example:
 *     java AdMIPex1  example.mps
 */

import ilog.concert.*;
import ilog.cplex.*;


public class AdMIPex1 {
   static class MyBranch extends IloCplex.BranchCallback {
      IloNumVar[] _vars;
      MyBranch(IloNumVar[] vars) { _vars = vars; }

      public void main() throws IloException {
         if ( !getBranchType().equals(IloCplex.BranchType.BranchOnVariable) )
            return;

         // Branch on var with largest objective coefficient
         // among those with largest infeasibility

         double[] x   = getValues  (_vars);
         double[] obj = getObjCoefs(_vars);
         IloCplex.IntegerFeasibilityStatus[] feas = getFeasibilities(_vars);

         double maxinf = 0.0;
         double maxobj = 0.0;
         int    bestj  = -1;
         int    cols   = _vars.length;
         for (int j = 0; j < cols; ++j) {
            if ( feas[j].equals(IloCplex.IntegerFeasibilityStatus.Infeasible) ) {
               double xj_inf = x[j] - Math.floor(x[j]);
               if ( xj_inf > 0.5 )  xj_inf = 1.0 - xj_inf;
               if ( xj_inf >= maxinf                              &&
                    (xj_inf > maxinf || Math.abs(obj[j]) >= maxobj)  ) {
                  bestj  = j;
                  maxinf = xj_inf;
                  maxobj = Math.abs(obj[j]);
               }
            }
         }

         if ( bestj >= 0 ) {
            makeBranch(_vars[bestj], x[bestj],
                       IloCplex.BranchDirection.Up,   getObjValue());
            makeBranch(_vars[bestj], x[bestj],
                       IloCplex.BranchDirection.Down, getObjValue());
         }
      }
   }

   static class MySelect extends IloCplex.NodeCallback {
      public void main() throws IloException {
         long   remainingNodes = getNremainingNodes64();
         long   bestnode       = -1;
         int    maxdepth       = -1;
         double maxiisum       = 0.0;

         for (long i = 0; i < remainingNodes; ++i) {
            int depth = getDepth(i);
            double iisum = getInfeasibilitySum(i);
            if ( (depth >= maxdepth)                   &&
                 (depth > maxdepth || iisum > maxiisum)  ) {
               bestnode = i;
               maxdepth = depth;
               maxiisum = iisum;
            }
         }
         if ( bestnode >= 0 ) selectNode(bestnode);
      }
   }

   public static void main(String[] args) {
      if ( args.length != 1 ) {
         System.out.println("Usage: AdMIPex1 filename");
         System.out.println("   where filename is a file with extension ");
         System.out.println("      MPS, SAV, or LP (lower case is allowed)");
         System.out.println(" Exiting...");
         System.exit(-1);
      }

      try (IloCplex cplex = new IloCplex()) {
         cplex.importModel(args[0]);
         IloLPMatrix lp = (IloLPMatrix)cplex.LPMatrixIterator().next();

         cplex.use(new MyBranch(lp.getNumVars()));
         cplex.use(new MySelect());

         cplex.setParam(IloCplex.Param.MIP.Strategy.Search,
                        IloCplex.MIPSearch.Traditional);
         if ( cplex.solve() ) {
            System.out.println("Solution status = " + cplex.getStatus());
            System.out.println("Solution value  = " + cplex.getObjValue());
         }
      }
      catch (IloException e) {
         System.err.println("Concert exception caught: " + e);
      }
   }
}
