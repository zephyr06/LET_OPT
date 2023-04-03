// -------------------------------------------------------------- -*- C++ -*-
// File: iloconflictex1.cpp
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
// Read a problem and use the conflict refiner.
//
// See the usage message below for more details.

#include <sstream>
#include <ilcplex/ilocplex.h>

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::string;
using std::stringstream;


// Display a usage message and exit.
static void
usage(const char *progname)
{
   cerr << "Usage: " << progname << " filename" << endl
        << "  filename   Name of a file, with .mps, .lp, or .sav"    << endl
        << "             extension, and a possible, additional .gz"    << endl
        << "             extension"    << endl
      ;
   exit(2);
}

// Invoke the conflict refiner on a model and display the results.
int
main(int argc, char **argv)
{
   IloEnv env;
   try {
      if (argc != 2)
         usage(argv[0]);

      // Create the model and CPLEX objects.
      IloModel model(env);
      IloCplex cplex(env);

      // Read model from file with name args[0] into cplex optimizer object.
      cplex.importModel(model, argv[1]);
      cplex.extract(model);

      // A list of constraints to be considered by the conflict refiner.
      IloConstraintArray constraints(env);

      // Loop over all objects in the model and gather constraints.
      for (IloModel::Iterator it(model); it.ok(); ++it) {
         IloExtractable ext = *it;
         if (ext.isVariable()) {
            IloNumVar v = ext.asVariable();
            // Add variable bounds to the constraints array.
            constraints.add(IloBound(v, IloBound::Lower));
            constraints.add(IloBound(v, IloBound::Upper));
         }
         else if (ext.isConstraint()) {
            IloConstraint c = ext.asConstraint();
            constraints.add(c);
         }
      }

      // Define preferences for the constraints. Here, we give all
      // constraints a preference of 1.0, so they will be treated
      // equally.
      IloNumArray prefs(env, constraints.getSize());
      for (int i = 0; i < prefs.getSize(); ++i)
         prefs[i] = 1.0;

      // Run the conflict refiner. As opposed to letting the conflict
      // refiner run to completion (as is done here), the user can set
      // a resource limit (e.g., a time limit, an iteration limit, or
      // node limit) and still potentially get a "possible" conflict.
      if (cplex.refineConflict(constraints, prefs)) {
         // Display the solution status.
         IloCplex::CplexStatus status = cplex.getCplexStatus();
         cout << "Solution status = " << status << " (" <<
            static_cast<int>(status) << ")" << endl;

         // Get the conflict status for the constraints that were specified.
         IloCplex::ConflictStatusArray conflict = cplex.getConflict(constraints);

         // Print constraints that participate in the conflict.
         cout << "Conflict:" << endl;

         for (int i = 0; i < constraints.getSize(); ++i) {
            if (conflict[i] == IloCplex::ConflictMember ||
                conflict[i] == IloCplex::ConflictPossibleMember) {
               cout << "  " << constraints[i] << endl;
            }
         }

         cout << endl;

         // Write the identified conflict in the LP format.
         const char *confFile = "iloconflictex1.lp";
         cout << "Writing conflict file to '" << confFile << "'...." << endl;
         cplex.writeConflict(confFile);

         // Display the entire conflict subproblem.
         string line;
         ifstream file(confFile);
         if (file.is_open()) {
            while (getline(file, line)) {
               cout << line << endl;
            }
         }
         file.close();
      }
      else {
         cout << "A conflict was not identified." << endl;
         cout << "Exiting...." << endl;
         return 0;
      }

   }
   catch(IloException& e) {
      cerr << "Concert exception caught" << endl;
      throw;
   }
   catch(...) {
      cerr << "Unknown exception caught" << endl;
      throw;
   }
   env.end();
   return 0;
}

