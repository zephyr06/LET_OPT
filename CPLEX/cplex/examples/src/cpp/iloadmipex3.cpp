// -------------------------------------------------------------- -*- C++ -*-
// File: iloadmipex3.cpp
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
// iloadmipex3.cpp -  Using the branch callback for optimizing a MIP
//                    problem with Special Ordered Sets Type 1, with
//                    all the variables binary
//
// To run this example, command line arguments are required.
// i.e.,   iloadmipex3   filename
//
// Example:
//     iloadmipex3  example.mps
//

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

#define EPS 1.0e-4

static void usage (const char *progname);


ILOBRANCHCALLBACK1(SOSbranch, IloSOS1Array, sos) {
   IloNumArray    x;
   IloNumVarArray var;

   try {
      IloInt i;
      x   = IloNumArray(getEnv());
      var = IloNumVarArray(getEnv());
      IloNum bestx = EPS;
      IloInt besti = -1;
      IloInt bestj = -1;
      IloInt num = sos.getSize();

      for (i = 0; i < num; i++) {
         if ( getFeasibility(sos[i]) == Infeasible ) {
            var.clear();
            sos[i].getVariables(var);
            getValues(x, var);
            IloInt n = var.getSize();
            for (IloInt j = 0; j < n; j++) {
               IloNum inf = IloAbs(x[j] - IloRound(x[j]));
               if ( inf > bestx ) {
                  bestx = inf;
                  besti = i;
                  bestj = j;
               }
            }
         }
      }

      if ( besti >= 0 ) {
         IloCplex::BranchDirectionArray dir;
         IloNumArray                    val;
         try {
            dir = IloCplex::BranchDirectionArray(getEnv());
            val = IloNumArray(getEnv());
            var.clear();
            sos[besti].getVariables(var);
            IloInt n = var.getSize();
            for (IloInt j = 0; j < n; j++) {
               if ( j != bestj ) {
                  dir.add(IloCplex::BranchDown);
                  val.add(0.0);
               } else {
                  dir.add(IloCplex::BranchUp);
                  val.add(1.0);
               }
            }
            makeBranch(var,        val, dir,                  getObjValue());
            makeBranch(var[bestj], 0.0, IloCplex::BranchDown, getObjValue());
         }
         catch (...) {
            dir.end();
            val.end();
            throw;
         }
         dir.end();
         val.end();
      }
   }
   catch (...) {
      var.end();
      x.end();
      throw;
   }

   var.end();
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
      IloSOS1Array   sos1(env);
      IloSOS2Array   sos2(env);
      cplex.importModel(model, argv[1], obj, var, rng, sos1, sos2);

      cplex.use(SOSbranch(env, sos1));
      cplex.setParam(IloCplex::Param::MIP::Strategy::Search,
                     IloCplex::Traditional);

      cplex.extract(model);
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
