/* --------------------------------------------------------------------------
 * File: QCPex1.java
 * Version 22.1.0
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of IBM
 * 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
 * Copyright IBM Corporation 2003, 2022. All Rights Reserved.
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with
 * IBM Corp.
 * --------------------------------------------------------------------------
 *
 * QCPex1.java - Entering and optimizing a quadratically constrained problem
 */

import ilog.concert.*;
import ilog.cplex.*;


public class QCPex1 {
   public static void main(String[] args) {
      try (IloCplex cplex = new IloCplex()) {
         IloRange[]  row   = new IloRange[3];
         IloNumVar[] var   = populateByRow(cplex, row);

         if ( cplex.solve() ) {
            double[] x     = cplex.getValues(var);
            double[] slack = cplex.getSlacks(row);

            System.out.println("Solution status = " + cplex.getStatus());
            System.out.println("Solution value  = " + cplex.getObjValue());

            int nvars = x.length;
            for (int j = 0; j < nvars; ++j)
               System.out.println("Variable " + j + ": Value = " + x[j]);

            int ncons = slack.length;
            for (int i = 0; i < ncons; ++i)
               System.out.println("Constraint " + i + ": Slack = " + slack[i]);

            cplex.exportModel("qcpex1.lp");
         }
      }
      catch (IloException e) {
         System.err.println("Concert exception '" + e + "' caught");
         System.exit(-1);
      }
   }

   static IloNumVar[] populateByRow(IloMPModeler model,
                                    IloRange     row[]) throws IloException {
      double[]    lb = {0.0, 0.0, 0.0};
      double[]    ub = {40.0, Double.MAX_VALUE, Double.MAX_VALUE};
      IloNumVar[] x  = model.numVarArray(3, lb, ub);

      // - x0 +   x1 + x2 <= 20
      //   x0 - 3*x1 + x2 <= 30
      double[][] val = { {-1.0,  1.0,  1.0},
                         { 1.0, -3.0,  1.0} };
      row[0] = model.addLe(model.scalProd(val[0], x), 20.0);
      row[1] = model.addLe(model.scalProd(val[1], x), 30.0);

      // x0*x0 + x1*x1 + x2*x2 <= 1.0
      row[2] = model.addLe(model.sum(model.prod(x[0], x[0]),
                                     model.prod(x[1], x[1]),
                                     model.prod(x[2], x[2])), 1.0);

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

      return x;
   }
}
