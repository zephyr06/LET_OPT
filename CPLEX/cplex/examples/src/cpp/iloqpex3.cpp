// -------------------------------------------------------------- -*- C++ -*-
// File: iloqpex3.cpp
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
// iloqpex3.cpp - Entering and modifying a QP problem
//
// Example iloqpex3.cpp illustrates how to enter and modify a QP problem 
// by using linear quadratic expressions.

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

static void 
   createQPModel (IloModel model, IloNumVarArray x, IloRangeArray c);

static void 
   modifyQuadObjective(IloObjective obj, IloNumVarArray x) ;

static void 
   solveAndPrint(IloCplex cplex, IloNumVarArray var, char const *msg);

static void 
   printObjective (IloObjective obj);

int
main (int argc, char **argv)
{
   IloEnv   env;
   try {
      // create a QP problem
      IloModel model(env);
      IloNumVarArray var(env);
      IloRangeArray con(env);
      createQPModel (model, var, con);

      IloCplex cplex(model);
      cplex.exportModel("qp1ex3.lp");

      // solve the QP problem
      solveAndPrint(cplex, var, "Solving the QP problem ...");

      // Modify the quadratic objective function
      modifyQuadObjective(cplex.getObjective(), var);
      cplex.exportModel("qp2ex3.lp");

      // solve the modified QP problem
      solveAndPrint(cplex, var, "Solving the modified QP problem ...");
   }
   catch (IloException& e) {
      cerr << "Concert exception caught: " << e << endl;
   }
   catch (...) {
      cerr << "Unknown exception caught" << endl;
   }
   
   env.end();

   return 0;
}  // END main


// Creating a simple QP problem
static void 
createQPModel (IloModel model, IloNumVarArray x, IloRangeArray c)
{
   IloEnv env = model.getEnv();

   x.add(IloNumVar(env, 0.0, 40.0,        "x0"));
   x.add(IloNumVar(env, 0.0, IloInfinity, "x1"));
   x.add(IloNumVar(env, 0.0, IloInfinity, "x2"));
   
   // - x0 +   x1 + x2 <= 20
   //   x0 - 3*x1 + x2 <= 30
   c.add( - x[0] +     x[1] + x[2] <= 20);
   c.add(   x[0] - 3 * x[1] + x[2] <= 30);
   model.add(c);

   // minimize - x0 - x1 - x2 + x0*x0 + x1*x1 + x0*x1 + x1*x0 
   IloInt i, j, nvars = x.getSize();
   IloExpr objExpr(env);
   for (i = 0; i < nvars; ++i) {
      objExpr += -1.0*x[i];
      for (j = 0; j < nvars; ++j) {
         objExpr += 1.0*x[i]*x[j];
      }
   }
   IloObjective obj = IloMinimize(env, objExpr);
   model.add(obj);
   objExpr.end();

   // Print out the objective function
   printObjective(obj);
} // END createQPModel


// Modifying all quadratic terms x[i]*x[j] 
// in the objective function.
static void 
modifyQuadObjective(IloObjective obj, IloNumVarArray x) 
{
   IloInt i, j;
   IloInt ncols = x.getSize();

   // Note that the quadratic expression in the objective
   // is normalized: i.e., for all i != j, terms 
   // c(i,j)*x[i]*x[j] + c(j,i)*x[j]*x[i] are normalized as
   // (c(i,j) + c(j,i)) * x[i]*x[j], or 
   // (c(i,j) + c(j,i)) * x[j]*x[i].
   // Therefore you can only modify one of the terms 
   // x[i]*x[j] or x[j]*x[i]. 
   // If you modify both x[i]*x[j] and x[j]*x[i], then 
   // the second modification will overwrite the first one.
   for (i = 0; i < ncols; ++i) {
      obj.setQuadCoef(x[i], x[i], i*i);
      for (j = 0; j < i; ++j)
         obj.setQuadCoef(x[i], x[j], -2.0*(i*j));
   }

   // Print out the objective function
   printObjective(obj);     
} //END modifyQuadObjective


// Solve the current model and print results
static void 
solveAndPrint(IloCplex cplex, IloNumVarArray var, char const *msg) 
{
   IloEnv env = var.getEnv();
   env.out() << msg << endl;
  
   if ( cplex.solve() ) {
      env.out() << "Solution status = " << cplex.getStatus()   << endl;
      env.out() << "Solution value  = " << cplex.getObjValue() << endl;

      IloNumArray val(env);
      cplex.getValues(val, var);
      IloInt j, nvars = var.getSize();
      for (j = 0; j < nvars; ++j) {
         env.out() << "Variable " << j << ": Value = " << val[j] << endl;
      }
      val.end();
   }
   env.out() << endl;
} // END solveAndPrint


// Print out the objective function.
// Note that the quadratic expression in the objective
// is normalized: i.E., for all i != j, terms 
// c(i,j)*x[i]*x[j] + c(j,i)*x[j]*x[i] is normalized as
// (c(i,j) + c(j,i)) * x[i]*x[j], or 
// (c(i,j) + c(j,i)) * x[j]*x[i].
static void 
printObjective (IloObjective obj) 
{
   IloEnv env = obj.getEnv();

   env.out() << "obj: " << obj << endl;

   // Count the number of linear terms 
   // in the objective function.
   IloInt nlinterms = 0;
   for (IloExpr::LinearIterator lit = obj.getLinearIterator(); lit.ok(); ++lit) {
      ++nlinterms;
   }
   
   // Count the number of quadratic terms 
   // in the objective function.
   IloInt nquadterms = 0;
   IloInt nquaddiag  = 0;
   for (IloExpr::QuadIterator qit = obj.getQuadIterator(); qit.ok(); ++qit) {
      ++nquadterms;
      if ( qit.getVar1().getImpl() == qit.getVar2().getImpl() )
         ++nquaddiag;
   }
      
   env.out() << "number of linear terms in the objective             : " << nlinterms  << endl;
   env.out() << "number of quadratic terms in the objective          : " << nquadterms << endl;
   env.out() << "number of diagonal quadratic terms in the objective : " << nquaddiag  << endl;
   env.out() << endl;
} // END printObjective
