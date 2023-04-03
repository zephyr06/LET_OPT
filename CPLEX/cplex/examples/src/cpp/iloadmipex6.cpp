// -------------------------------------------------------------- -*- C++ -*-
// File: iloadmipex6.cpp
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
// iloadmipex6.cpp -  Start the solution of the root relaxation in a
//                    MIP model from an existing primal solution 
//
// To run this example, command line arguments are required.
// i.e.,   iloadmipex6   filename
//
// Example:
//     iloadmipex6  example.mps
//

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

static void usage (const char *progname);


ILOSOLVECALLBACK3(UserSolve, IloNumVarArray, vars,
                             IloNumArray, x,
                             IloBool, done) {
   if ( !done ) {
      setStart(x, vars, 0, 0);
      solve(IloCplex::Primal);
      if ( getStatus() == IloAlgorithm::Optimal ) useSolution();
      done = IloTrue;
   }
}


void solveRelaxed(IloModel mdl, IloNumVarArray vars, IloNumArray rel) {
   IloEnv env = mdl.getEnv();
   IloModel relax(env);
   relax.add(mdl);
   relax.add(IloConversion(env, vars, ILOFLOAT));
   IloCplex cplex(relax);
   cplex.solve();
   env.out() << "Solution status = " << cplex.getStatus() << endl;
   cplex.getValues(rel, vars);
}


int
main (int argc, char **argv)
{
   IloEnv env;
   try {
      IloModel model(env);
      IloCplex cplex(env);

      if ( argc != 2 ) {
         usage (argv[0]);
         throw(-1);
      }

      IloObjective   obj;
      IloNumVarArray var(env);
      IloRangeArray  rng(env);
      cplex.importModel(model, argv[1], obj, var, rng);

      IloNumArray relaxed(env);
      solveRelaxed(model, var, relaxed);
      cplex.use(UserSolve(env, var, relaxed, IloFalse));
      cplex.setParam(IloCplex::Param::MIP::Strategy::Search,
                     IloCplex::Traditional);

      cplex.extract(model);
      cplex.solve();

      IloNumArray vals(env);
      cplex.getValues(vals, var);
      env.out() << "Solution status = " << cplex.getStatus() << endl;
      env.out() << "Solution value  = " << cplex.getObjValue() << endl;
      env.out() << "Values          = " << vals << endl;
      env.out() << "Iterations      = " << cplex.getNiterations() << endl;
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


static void usage (const char *progname)
{
   cerr << "Usage: " << progname << " filename" << endl;
   cerr << "   where filename is a file with extension " << endl;
   cerr << "      MPS, SAV, or LP (lower case is allowed)" << endl;
   cerr << " Exiting..." << endl;
} // END usage
