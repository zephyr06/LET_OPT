// -------------------------------------------------------------- -*- C++ -*-
// File: iloqpex2.cpp
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
// iloqpex2.cpp - Reading in and optimizing a QP
//
// To run this example, command line arguments are required.
// i.e.,   iloqpex2   filename  method
// where 
//     filename is the name of the file, with .mps, .lp, or .sav extension
//     method   is the optimization method
//                 o          default
//                 p          primal simplex
//                 d          dual   simplex
//                 b          barrier without crossover
//                 n          network with dual simplex cleanup
//
// Example:
//     iloqpex2  qpex.lp  o
//

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

static void usage (const char *progname);

int
main (int argc, char **argv)
{
   IloEnv env;
   try {
      IloModel model(env);
      IloCplex cplex(env);

      if (( argc != 3 )                             ||
          ( strchr ("opdbn", argv[2][0]) == NULL )  ) {
         usage (argv[0]);
         throw(-1);
      }

      switch (argv[2][0]) {
         case 'o':
            cplex.setParam(IloCplex::Param::RootAlgorithm, IloCplex::AutoAlg);
            break;
         case 'p':
            cplex.setParam(IloCplex::Param::RootAlgorithm, IloCplex::Primal);
            break;
         case 'd':
            cplex.setParam(IloCplex::Param::RootAlgorithm, IloCplex::Dual);
            break;
         case 'b':
            cplex.setParam(IloCplex::Param::RootAlgorithm, IloCplex::Barrier);
            break;
         case 'n':
            cplex.setParam(IloCplex::Param::RootAlgorithm, IloCplex::Network);
            break;
         default:
            break;
      }

      IloObjective   obj;
      IloNumVarArray var(env);
      IloRangeArray  rng(env);
      cplex.importModel(model, argv[1], obj, var, rng);

      cplex.extract(model);
      if ( !cplex.solve() ) {
         env.error() << "Failed to optimize LP" << endl;
         throw(-1);
      }

      IloNumArray vals(env);
      cplex.getValues(vals, var);
      env.out() << "Solution status = " << cplex.getStatus() << endl;
      env.out() << "Solution value  = " << cplex.getObjValue() << endl;
      env.out() << "Solution vector = " << vals << endl;
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
   cerr << "Usage: " << progname << " filename algorithm" << endl;
   cerr << "   where filename is a file with extension " << endl;
   cerr << "      MPS, SAV, or LP (lower case is allowed)" << endl;
   cerr << "   and algorithm is one of the letters" << endl;
   cerr << "      o          default" << endl;
   cerr << "      p          primal simplex" << endl;
   cerr << "      d          dual simplex  " << endl;
   cerr << "      b          barrier       " << endl;
   cerr << "      n          network simplex" << endl;
   cerr << " Exiting..." << endl;
} // END usage
