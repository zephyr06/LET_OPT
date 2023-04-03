// -------------------------------------------------------------- -*- C++ -*-
// File: ilolpex3.cpp
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
// ilolpex3.cpp, example of adding constraints to solve a problem
//
//   minimize  c*x
//   subject to  Hx = d
//               Ax = b
//               l <= x <= u
//   where
//
//   H = (  0  0  0  0  0  0  0 -1 -1 -1  0  0 )  d = ( -1 )
//       (  1  0  0  0  0  1  0  1  0  0  0  0 )      (  4 )
//       (  0  1  0  1  0  0  1  0  1  0  0  0 )      (  1 )
//       (  0  0  1  0  1  0  0  0  0  1  0  0 )      (  1 )
//       (  0  0  0  0  0 -1 -1  0  0  0 -1  1 )      ( -2 )
//       (  0  0  0 -1 -1  0  1  0  0  0  1  0 )      ( -2 )
//       ( -1 -1 -1  0  0  0  0  0  0  0  0 -1 )      ( -1 )
//
//   A = (  0  0  0  0  0  0  0  0  0  0  2  5 )  b = (  2 )
//       (  1  0  1  0  0  1  0  0  0  0  0  0 )      (  3 )
//
//   c = (  1  1  1  1  1  1  1  0  0  0  2  2 )
//   l = (  0  0  0  0  0  0  0  0  0  0  0  0 )
//   u = ( 50 50 50 50 50 50 50 50 50 50 50 50 )
//
//
//  Treat the constraints with A as the complicating constraints, and
//  the constraints with H as the "simple" problem.
//  
//  The idea is to solve the simple problem first, and then add the
//  constraints for the complicating constraints, and solve with dual.

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

int
main()
{
   IloEnv   env;
   try {
      IloModel model(env, "chvatal");

      IloNumVarArray x(env, 12, 0.0, 50.0);
      model.add(IloMinimize(env, x[0] + x[1] + x[2] + x[3] + x[4] +
                                 x[5] + x[6] + 2*x[10] + 2*x[11] ));

      model.add(                                   -x[7]-x[8]-x[9]          
         == -1);
      model.add( x[0]                    +x[5]     +x[7]
         ==  4);
      model.add(      x[1]     +x[3]          +x[6]     +x[8]                 
         ==  1);
      model.add(           x[2]     +x[4]                    +x[9]            
         ==  1);
      model.add(                         -x[5]-x[6]               -x[10]+x[11]
         == -2);
      model.add(               -x[3]-x[4]                         +x[10]      
         == -2);
      model.add(-x[0]-x[1]-x[2]                                         -x[11]
         == -1);

      IloCplex cplex(model);
      cplex.setParam(IloCplex::Param::Simplex::Display, 2);
      cplex.setParam(IloCplex::Param::RootAlgorithm, IloCplex::Network);
      cplex.solve();
      cplex.out() << "After network optimization, objective is "
                  << cplex.getObjValue() << endl;

      model.add(2*x[10] + 5*x[11] == 2);
      model.add(  x[0] + x[2] + x[5] == 3);

      cplex.setParam(IloCplex::Param::RootAlgorithm, IloCplex::Dual);
      cplex.solve();

      IloNumArray vals(env);
      cplex.getValues(vals, x);
      cplex.out() << "Solution status " << cplex.getStatus() << endl;
      cplex.out() << "Objective value " << cplex.getObjValue() << endl;
      cplex.out() << "Solution is: " << vals << endl;

      cplex.exportModel("lpex3.lp");
   }
   catch (IloException& e) {
      cerr << "Concert exception caught: " << e << endl;
   }
   catch (...) {
      cerr << "Unknown exception caught" << endl;
   }

   env.end();
   return 0;
} // END main
