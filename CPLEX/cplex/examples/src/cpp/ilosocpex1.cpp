// --------------------------------------------------------------------------
// File: ilosocpex1
// Version 22.1.0
// --------------------------------------------------------------------------
// Licensed Materials - Property of IBM
// 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
// Copyright IBM Corporation 2003, 2022. All Rights Reserved.
//
// US Government Users Restricted Rights - Use, duplication or
// disclosure restricted by GSA ADP Schedule Contract with
// IBM Corp.
// --------------------------------------------------------------------------

// ilosocpex1.cpp -- Solve a second order cone program to optimality, fetch
//                   the dual values and test that the primal and dual solution
//                   vectors returned by CPLEX satisfy the KKT conditions.
//                   The problems that this code can handle are second order
//                   cone programs in standard form. A second order cone
//                   program in standard form is a problem of the following
//                   type (c' is the transpose of c):
//                     min c1'x1 + ... + cr'xr
//                       A1 x1 + ... + Ar xr = b
//                       xi in second order cone (SOC)
//                   where xi is a vector of length ni. Note that the
//                   different xi are orthogonal. The constraint "xi in SOC"
//                   for xi=(xi[1], ..., xi[ni]) is
//                       xi[1] >= |(xi[2],...,xi[ni])|
//                   where |.| denotes the Euclidean norm. In CPLEX such a
//                   constraint is formulated as
//                       -xi[1]^2 + xi[2]^2 + ... + xi[ni]^2 <= 0
//                        xi[1]                              >= 0
//                                  xi[2], ..., xi[ni] free
//                   Note that if ni = 1 then the second order cone constraint
//                   reduces to xi[1] >= 0.

#include <cmath>
#include <string>
#include <iostream>
#include <ilcplex/ilocplex.h>

ILOSTLBEGIN // import namespace std

// Tolerance for testing KKT conditions.
#define TESTTOL 1e-9
// Tolerance for barrier convergence.
#define CONVTOL 1e-9

// Marks variables that are in cone constraints but are not the cone head.
#define NOT_CONE_HEAD -1
// Marks variables thare not in any cone constraint.
#define NOT_IN_CONE   -2

// We store for each variable/ constraint its index in the array
// of variables/constraints as a user object. This makes the code in
// checkkkt() a little simpler. The idx() template function defined
// here returns that index for either a variable or a constraint.
namespace {
   template<typename T> int
   idx(T const& t) { return *static_cast<int *>(t.getObject()); }

   // NOTE: CPLEX does not provide a function to directly get the dual
   //       multipliers for second order cone constraint.
   //       Example iloqcpdual.c illustrates how the dual multipliers for a
   //       quadratic constraint can be computed from that constraint's slack.
   //       However, for SOCP we can do something simpler: we can read those
   //       multipliers directly from the dual slacks for the
   //       cone head variables. For a second order cone constraint
   //          x[1] >= |(x[2], ..., x[n])|
   //       the dual multiplier is the dual slack value for x[1]. For each
   //       column j cone[j] gives the cone in which j is the cone head
   //       variable or a negative value if j is not the head variable in
   //       any cone. So we can use this array to easily get the dual
   //       multipliers for the cone constraints.
   void getsocpconstrmultipliers(IloCplex cplex,
                                 IloNumVarArray vars, // all variables
                                 IloRangeArray rngs,  // all constraints
                                 IloNumArray pi,
                                 IloNumArray dslack = 0)
   {
      pi.clear();

      // Compute the full dual slack.
      IloNumArray dense(cplex.getEnv());
      cplex.getReducedCosts(dense, vars);
      for (IloInt i = 0; i < rngs.getSize(); ++i) {
         if ( rngs[i].getQuadIterator().ok() ) {
            IloNumVarArray nzVars(cplex.getEnv());
            IloNumArray nzVals(cplex.getEnv());
            cplex.getQCDSlack(rngs[i], nzVals, nzVars);
            for (IloInt j = 0; j < nzVars.getSize(); ++j)
               dense[idx(nzVars[j])] += nzVals[j];
            nzVals.end();
            nzVars.end();
         }
         pi.add(0);
      }

      // Find the cone head variables (the variables with negative
      // coefficient in the quadratic constraints) and pick up the
      // dual slacks for them.
      for (IloInt i = 0; i < rngs.getSize(); ++i) {
         IloExpr::QuadIterator it = rngs[i].getQuadIterator();
         while (it.ok()) {
            if ( it.getCoef() < 0 ) {
               pi[i] = dense[idx(it.getVar1())];
               break;
            }
            ++it;
         }
      }

      // Setup dual slack if the user asked for that.
      if ( dslack.getImpl() ) {
         dslack.clear();
         for (IloInt i = 0; i < dense.getSize(); ++i)
            dslack.add(dense[i]);
      }

      dense.end();
   }
}

// Test KKT conditions on the solution.
// The function returns true if the tested KKT conditions are satisfied
// and false otherwise.
// The function assumes that the model currently extracted to CPLEX is fully
// described by obj, vars and rngs.
static bool
checkkkt (IloCplex& cplex, IloObjective const& obj, IloNumVarArray const& vars,
          IloRangeArray const& rngs, IloIntArray const& cone, double tol)
{
   IloEnv env = cplex.getEnv();
   IloNumArray x(env), dslack(env);
   IloNumArray pi(env, rngs.getSize()), slack(env);

   // Read primal and dual solution information.
   cplex.getValues(x, vars);
   cplex.getSlacks(slack, rngs);

   // pi for second order cone constraints.
   getsocpconstrmultipliers(cplex, vars, rngs, pi, dslack);

   // pi for linear constraints.
   for (IloInt i = 0; i < rngs.getSize(); ++i) {
      IloRange r = rngs[i];
      if ( !r.getQuadIterator().ok() )
         pi[idx(r)] = cplex.getDual(r);
   }

   // Print out the data we just fetched.
   streamsize oprec = env.out().precision(3);
   ios_base::fmtflags oflags = env.out().setf(ios::fixed | ios::showpos);
   env.out() << "x      = [";
   for (IloInt i = 0; i < x.getSize(); ++i)
      env.out() << " " << x[i];
   env.out() << " ]" << endl;
   env.out() << "dslack = [";
   for (IloInt i = 0; i < dslack.getSize(); ++i)
      env.out() << " " << dslack[i];
   env.out() << " ]" << endl;
   env.out() << "pi     = [";
   for (IloInt i = 0; i < rngs.getSize(); ++i)
      env.out() << " " << pi[i];
   env.out() << " ]" << endl;
   env.out() << "slack  = [";
   for (IloInt i = 0; i < rngs.getSize(); ++i)
      env.out() << " " << slack[i];
   env.out() << " ]" << endl;
   env.out().precision(oprec);
   env.out().flags(oflags);

   // Test primal feasibility.
   // This example illustrates the use of dual vectors returned by CPLEX
   // to verify dual feasibility, so we do not test primal feasibility
   // here.

   // Test dual feasibility.
   // We must have
   // - for all <= constraints the respective pi value is non-negative,
   // - for all >= constraints the respective pi value is non-positive,
   // - the dslack value for all non-cone variables must be non-negative.
   // Note that we do not support ranged constraints here.
   for (IloInt i = 0; i < vars.getSize(); ++i) {
      IloNumVar v = vars[i];
      if ( cone[i] == NOT_IN_CONE && dslack[i] < -tol ) {
         env.error() << "Dual multiplier for " << v << " is not feasible: "
                     << dslack[i] << endl;
         return false;
      }
   }
   for (IloInt i = 0; i < rngs.getSize(); ++i) {
      IloRange r = rngs[i];
      if ( fabs (r.getLB() - r.getUB()) <= tol ) {
         // Nothing to check for equality constraints.
      }
      else if ( r.getLB() > -IloInfinity && pi[i] > tol ) {
         env.error() << "Dual multiplier " << pi[i] << " for >= constraint"
                     << endl << r << endl
                     << "not feasible"
                     << endl;
         return false;
      }
      else if ( r.getUB() < IloInfinity && pi[i] < -tol ) {
         env.error() << "Dual multiplier " << pi[i] << " for <= constraint"
                     << endl << r << endl
                     << "not feasible"
                     << endl;
         return false;
      }
   }

   // Test complementary slackness.
   // For each constraint either the constraint must have zero slack or
   // the dual multiplier for the constraint must be 0. We must also
   // consider the special case in which a variable is not explicitly
   // contained in a second order cone constraint.
   for (IloInt i = 0; i < vars.getSize(); ++i) {
      if ( cone[i] == NOT_IN_CONE ) {
         if ( fabs(x[i]) > tol && dslack[i] > tol ) {
            env.error() << "Invalid complementary slackness for " << vars[i]
                        << ":" << endl
                        << " " << x[i] << " and " << dslack[i]
                        << endl;
            return false;
         }
      }
   }
   for (IloInt i = 0; i < rngs.getSize(); ++i) {
      if ( fabs(slack[i]) > tol && fabs(pi[i]) > tol ) {
         env.error() << "Invalid complementary slackness for "
                     << endl << rngs[i] << ":" << endl
                     << " " << slack[i] << " and " << pi[i]
                     << endl;
         return false;
      }
   }

   // Test stationarity.
   // We must have
   //  c - g[i]'(X)*pi[i] = 0
   // where c is the objective function, g[i] is the i-th constraint of the
   // problem, g[i]'(x) is the derivate of g[i] with respect to x and X is the
   // optimal solution.
   // We need to distinguish the following cases:
   // - linear constraints g(x) = ax - b. The derivative of such a
   //   constraint is g'(x) = a.
   // - second order constraints g(x[1],...,x[n]) = -x[1] + |(x[2],...,x[n])|
   //   the derivative of such a constraint is
   //     g'(x) = (-1, x[2]/|(x[2],...,x[n])|, ..., x[n]/|(x[2],...,x[n])|
   //   (here |.| denotes the Euclidean norm).
   // - bound constraints g(x) = -x for variables that are not explicitly
   //   contained in any second order cone constraint. The derivative for
   //   such a constraint is g'(x) = -1.
   // Note that it may happen that the derivative of a second order cone
   // constraint is not defined at the optimal solution X (this happens if
   // X=0). In this case we just skip the stationarity test.
   IloNumArray sum(env, vars.getSize());
   for (IloExpr::LinearIterator it = obj.getLinearIterator(); it.ok(); ++it)
      sum[idx(it.getVar())] = it.getCoef();

   for (IloInt i = 0; i < vars.getSize(); ++i) {
      if ( cone[i] == NOT_IN_CONE )
         sum[i] -= dslack[i];
   }
   for (IloInt i = 0; i < rngs.getSize(); ++i) {
      IloRange r = rngs[i];
      if ( r.getQuadIterator().ok() ) {
         // Quadratic (second order cone) constraint.
         IloNum norm = 0.0;
         for (IloExpr::QuadIterator q = r.getQuadIterator(); q.ok(); ++q) {
            if ( q.getCoef() > 0 )
               norm += x[idx(q.getVar1())] * x[idx(q.getVar1())];
         }
         norm = sqrt(norm);
         if ( fabs(norm) <= tol ) {
            // Derivative is not defined. Skip test.
            env.warning() << "Cannot test stationarity at non-differentiable point."
                          << endl;
            return true;
         }
         else {
            for (IloExpr::QuadIterator q = r.getQuadIterator(); q.ok(); ++q) {
               if ( q.getCoef() < 0 )
                  sum[idx(q.getVar1())] -= pi[i];
               else
                  sum[idx(q.getVar1())] += pi[i] * x[idx(q.getVar1())] / norm;
            }
         }
      }
      else {
         // Linear constraint.
         for (IloExpr::LinearIterator l = r.getLinearIterator(); l.ok(); ++l)
            sum[idx(l.getVar())] -= pi[i] * l.getCoef();
      }
   }

   // Now test that all elements in sum[] are 0.
   for (IloInt i = 0; i < vars.getSize(); ++i) {
      if ( fabs(sum[i]) > tol ) {
         env.error() << "Invalid stationarity " << sum[i] << " for "
                     << vars[i] << endl;
         return false;
      }
   }

   return true;   
}

// This function creates the following model:
//   Minimize
//    obj: x1 + x2 + x3 + x4 + x5 + x6
//   Subject To
//    c1: x1 + x2      + x5      = 8
//    c2:           x3 + x5 + x6 = 10
//    q1: [ -x1^2 + x2^2 + x3^2 ] <= 0
//    q2: [ -x4^2 + x5^2 ] <= 0
//   Bounds
//    x2 Free
//    x3 Free
//    x5 Free
//   End
// which is a second order cone program in standard form.
// The function returns objective, variables and constraints in the
// values obj, vars and rngs.
// The function also sets up cone so that for a column j we have
// cone[j] >= 0               Column j is in a cone constraint and is the
//                            cone's head variable.
// cone[j] == NOT_CONE_HEAD   Column j is in a cone constraint but is
//                            not the cone's head variable..
// cone[j] == NOT_IN_CONE     Column j is not contained in any cone constraint.
static void
createmodel (IloModel& model, IloObjective &obj, IloNumVarArray &vars,
             IloRangeArray &rngs, IloIntArray& cone)
{
   // The indices we assign as user objects to the modeling objects.
   // We define them as static data so that we don't have to worry about
   // dynamic memory allocation/leakage.
   static int indices[] = { 0, 1, 2, 3, 4, 5, 6 };

   IloEnv env = model.getEnv();

   // Create variables.
   IloNumVar x1(env,            0, IloInfinity, "x1");
   IloNumVar x2(env, -IloInfinity, IloInfinity, "x2");
   IloNumVar x3(env, -IloInfinity, IloInfinity, "x3");
   IloNumVar x4(env,            0, IloInfinity, "x4");
   IloNumVar x5(env, -IloInfinity, IloInfinity, "x5");
   IloNumVar x6(env,            0, IloInfinity, "x6");

   // Create objective function and immediately store it in return value.
   obj = IloMinimize(env, x1 + x2 + x3 + x4 + x5 + x6);

   // Create constraints.
   IloRange c1(env, 8,  x1 + x2      + x5,       8, "c1");
   IloRange c2(env, 10,           x3 + x5 + x6, 10, "c2");
   IloRange q1(env, -IloInfinity, -x1*x1 + x2*x2 + x3*x3, 0, "q1");
   cone.add(2);             // x1, cone head of constraint at index 2
   cone.add(NOT_CONE_HEAD); // x2
   cone.add(NOT_CONE_HEAD); // x3
   IloRange q2(env, -IloInfinity, -x4*x4 + x5*x5, 0, "q2");
   cone.add(3);             // x4, cone head of constraint at index 3
   cone.add(NOT_CONE_HEAD); // x5

   cone.add(NOT_IN_CONE);   // x6

   // Setup model.
   model.add(obj);
   model.add(obj);
   model.add(c1);
   model.add(c2);
   model.add(q1);
   model.add(q2);

   // Setup return values.
   vars.add(x1);
   vars.add(x2);
   vars.add(x3);
   vars.add(x4);
   vars.add(x5);
   vars.add(x6);

   rngs.add(c1);
   rngs.add(c2);
   rngs.add(q1);
   rngs.add(q2);

   // We set the user object for each modeling object to its index in the
   // respective array. This makes the code in checkkkt a little simpler.
   for (IloInt i = 0; i < vars.getSize(); ++i)
      vars[i].setObject(&indices[i]);
   for (IloInt i = 0; i < rngs.getSize(); ++i)
      rngs[i].setObject(&indices[i]);
}

int
main (void)
{
   IloEnv env;
   int retval = -1;

   try {
      // Create the model.
      IloModel model(env);
      IloCplex cplex(env);
      IloObjective obj(env);
      IloNumVarArray vars(env);
      IloRangeArray rngs(env);
      IloIntArray cone(env);
      createmodel(model, obj, vars, rngs, cone);

      // Extract model.
      cplex.extract(model);

      // Solve the problem. If we cannot find an _optimal_ solution then
      // there is no point in checking the KKT conditions and we throw an
      // exception.
      cplex.setParam(IloCplex::Param::Barrier::QCPConvergeTol, CONVTOL);
      if ( !cplex.solve() || cplex.getStatus() != IloAlgorithm::Optimal )
         throw string("Failed to solve problem to optimality");

      // Test the KKT conditions on the solution.
      if ( !checkkkt (cplex, obj, vars, rngs, cone, TESTTOL) ) {
         env.error() << "Testing of KKT conditions failed." << endl;
      }
      else {
         env.out() << "Solution status: " << cplex.getStatus() << endl;
         env.out() << "KKT conditions are satisfied." << endl;
         retval = 0;
      }

      env.end();
   } catch (IloException &e) {
      cerr << "IloException: " << e << endl;
      if (env.getImpl())
         env.end();
      ::abort();
   } catch (string& e) {
      cerr << e << endl;
      if (env.getImpl())
         env.end();
      ::abort();
   }
   return retval;
}
