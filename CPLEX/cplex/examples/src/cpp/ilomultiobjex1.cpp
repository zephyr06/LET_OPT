// -------------------------------------------------------------- -*- C++ -*-
// File: ilomultiobjex1.cpp
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
// Reading and optimizing a multiobjective problem.
//
// Demonstrates specifying parameter sets and accessing the results of
// the various objectives.
//
// See the usage message for more details.

#include <ilcplex/ilocplex.h>

using std::cout;
using std::cerr;
using std::endl;


// Print a usage message to stderr and abort.
void usage(const char* name) {
   cerr << endl;
   cerr << "Usage: " << name << " <filename> [<prmfile1> <prmfile1> ...]" << endl;
   cerr << endl;
   cerr << "   The files prmfile1, ... are applied as paramsets for" << endl;
   cerr << "   the different objective priorities." << endl;
   cerr << endl;
   exit(2);
}


int
main(int argc, char **argv)
{
   IloEnv env;

   // Handle command line arguments.
   if (argc < 2)
      usage(argv[0]);

   try {
      IloModel model(env);

      IloCplex cpx(env);
      cpx.importModel(model, argv[1]);
      cpx.extract(model);

      IloBool result;
      if (argc > 2) {
         IloArray<IloCplex::ParameterSet> paramsets(env, argc - 2);
         // Read in all of the parameter sets.
         for (int i = 0; i < (argc - 2); ++i) {
            IloCplex::ParameterSet ps(env);
            ps.read(argv[i + 2]);
            paramsets[i] = ps;
         }
         result = cpx.solve(paramsets);
      }
      else {
         result = cpx.solve();
      }

      if (!result) {
         cerr << "CPLEX status = " << cpx.getCplexStatus() << endl;
         throw IloAlgorithm::Exception("Failed to optimize.");
      }

      // Print the solution status.
      cpx.out() << endl;
      cpx.out() << "Solution status = " << cpx.getStatus() << endl;
      cpx.out() << endl;

      // Print the values of the various objetives.
      cpx.out() << "Objective values..." << endl;
      for (int i = 0; i < cpx.getMultiObjNsolves(); ++i) {
         cpx.out() << "Objective priority "
                   << cpx.getMultiObjInfo(IloCplex::MultiObjPriority, i)
                   << " value = "
                   << cpx.getMultiObjInfo(IloCplex::MultiObjObjValue, i)
                   << " status = "
                   << cpx.getMultiObjSubStatus(i)
                   << " CPLEX status = "
                   << cpx.getMultiObjSubCplexStatus(i)
                   << endl;
      }
   }
   catch (IloException& ex) {
      cerr << "Concert exception caught" << endl;
      throw;
   }
   catch (...) {
      cerr << "Unknown exception caught" << endl;
      throw;
   }

   env.end();

   return 0;
}
