// -------------------------------------------------------------- -*- C++ -*-
// File: iloadmipex5.cpp
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
// Solve a facility location problem with cut callbacks or lazy constraints.
//
// See iloadmipex8.cpp for an implementation using the more recent generic
// callback.
//
// Given a set of locations J and a set of clients C, the following model is
// solved:
//
//  Minimize
//   sum(j in J) fixedCost[j]*used[j] +
//   sum(j in J)sum(c in C) cost[c][j]*supply[c][j]
//  Subject to
//   sum(j in J) supply[c][j] == 1                    for all c in C
//   sum(c in C) supply[c][j] <= (|C| - 1) * used[j]  for all j in J
//               supply[c][j] in {0, 1}               for all c in C, j in J
//                    used[j] in {0, 1}               for all j in J
//
// In addition to the constraints stated above, the code also separates
// a disaggregated version of the capacity constraints (see comments for the
// cut callback) to improve performance.
//
// Optionally, the capacity constraints can be separated from a lazy
// constraint callback instead of being stated as part of the initial model.
//
// See the usage message for how to switch between these options.

#include <sstream>
#include <ilcplex/ilocplex.h>


using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::stringstream;

// Some shortcut typedefs.
typedef IloArray<IloNumArray>    FloatMatrix;
typedef IloArray<IloNumVarArray> NumVarMatrix;

#define EPS 1e-6 // epsilon used for violation of cuts

// Separate the disaggregated capacity constraints.
// In the model we have for each location j the constraint
//    sum(c in clients) supply[c][j] <= (nbClients-1) * used[j]
// Clearly, a client can only be serviced from a location that is used,
// so we also have a constraint
//    supply[c][j] <= used[j]
// that must be satisfied by every feasible solution. These constraints tend
// to be violated in LP relaxation. In this callback we separate them.
ILOUSERCUTCALLBACK2(Disaggregated, IloNumVarArray, used, NumVarMatrix, supply) {
   IloInt const nbLocations = used.getSize();
   IloInt const nbClients = supply.getSize();

   // For each j and c check whether in the current solution (obtained by
   // calls to getValue()) we have supply[c][j]>used[j]. If so, then we have
   // found a violated constraint and add it as a cut.
   for (IloInt j = 0; j < nbLocations; ++j) {
      for (IloInt c = 0; c < nbClients; ++c) {
         IloNum const s = getValue(supply[c][j]);
         IloNum const o = getValue(used[j]);
         if ( s > o + EPS) {
            cout << "Adding: " << supply[c][j].getName() << " <= "
                 << used[j].getName() << " [" << s << " > " << o << "]" << endl;
            add(supply[c][j] <= used[j]).end();
         }
      }
   }
}

// Variant of the Disaggregated callback that does not look for violated
// cuts dynamically. Instead it uses a static table of cuts and scans this
// table for violated cuts.
ILOUSERCUTCALLBACK1(CutsFromTable, IloRangeArray, cuts) {
   for (IloInt i = 0; i < cuts.getSize(); ++i) {
      IloRange& cut = cuts[i];
      IloNum const lhs = getValue(cut.getExpr());
      if (lhs < cut.getLB() - EPS || lhs > cut.getUB() + EPS ) {
         cout << "Adding: " << cut << " [lhs = " << lhs << "]" << endl;
         add(cut);
      }
   }
}

// Lazy constraint callback to enforce the capacity constraints.
// If used then the callback is invoked for every integer feasible solution
// CPLEX finds. For each location j it checks whether constraint
//    sum(c in C) supply[c][j] <= (|C| - 1) * used[j]
// is satisfied. If not then it adds the violated constraint as lazy constraint.
ILOLAZYCONSTRAINTCALLBACK2(LazyCallback, IloNumVarArray, used, NumVarMatrix, supply) {
   IloInt const nbLocations = used.getSize();
   IloInt const nbClients = supply.getSize();
   for (IloInt j = 0; j < nbLocations; ++j) {
      IloNum isUsed = getValue(used[j]);
      IloNum served = 0.0; // Number of clients currently served from j
      for (IloInt c = 0; c < nbClients; ++c)
         served += getValue(supply[c][j]);
      if ( served > (nbClients - 1.0) * isUsed + EPS ) {
         IloNumExpr sum = IloExpr(getEnv());
         for (IloInt c = 0; c < nbClients; ++c)
            sum += supply[c][j];
         sum -= (nbClients - 1) * used[j];
         cout << "Adding lazy capacity constraint " << sum << " <= 0" << endl;
         add(sum <= 0.0).end();
         sum.end();
      }
   }
}

static void usage(const char *progname);

int
main(int argc, char **argv)
{
   IloEnv env;
   try {
      // Set default arguments and parse command line.
      char const *datadir = "../../../examples/data";
      bool fromTable = false;
      bool lazy = false;
      bool useCallback = true;

      for (int i = 1; i < argc; ++i) {
         if ( ::strncmp(argv[i], "-data=", 6) == 0 )
            datadir = argv[i] + 6;
         else if ( ::strcmp(argv[i], "-table") == 0 )
            fromTable = true;
         else if ( ::strcmp(argv[i], "-lazy") == 0 )
            lazy = true;
         else if ( ::strcmp(argv[i], "-no-cuts") == 0 )
            useCallback = false;
         else {
            cerr << "Unknown argument " << argv[i] << endl;
            usage(argv[0]);
         }
      }

      // Setup input file name and open the file.
      stringstream filename;
      filename << datadir << "/" << "facility.dat";
      ifstream file(filename.str().c_str());
      if (!file) {
         cerr << "ERROR: could not open file '" << filename.str()
              << "' for reading" << endl;
         usage(argv[0]);
      }

      // Input data.
      IloNumArray fixedCost(env);
      FloatMatrix cost(env);
      file >> fixedCost >> cost;
      IloInt nbLocations = fixedCost.getSize();
      IloInt nbClients   = cost.getSize();

      // Create variables.
      // - used[j]      If location j is used.
      // - supply[c][j] Amount shipped from location j to client c. This is a
      //                number in [0,1] and specifies the percentage of c's
      //                demand that is served from location i.
      IloNumVarArray used(env, nbLocations, 0, 1, ILOINT);
      used.setNames("used");
      NumVarMatrix supply(env, nbClients);
      for (IloInt c = 0; c < nbClients; ++c) {
         supply[c] = IloNumVarArray(env, nbLocations, 0, 1, ILOINT);
         std::stringstream s;
         s << "supply(" << c << ")";
         supply[c].setNames(s.str().c_str());
      }

      IloModel model(env);
      // The supply for each client must sum to 1, i.e., the demand of each
      // client must be met.
      for (IloInt c = 0; c < nbClients; ++c)
         model.add(IloSum(supply[c]) == 1);

      // Capacity constraint for each location. We just require that a single
      // location cannot serve all clients, that is, the capacity of each
      // location is nbClients-1. This makes the model a little harder to
      // solve and allows us to separate more cuts.
      if ( !lazy ) {
         for (IloInt j = 0; j < nbLocations; ++j) {
            IloExpr v(env);
            for (IloInt c = 0; c < nbClients; ++c)
               v += supply[c][j];
            model.add(v <= (nbClients - 1) * used[j]);
            v.end();
         }
      }

      // Objective function. We have the fixed cost for useding a location
      // and the cost proportional to the amount that is shipped from a
      // location.
      IloExpr obj = IloScalProd(fixedCost, used);
      for (IloInt c = 0; c < nbClients; ++c) {
         obj += IloScalProd(cost[c], supply[c]);
      }
      model.add(IloMinimize(env, obj));
      obj.end();

      IloCplex cplex(env);
      cplex.extract(model);

      // Tweak some CPLEX parameters so that CPLEX has a harder time to
      // solve the model and our cut separators can actually kick in.
      cplex.setParam(IloCplex::Param::Threads, 1);
      cplex.setParam(IloCplex::Param::MIP::Strategy::HeuristicFreq, -1);
      cplex.setParam(IloCplex::Param::MIP::Cuts::MIRCut, -1);
      cplex.setParam(IloCplex::Param::MIP::Cuts::Implied, -1);
      cplex.setParam(IloCplex::Param::MIP::Cuts::Gomory, -1);
      cplex.setParam(IloCplex::Param::MIP::Cuts::FlowCovers, -1);
      cplex.setParam(IloCplex::Param::MIP::Cuts::PathCut, -1);
      cplex.setParam(IloCplex::Param::MIP::Cuts::LiftProj, -1);
      cplex.setParam(IloCplex::Param::MIP::Cuts::ZeroHalfCut, -1);
      cplex.setParam(IloCplex::Param::MIP::Cuts::Cliques, -1);
      cplex.setParam(IloCplex::Param::MIP::Cuts::Covers, -1);

      if ( useCallback ) {
         if ( fromTable ) {
            // Generate all disaggregated constraints and put them into a
            // table that is scanned by the callback.
            IloRangeArray cuts(env);
            for (IloInt j = 0; j < nbLocations; ++j)
               for (IloInt c = 0; c < nbClients; ++c)
                  cuts.add(supply[c][j] - used[j] <= 0.0);
            cplex.use(CutsFromTable(env, cuts));
         }
         else {
            cplex.use(Disaggregated(env, used, supply));
         }
      }

      if ( lazy )
         cplex.use(LazyCallback(env, used, supply));

      if ( !cplex.solve() )
         throw IloAlgorithm::Exception("No feasible solution found");

      IloNum tolerance = cplex.getParam(
         IloCplex::Param::MIP::Tolerances::Integrality);

      cout << "Solution status:                   " << cplex.getStatus() << endl;
      cout << "Nodes processed:                   " << cplex.getNnodes() << endl;
      cout << "Active user cuts/lazy constraints: " << cplex.getNcuts(IloCplex::CutUser) << endl;
      cout << "Optimal value:                     " << cplex.getObjValue() << endl;
      for (IloInt j = 0; j < nbLocations; j++) {
         if (cplex.getValue(used[j]) >= 1 - tolerance) {
            cout << "Facility " << j << " is used, it serves clients";
            for (IloInt c = 0; c < nbClients; ++c) {
               if (cplex.getValue(supply[c][j]) >= 1 - tolerance)
                  cout << " " << c;
            }
            cout << endl; 
         }
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

static void usage(const char *progname)
{
   cerr << "Usage: " << progname << "[options...]" << endl
        << " By default, a user cut callback is used to dynamically"    << endl
        << " separate constraints."  << endl << endl
        << " Supported options are:" << endl
        << "  -table       Instead of the default behavior, use a"      << endl
        << "               static table that holds all cuts and"        << endl
        << "               scan that table for violated cuts."          << endl
        << "  -no-cuts     Do not separate any cuts."                   << endl
        << "  -lazy        Do not include capacity constraints in the"  << endl
        << "               model. Instead, separate them from a lazy"   << endl
        << "               constraint callback."                        << endl
        << "  -data=<dir>  Specify the directory in which the data"     << endl
        << "               file facility.dat is located."               << endl
      ;
   exit(2);
}
