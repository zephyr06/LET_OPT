/* --------------------------------------------------------------------------
 * File: QPex1.java
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
 * QPex1.java - Entering and optimizing a QP problem
 */

import ilog.concert.*;
import ilog.cplex.*;


public class QPex1 {
   public static void main(String[] args) {
      try (IloCplex cplex = new IloCplex()) {
         IloLPMatrix lp = populateByRow(cplex);

         if ( cplex.solve() ) {
            double[] x     = cplex.getValues(lp);
            double[] dj    = cplex.getReducedCosts(lp);
            double[] pi    = cplex.getDuals(lp);
            double[] slack = cplex.getSlacks(lp);

            System.out.println("Solution status = " + cplex.getStatus());
            System.out.println("Solution value  = " + cplex.getObjValue());

            int nvars = x.length;
            for (int j = 0; j < nvars; ++j) {
               System.out.println("Variable " + j +
                                  ": Value = " + x[j] +
                                  " Reduced cost = " + dj[j]);
            }

            int ncons = slack.length;
            for (int i = 0; i < ncons; ++i) {
               System.out.println("Constraint " + i +
                                  ": Slack = " + slack[i] +
                                  " Pi = " + pi[i]);
            }

            cplex.exportModel("qpex1.lp");
         }
      }
      catch (IloException e) {
         System.err.println("Concert exception '" + e + "' caught");
         System.exit(-1);
      }
   }

   static IloLPMatrix populateByRow(IloMPModeler model) throws IloException {
      IloLPMatrix lp = model.addLPMatrix();

      double[]    lb = {0.0, 0.0, 0.0};
      double[]    ub = {40.0, Double.MAX_VALUE, Double.MAX_VALUE};
      IloNumVar[] x  = model.numVarArray(model.columnArray(lp, 3), lb, ub);

      // - x0 +   x1 + x2 <= 20
      //   x0 - 3*x1 + x2 <= 30
      double[]   lhs = {-Double.MAX_VALUE, -Double.MAX_VALUE};
      double[]   rhs = {20.0, 30.0};
      double[][] val = {{-1.0,  1.0,  1.0},
                        { 1.0, -3.0,  1.0}};
      int[][]    ind = {{0, 1, 2},
                        {0, 1, 2}};
      lp.addRows(lhs, rhs, ind, val);

      // Q = 0.5 ( 33*x0*x0 + 22*x1*x1 + 11*x2*x2 - 12*x0*x1 - 23*x1*x2 )
      IloNumExpr x00 = model.prod( 33.0, x[0], x[0]);
      IloNumExpr x11 = model.prod( 22.0, x[1], x[1]);
      IloNumExpr x22 = model.prod( 11.0, x[2], x[2]);
      IloNumExpr x01 = model.prod(-12.0, x[0], x[1]);
      IloNumExpr x12 = model.prod(-23.0, x[1], x[2]);
      IloNumExpr Q   = model.prod(0.5, model.sum(x00, x11, x22, x01, x12));

      // maximize x0 + 2*x1 + 3*x2 + Q
      double[] objvals = {1.0, 2.0, 3.0};
      model.add(model.maximize(model.diff(model.scalProd(x, objvals), Q)));

      return (lp);
   }
}
