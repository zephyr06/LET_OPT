// -------------------------------------------------------------- -*- C++ -*-
// File: iloadmipex8.cpp
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
// Solve a capacitated facility location problem with cutting planes using
// the new generic callback interface.
//
// See iloadmipex5.cpp for an implementation using legacy callbacks.
//
// We are given a set of candidate locations J and a set of clients C.
// Facilities should be opened in some of the candidate location so that the
// clients' demand can be served. The facility location problem consists of
// deciding in which locations facilities should be opened and assigning each
// client to a facility at a minimum cost.
//
// A fixed cost is associated to opening a facility, and a linear cost is
// associated to the demand supplied from a given facility to a client.
//
// Furthermore, each facility has a capacity and can only serve |C| - 1
// clients. Note that in the variant of the problem considered here, each
// client is served by only one facility.
//
// The problem is formulated as a mixed integer linear program using binary
// variables: used[j], for all locations j in J, indicating if a facility is
// opened in location j; supply[c][j], for each client c in C and facility j
// in J, indicating the demand of client c supplied from facility j.
//
// Thus, the following model formulates the facility location problem:
//
//  Minimize sum(j in J) fixedCost[j] * used[j] +
//           sum(j in J) sum(c in C) cost[c][j] * supply[c][j]
//  Subject to:
//    sum(j in J) supply[c][j] == 1                    for all c in C,
//    sum(c in C) supply[c][j] <= (|C| - 1) * used[j]  for all j in J,
//    supply[c][j] in {0, 1}                   for all c in C, j in J,
//    used[j] in {0, 1}                                for all j in J.
//
// The first set of constraints are the demand constraints ensuring that the
// demand of each client is satisfied. The second set of constraints are the
// capacity constraints, ensuring that if a facility is placed in location j
// the capacity of that facility is not exceeded.
//
// The program in this file, formulates a facility location problem and
// solves it. Furthermore, different cutting planes methods are implemented
// using the generic callback API to help the solution of the problem:
//
// - Disaggregated capacity cuts separated algorithmically (see the
//   FacilityCallback::separateDisaggregatedCuts method for details),
//
// - Disaggregated capacity cuts separated using a cut table (see the
//   FacilityCallback::cutsFromTable method for details),
//
// - Capacity constraints separated as lazy constraints (see the
//   FacilityCallback::lazyCapacity method for details).
//
// Those different methods are invoked using the generic callback API.
//
// See the usage message below for how to switch between these options.

#include <sstream>
#include <ilcplex/ilocplex.h>


using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::stringstream;

static void usage(const char *progname)
{
   cerr << "Usage: " << progname << " [options...]" << endl
        << " By default, a user cut callback is useed to dynamically"    << endl
        << " separate constraints."  << endl << endl
        << " Supported options are:" << endl
        << "  -table       Instead of the default behavior, use a"      << endl
        << "               static table that holds all cuts and"        << endl
        << "               scan that table for violated cuts."          << endl
        << "  -no-cuts     Do not separate any cuts."                   << endl
        << "  -lazy        Do not include capacity constraints in the"  << endl
        << "               model. Instead, separate them from a lazy"   << endl
        << "               constraint callback."                        << endl
        << "  -data=<file> Specify data file to use."  << endl
      ;
   exit(2);
}

// Some shortcut typedefs.
typedef IloArray<IloNumArray>    FloatMatrix;
typedef IloArray<IloNumVarArray> NumVarMatrix;

#define EPS 1e-6 // epsilon useed for violation of cuts

// This is the class implementing the generic callback interface.
//
// It has three main functions:
//    - separateDisaggregatedCuts: add disaggregated constraints linking
//      clients and location.
//    - cutsFromTable: do the same using a cut table.
//    - lazyCapacity: adds the capacity constraint as a lazy constraint.
class FacilityCallback: public IloCplex::Callback::Function {
   private:
      // Empty constructor is forbidden.
      FacilityCallback ()
      {}

      // Copy constructor is forbidden.
      FacilityCallback(const FacilityCallback &tocopy);

      // Variables for opening facilities.
      IloNumVarArray opened;

      // Variables representing amount supplied from facility j to customer c.
      NumVarMatrix supply;

      // Table of cuts that can be separated.
      IloRangeArray cuts;

   public:
      // Constructor with data.
      FacilityCallback(const IloNumVarArray &_opened,
                       const NumVarMatrix &_supply):
         opened(_opened), supply(_supply), cuts(opened.getEnv())
      {}

      // Separate the disaggregated capacity constraints.
      //
      // In the model we have for each location j the constraint
      //    sum(c in clients) supply[c][j] <= (nbClients-1) * opened[j]
      // Clearly, a client can only be serviced from a location that is opened,
      // so we also have a constraint
      //    supply[c][j] <= opened[j]
      // that must be satisfied by every feasible solution. These constraints
      // tend to be violated in LP relaxation. In this callback we separate
      // them.
      inline void
      separateDisaggregatedCuts (const IloCplex::Callback::Context &context) const {
         IloInt const nbLocations = opened.getSize();
         IloInt const nbClients = supply.getSize();

         // For each j and c check whether in the current solution (obtained by
         // calls to getValue()) we have supply[c][j] > opened[j]. If so, then we have
         // found a violated constraint and add it as a cut.
         for (IloInt j = 0; j < nbLocations; ++j) {
            for (IloInt c = 0; c < nbClients; ++c) {
               IloNum const s = context.getRelaxationPoint(supply[c][j]);
               IloNum const o = context.getRelaxationPoint(opened[j]);
               if ( s > o + EPS) {
                  cout << "Adding: " << supply[c][j].getName() << " <= "
                       << opened[j].getName() << " [" << s << " > " << o << "]" << endl;
                  context.addUserCut( supply[c][j] - opened[j] <= 0,
                                     IloCplex::UseCutPurge, IloFalse);
               }
            }
         }
      }

      // Variant of separateDisaggregatedCuts that looks for violated cuts in
      // the static table cuts.
      inline void
      cutsFromTable (const IloCplex::Callback::Context &context) const {
         for (IloInt i = 0; i < cuts.getSize(); ++i) {
            const IloRange& cut = cuts[i];
            IloNum const lhs = context.getRelaxationValue(cut.getExpr());
            if (lhs < cut.getLB() - EPS || lhs > cut.getUB() + EPS ) {
               cout << "Adding: " << cut << " [lhs = " << lhs << "]" << endl;
               context.addUserCut(cut, IloCplex::UseCutPurge, IloFalse);
            }
         }
      }

      // Function to populate the cut table used by cutsFromTable.
      void populateCutTable (const IloEnv &env)
      {
         IloInt const nbLocations = opened.getSize();
         IloInt const nbClients = supply.getSize();
         // Generate all disaggregated constraints and put them into a
         // table that is scanned by the callback.
         cuts = IloRangeArray(env);
         for (IloInt j = 0; j < nbLocations; ++j)
            for (IloInt c = 0; c < nbClients; ++c)
               cuts.add(supply[c][j] - opened[j] <= 0.0);
      }

      // Lazy constraint callback to enforce the capacity constraints.
      //
      // If used then the callback is invoked for every integer feasible
      // solution CPLEX finds. For each location j it checks whether
      // constraint
      //    sum(c in C) supply[c][j] <= (|C| - 1) * opened[j]
      // is satisfied. If not then it adds the violated constraint as lazy
      // constraint.
      inline void
      lazyCapacity (const IloCplex::Callback::Context &context) const {
         IloInt const nbLocations = opened.getSize();
         IloInt const nbClients = supply.getSize();
         if ( !context.isCandidatePoint() )
            throw IloCplex::Exception(-1, "Unbounded solution");
         for (IloInt j = 0; j < nbLocations; ++j) {
            IloNum isUsed = context.getCandidatePoint(opened[j]);
            IloNum served = 0.0; // Number of clients currently served from j
            for (IloInt c = 0; c < nbClients; ++c)
               served += context.getCandidatePoint(supply[c][j]);
            if ( served > (nbClients - 1.0) * isUsed + EPS ) {
               IloNumExpr sum = IloExpr(context.getEnv());
               for (IloInt c = 0; c < nbClients; ++c)
                  sum += supply[c][j];
               sum -= (nbClients - 1) * opened[j];
               cout << "Adding lazy capacity constraint " << sum << " <= 0" << endl;
               context.rejectCandidate(sum <= 0.0);
               sum.end();
            }
         }
      }

      // This is the function that we have to implement and that CPLEX will call
      // during the solution process at the places that we asked for.
      virtual void invoke (const IloCplex::Callback::Context &context) ILO_OVERRIDE;

      /// Destructor
      virtual ~FacilityCallback();
};

// Implementation of the invoke method.
//
// This is the method that we have to implement to fulfill the
// generic callback contract. CPLEX will call this method during
// the solution process at the places that we asked for.
void
FacilityCallback::invoke (const IloCplex::Callback::Context &context)
{
   if ( context.inRelaxation() ) {
      if ( cuts.getSize() > 0 ) {
         cutsFromTable(context);
      }
      else {
         separateDisaggregatedCuts(context);
      }
   }

   if ( context.inCandidate() )
         lazyCapacity (context);

}

// Destructor
FacilityCallback::~FacilityCallback()
{
   cuts.endElements();
}

int
main(int argc, char **argv)
{
   IloEnv env;
   try {
      // Set default arguments and parse command line.
      char const *datafile ="../../../examples/data/facility.dat";
      bool tableCuts = false;
      bool lazy = false;
      bool separateCuts = true;

      for (int i = 1; i < argc; ++i) {
         if ( ::strncmp(argv[i], "-data=", 6) == 0 )
            datafile = argv[i] + 6;
         else if ( ::strcmp(argv[i], "-table") == 0 )
            tableCuts = true;
         else if ( ::strcmp(argv[i], "-lazy") == 0 )
            lazy = true;
         else if ( ::strcmp(argv[i], "-no-cuts") == 0 )
            separateCuts = false;
         else {
            cerr << "Unknown argument " << argv[i] << endl;
            usage(argv[0]);
         }
      }

      // Setup input file name and open the file.
      ifstream file(datafile);
      if (!file) {
         cerr << "ERROR: could not open file '" << datafile
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
      // - opened[j]      If location j is opened.
      // - supply[c][j] Amount shipped from location j to client c. This is a
      //                number in [0,1] and specifies the percentage of c's
      //                demand that is served from location i.
      IloNumVarArray opened(env, nbLocations, 0, 1, ILOINT);
      opened.setNames("opened");
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
            model.add(v <= (nbClients - 1) * opened[j]);
            v.end();
         }
      }

      // Objective function. We have the fixed cost for openeding a location
      // and the cost proportional to the amount that is shipped from a
      // location.
      IloExpr obj = IloScalProd(fixedCost, opened);
      for (IloInt c = 0; c < nbClients; ++c) {
         obj += IloScalProd(cost[c], supply[c]);
      }
      model.add(IloMinimize(env, obj));
      obj.end();

      IloCplex cplex(env);
      cplex.extract(model);

      // Tweak some CPLEX parameters so that CPLEX has a harder time to
      // solve the model and our cut separators can actually kick in.
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

      // Now we get to setting up the generic callback.
      // We instantiate a FacilityCallback and set the contextMask parameter.
      FacilityCallback cb(opened,supply);
      CPXLONG contextMask = 0;
      if ( separateCuts ) {
         contextMask |= IloCplex::Callback::Context::Id::Relaxation;
         if ( tableCuts ) {
            cb.populateCutTable(env);
         }
      }

      if ( lazy )
         contextMask |= IloCplex::Callback::Context::Id::Candidate;

      // If contextMask is not zero we add the callback.
      if ( contextMask != 0 )
         cplex.use(&cb, contextMask);

      if ( !cplex.solve() )
         throw IloAlgorithm::Exception("No feasible solution found");

      IloNum tolerance = cplex.getParam(
         IloCplex::Param::MIP::Tolerances::Integrality);

      cout << "Solution status:                   " << cplex.getStatus() << endl;
      cout << "Nodes processed:                   " << cplex.getNnodes() << endl;
      cout << "Active user cuts/lazy constraints: " << cplex.getNcuts(IloCplex::CutUser) << endl;
      cout << "Optimal value:                     " << cplex.getObjValue() << endl;
      for (IloInt j = 0; j < nbLocations; j++) {
         if (cplex.getValue(opened[j]) >= 1 - tolerance) {
            cout << "Facility " << j << " is opened, it serves clients";
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

