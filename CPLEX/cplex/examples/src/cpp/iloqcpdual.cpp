// -------------------------------------------------------------- -*- C++ -*-
// File: iloqcpdual.cpp
// Version 22.1.0
// --------------------------------------------------------------------------
// Licensed Materials - Property of IBM
// 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
// Copyright IBM Corporation 2000, 2022. All Rights Reserved.
//
// US Government Users Restricted Rights - Use, duplication or
// disclosure restricted by GSA ADP Schedule Contract with
// IBM Corp.
// --------------------------------------------------------------------------
//
// iloqcpdual.cpp - Illustrates how to query and analyze dual values of QCPs
//

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

#define ZEROTOL 1e-6

namespace {
   // At someplaces we need to get the index of a variable or constraint
   // in an array of variables or constraints. To this end we store the
   // index of each object in that array as the user object of the variable
   // or constraint. The idx() function returns the index stored with a
   // variable or a constraint.
   template<typename T>
   int idx(T const& t) { return *static_cast<int *>(t.getObject()); }

   /* ***************************************************************** *
    *                                                                   *
    *    C A L C U L A T E   D U A L S   F O R   Q U A D S              *
    *                                                                   *
    *   CPLEX does not give us the dual multipliers for quadratic       *
    *   constraints directly. This is because they may not be properly  *
    *   defined at the cone top and deciding whether we are at the cone *
    *   top or not involves (problem specific) tolerance issues. CPLEX  *
    *   instead gives us all the values we need in order to compute the *
    *   dual multipliers if we are not at the cone top.                 *
    *                                                                   *
    * ***************************************************************** */
   // Calculate dual multipliers for quadratic constraints from dual
   // slack vectors and optimal solutions.
   // The dual multiplier is essentially the dual slack divided
   // by the derivative evaluated at the optimal solution. If the optimal
   // solution is 0 then the derivative at this point is not defined (we are
   // at the cone top) and we cannot compute a dual multiplier.
   // The 'qs' argument to the function must be an array with _all_
   // quadratic constraints in the model currently extracted to 'cplex'.
   IloNumArray getqconstrmultipliers (IloCplex cplex,
                                      IloNumArray optimalX,
                                      IloRangeArray qs)
   {
      IloNumArray ret(cplex.getEnv());

      for (IloInt q = 0; q < qs.getSize(); ++q) {
         IloNumVarArray vars(cplex.getEnv());
         IloNumArray vals(cplex.getEnv());

         // Get dual slack for constraint
         cplex.getQCDSlack(qs[q], vals, vars);

         IloNumArray grad(cplex.getEnv(), optimalX.getSize());
         bool conetop = true;
         for (IloExpr::QuadIterator it = qs[q].getQuadIterator();
              it.ok(); ++it)
         {
            int const x1 = idx(it.getVar1());
            int const x2 = idx(it.getVar2());
            grad[x1] += optimalX[x2] * it.getCoef();
            grad[x2] += optimalX[x1] * it.getCoef();
            if ( fabs (optimalX[x1]) > ZEROTOL ||
                 fabs (optimalX[x2]) > ZEROTOL )
               conetop = false;
         }
         if ( conetop )
            throw IloCplex::Exception(CPXERR_BAD_ARGUMENT,
                                      "Cannot compute dual multiplier at cone top!");

         // Compute qpi[q] as slack/gradient.
         // We may have several indices to choose from and use the one
         // with largest absolute value in the denominator.
         IloNum maxabs = -1.0;
         IloNum qpi = 0;
         for (IloInt i = 0; i < vars.getSize(); ++i) {
            int const j = idx(vars[i]);
            if ( fabs (grad[j]) > ZEROTOL ) {
               if ( fabs (grad[j]) > maxabs ) {
                  qpi = vals[i] / grad[j];
                  maxabs = fabs (grad[j]);
               }
            }
         }
         ret.add(qpi);

         grad.end();
         vals.end();
         vars.end();
      }

      return ret;
   }
}

int
main (int argc, char **argv)
{
   int result = 0;
   IloEnv env;

   try {
      static int indices[] = { 0, 1, 2, 3, 4, 5, 6 };
      IloModel model(env);

      /* ***************************************************************** *
       *                                                                   *
       *    S E T U P   P R O B L E M                                      *
       *                                                                   *
       *  The model we setup here is                                       *
       * Minimize                                                          *
       *  obj: 3x1 - x2 + 3x3 + 2x4 + x5 + 2x6 + 4x7                       *
       * Subject To                                                        *
       *  c1: x1 + x2 = 4                                                  *
       *  c2: x1 + x3 >= 3                                                 *
       *  c3: x6 + x7 <= 5                                                 *
       *  c4: -x1 + x7 >= -2                                               *
       *  q1: [ -x1^2 + x2^2 ] <= 0                                        *
       *  q2: [ 4.25x3^2 -2x3*x4 + 4.25x4^2 - 2x4*x5 + 4x5^2  ] + 2 x1 <= 9.0
       *  q3: [ x6^2 - x7^2 ] >= 4                                         *
       * Bounds                                                            *
       *  0 <= x1 <= 3                                                     *
       *  x2 Free                                                          *
       *  0 <= x3 <= 0.5                                                   *
       *  x4 Free                                                          *
       *  x5 Free                                                          *
       *  x7 Free                                                          *
       * End                                                               *
       *                                                                   *
       * ***************************************************************** */

      IloNumVarArray x(env);
      x.add(IloNumVar(env, 0, 3, "x1"));
      x.add(IloNumVar(env, -IloInfinity, IloInfinity, "x2"));
      x.add(IloNumVar(env, 0, 0.5, "x3"));
      x.add(IloNumVar(env, -IloInfinity, IloInfinity, "x4"));
      x.add(IloNumVar(env, -IloInfinity, IloInfinity, "x5"));
      x.add(IloNumVar(env, 0, IloInfinity, "x6"));
      x.add(IloNumVar(env, -IloInfinity, IloInfinity, "x7"));
      for (IloInt i = 0; i < x.getSize(); ++i)
         x[i].setObject(&indices[i]);

      IloObjective obj = IloMinimize(env,
                                     3*x[0] - x[1] + 3*x[2] + 2*x[3] +
                                     x[4] + 2*x[5] + 4*x[6], "obj");
      model.add(obj);

      IloRangeArray linear(env);
      linear.add(IloRange(env, 4.0, x[0] + x[1], 4.0, "c1"));
      linear.add(IloRange(env, 3.0, x[0] + x[2], IloInfinity, "c2"));
      linear.add(IloRange(env, -IloInfinity, x[5] + x[6], 5.0, "c3"));
      linear.add(IloRange(env, -2.0, -x[0] + x[6], IloInfinity, "c4"));
      for (IloInt i = 0; i < linear.getSize(); ++i)
         linear[i].setObject(&indices[i]);
      model.add(linear);

      IloRangeArray quad(env);
      quad.add(IloRange(env, -IloInfinity, -x[0]*x[0] + x[1] * x[1], 0, "q1"));
      quad.add(IloRange(env, -IloInfinity,
                        4.25*x[2]*x[2] - 2*x[2]*x[3] + 4.25*x[3]*x[3] +
                        -2*x[3]*x[4] + 4*x[4]*x[4] + 2*x[0],
                        9.0, "q2"));
      quad.add(IloRange(env, 4.0, x[5]*x[5] - x[6]*x[6], IloInfinity, "q3"));
      for (IloInt i = 0; i < quad.getSize(); ++i)
         quad[i].setObject(&indices[i]);
      model.add(quad);

      /* ***************************************************************** *
       *                                                                   *
       *    O P T I M I Z E   P R O B L E M                                *
       *                                                                   *
       * ***************************************************************** */
      IloCplex cplex(model);
      cplex.setParam(IloCplex::Param::Barrier::QCPConvergeTol, 1e-10);
      cplex.solve();

      /* ***************************************************************** *
       *                                                                   *
       *    Q U E R Y   S O L U T I O N                                    *
       *                                                                   *
       * ***************************************************************** */
      IloNumArray xval(env);
      IloNumArray slack(env);
      IloNumArray qslack(env);
      IloNumArray cpi(env);
      IloNumArray rpi(env);
      IloNumArray qpi;
      cplex.getValues(x, xval);
      cplex.getSlacks(slack, linear);
      cplex.getSlacks(qslack, quad);
      cplex.getReducedCosts(cpi, x);
      cplex.getDuals(rpi, linear);
      qpi = getqconstrmultipliers(cplex, xval, quad);

      
      /* ***************************************************************** *
       *                                                                   *
       *    C H E C K   K K T   C O N D I T I O N S                        *
       *                                                                   *
       *    Here we verify that the optimal solution computed by CPLEX     *
       *    (and the qpi[] values computed above) satisfy the KKT          *
       *    conditions.                                                    *
       *                                                                   *
       * ***************************************************************** */

      // Primal feasibility: This example is about duals so we skip this test.

      // Dual feasibility: We must verify
      // - for <= constraints (linear or quadratic) the dual
      //   multiplier is non-positive.
      // - for >= constraints (linear or quadratic) the dual
      //   multiplier is non-negative.
      for (IloInt i = 0; i < linear.getSize(); ++i) {
         if ( linear[i].getLB() <= -IloInfinity ) {
            // <= constraint
            if ( rpi[i] > ZEROTOL ) {
               cerr << "Dual feasibility test failed for <= row " << i
                    << ": " << rpi[i] << endl;
               result = -1;
            }
         }
         else if ( linear[i].getUB() >= IloInfinity ) {
            // >= constraint
            if ( rpi[i] < -ZEROTOL ) {
               cerr << "Dual feasibility test failed for >= row " << i
                    << ":" << rpi[i] << endl;
               result = -1;
            }
         }
         else {
            // nothing to do for equality constraints
         }
      }
      for (IloInt q = 0; q < quad.getSize(); ++q) {
         if ( quad[q].getLB() <= -IloInfinity ) {
            // <= constraint
            if ( qpi[q] > ZEROTOL ) {
               cerr << "Dual feasibility test failed for <= quad " << q
                    << ": " << qpi[q] << endl;
               result = -1;
            }
         }
         else if ( quad[q].getUB() >= IloInfinity ) {
            // >= constraint
            if ( qpi[q] < -ZEROTOL ) {
               cerr << "Dual feasibility test failed for >= quad " << q
                    << ":" << qpi[q] << endl;
               result = -1;
            }
         }
         else {
            // nothing to do for equality constraints
         }
      }

      // Complementary slackness.
      // For any constraint the product of primal slack and dual multiplier
      // must be 0.
      for (IloInt i = 0; i < linear.getSize(); ++i) {
         if ( fabs (linear[i].getUB() - linear[i].getLB()) > ZEROTOL &&
              fabs (slack[i] * rpi[i]) > ZEROTOL )
         {
            cerr << "Complementary slackness test failed for row " << i
                 << ": " << fabs (slack[i] * rpi[i]) << endl;
            result = -1;
         }
      }
      for (IloInt q = 0; q < quad.getSize(); ++q) {
         if ( fabs (quad[q].getUB() - quad[q].getLB()) > ZEROTOL &&
              fabs (qslack[q] * qpi[q]) > ZEROTOL )
         {
            cerr << "Complementary slackness test failed for quad " << q
                 << ":" << fabs (qslack[q] * qpi[q]) << endl;
            result = -1;
         }
      }
      for (IloInt j = 0; j < x.getSize(); ++j) {
         if ( x[j].getUB() < IloInfinity ) {
            double const slk = x[j].getUB() - xval[j];
            double const dual = cpi[j] < -ZEROTOL ? cpi[j] : 0.0;
            if ( fabs (slk * dual) > ZEROTOL ) {
               cerr << "Complementary slackness test failed for ub " << j
                    << ": " << fabs (slk * dual) << endl;
               result = -1;
            }
         }
         if ( x[j].getLB() > -IloInfinity ) {
            double const slk = xval[j] - x[j].getLB();
            double const dual = cpi[j] > ZEROTOL ? cpi[j] : 0.0;
            if ( fabs (slk * dual) > ZEROTOL ) {
               cerr << "Complementary slackness test failed for lb " << j
                    << ": " << fabs (slk * dual) << endl;
               result = -1;
            }
         }
      }

      // Stationarity.
      // The difference between objective function and gradient at optimal
      // solution multiplied by dual multipliers must be 0, i.e., for the
      // optimal solution x
      // 0 == c
      //      - sum(r in rows)  r'(x)*rpi[r]
      //      - sum(q in quads) q'(x)*qpi[q]
      //      - sum(c in cols)  b'(x)*cpi[c]
      // where r' and q' are the derivatives of a row or quadratic constraint,
      // x is the optimal solution and rpi[r] and qpi[q] are the dual
      // multipliers for row r and quadratic constraint q.
      // b' is the derivative of a bound constraint and cpi[c] the dual bound
      // multiplier for column c.
      IloNumArray kktsum(env, x.getSize());

      // Objective function.
      for (IloExpr::LinearIterator it = obj.getLinearIterator(); it.ok(); ++it)
         kktsum[idx(it.getVar())] = it.getCoef();

      // Linear constraints.
      // The derivative of a linear constraint ax - b (<)= 0 is just a.
      for (IloInt i = 0; i < linear.getSize(); ++i) {
         for (IloExpr::LinearIterator it = linear[i].getLinearIterator();
              it.ok(); ++it)
            kktsum[idx(it.getVar())] -= rpi[i] * it.getCoef();
      }

      // Quadratic constraints.
      // The derivative of a constraint xQx + ax - b <= 0 is
      // Qx + Q'x + a.
      for (IloInt q = 0; q < quad.getSize(); ++q) {
         for (IloExpr::LinearIterator it = quad[q].getLinearIterator();
              it.ok(); ++it)
            kktsum[idx(it.getVar())] -= qpi[q] * it.getCoef();

         for (IloExpr::QuadIterator it = quad[q].getQuadIterator();
              it.ok(); ++it)
         {
            kktsum[idx(it.getVar1())] -= qpi[q] * xval[idx(it.getVar2())] * it.getCoef();
            kktsum[idx(it.getVar2())] -= qpi[q] * xval[idx(it.getVar1())] * it.getCoef();
         }
      }

      // Bounds.
      // The derivative for lower bounds is -1 and that for upper bounds
      // is 1.
      // CPLEX already returns dj with the appropriate sign so there is
      // no need to distinguish between different bound types here.
      for (IloInt j = 0; j < x.getSize(); ++j)
         kktsum[j] -= cpi[j];

      for (IloInt j = 0; j < kktsum.getSize(); ++j) {
         if ( fabs (kktsum[j]) > ZEROTOL ) {
            cerr << "Stationarity test failed at index " << j
                 << ": " << kktsum[j] << endl;
            result = -1;
         }
      }

      if ( result == 0) {
         // KKT conditions satisfied. Dump out the optimal solutions and
         // the dual values.
         streamsize oprec = cout.precision(3);
         ios_base::fmtflags oflags = cout.setf(ios::fixed | ios::showpos);
         cout << "Optimal solution satisfies KKT conditions." << endl;
         cout << "   x[] = " << xval << endl;
         cout << " cpi[] = " << cpi << endl;
         cout << " rpi[] = " << rpi << endl;
         cout << " qpi[] = " << qpi << endl;
         cout.precision(oprec);
         cout.flags(oflags);
      }
   }
   catch (IloException& e) {
      cerr << "Concert exception caught: " << e << endl;
      result = -1;
   }
   catch (...) {
      cerr << "Unknown exception caught" << endl;
      result = -1;
   }

   env.end();

   return result;
}  // END main
