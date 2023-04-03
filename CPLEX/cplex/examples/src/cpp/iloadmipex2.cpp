// -------------------------------------------------------------- -*- C++ -*-
// File: iloadmipex2.cpp 
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
// Use the heuristic callback for optimizing a MIP problem.
//
// See iloadmipex9.cpp for an implementation using the more recent generic
// callback.
//
// To run this example, command line arguments are required.
// i.e.,   iloadmipex2   filename
//
// Example:
//     iloadmipex2  example.mps
//

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN


static void usage (const char *progname);


ILOHEURISTICCALLBACK1(Rounddown, IloNumVarArray, vars) {
   IntegerFeasibilityArray feas;
   IloNumArray             obj;
   IloNumArray             x;
   try {
      feas = IntegerFeasibilityArray(getEnv());
      obj  = IloNumArray(getEnv());
      x    = IloNumArray(getEnv());
      getFeasibilities(feas, vars);
      getObjCoefs     (obj , vars);
      getValues       (x   , vars);

      IloNum objval = getObjValue();
      IloInt cols   = vars.getSize();
      for (IloInt j = 0; j < cols; j++) {
         // Set the fractional variable to zero and update the objective value
         if ( feas[j] == Infeasible ) {
            objval -= x[j] * obj[j];
            x[j] = 0.0;
         }
      }
      setSolution(vars, x, objval);
   }
   catch (...) {
      feas.end();
      obj.end();
      x.end();
      throw;
   }
   feas.end();
   obj.end();
   x.end();
}

int
main (int argc, char **argv)
{
   IloEnv   env;
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

      cplex.use(Rounddown(env, var));

      cplex.extract(model);
      // Check model is all binary except for objective constant variable
      if ( cplex.getNbinVars() < cplex.getNcols() - 1 ) {
         cerr << "Problem contains non-binary variables, exiting." << endl;
         throw (-1);
      }
      cplex.setParam(IloCplex::Param::MIP::Strategy::Search,
                     IloCplex::Traditional);
      cplex.solve();

      IloNumArray vals(env);
      cplex.getValues(vals, var);
      env.out() << "Solution status = " << cplex.getStatus() << endl;
      env.out() << "Solution value  = " << cplex.getObjValue() << endl;
      env.out() << "Values          = " << vals << endl;
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
