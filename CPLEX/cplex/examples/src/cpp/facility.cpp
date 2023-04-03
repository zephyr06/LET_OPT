// -------------------------------------------------------------- -*- C++ -*-
// File: facility.cpp
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

// Solve a capacitated facility location problem, potentially using Benders
// decomposition.
// The model solved here is
//   minimize
//       sum(j in locations) fixedCost[j] * open[j] +
//       sum(j in locations) sum(i in clients) cost[i][j] * supply[i][j]
//   subject to
//       sum(j in locations) supply[i][j] == 1                     for each
//                                                                 client i
//       sum(i in clients) supply[i][j] <= capacity[j] * open[j]   for each
//                                                                 location j
//       supply[i][j] in [0,1]
//       open[j] in {0, 1}
// For further details see the usage() function.

#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

typedef IloArray<IloNumArray>    FloatMatrix;
typedef IloArray<IloNumVarArray> NumVarMatrix;

static void usage(const char *progname);
int
main(int argc, char **argv)
{
   IloEnv env;
   try {
      // Parse the command line.
      const char* filename  = "../../../examples/data/facility.dat";
      enum { NO_BENDERS, AUTO_BENDERS, ANNO_BENDERS } doBenders = NO_BENDERS;
      for (int i = 1; i < argc; ++i) {
         if (argv[i][0] == '-') {
            switch ( argv[i][1] ) {
            case 'a': doBenders = AUTO_BENDERS; break;
            case 'b': doBenders = ANNO_BENDERS; break;
            case 'd': doBenders = NO_BENDERS;   break;
            default: usage (argv[0]);
            }
         }
         else
            filename = argv[i];
      }

      // Read the data file and check that data is consistent.
      ifstream file(filename);
      if (!file) {
         cerr << "ERROR: could not open file '" << filename
              << "' for reading" << endl;
         usage (argv[0]);
         throw(-1);
      }
      IloNumArray capacity(env), fixedCost(env);
      FloatMatrix cost(env);
      file >> fixedCost >> cost >> capacity;
      IloInt const nbLocations = capacity.getSize();
      IloInt const nbClients   = cost.getSize(); 

      IloBool consistentData = (fixedCost.getSize() == nbLocations);
      for (IloInt i = 0; consistentData && (i < nbClients); ++i)
         consistentData = (cost[i].getSize() == nbLocations);
      if (!consistentData) {
         cerr << "ERROR: data file '" 
              << filename << "' contains inconsistent data" << endl;
         throw(-1);
      }

      // Create variables. We have variables
      // open[j]        if location j is open.
      // supply[i][j]]  how much client i is supplied from location j
      IloNumVarArray open(env, nbLocations, 0, 1, ILOINT);
      NumVarMatrix supply(env, nbClients);
      for (IloInt i = 0; i < nbClients; ++i)
         supply[i] = IloNumVarArray(env, nbLocations, 0, 1, ILOFLOAT);

      // Create the model.
      IloModel model(env);
      // Constraint: Each client i must be assigned to exactly one location:
      //   sum(j in nbLocations) supply[i][j] == 1  for each i in nbClients
      for (IloInt i = 0; i < nbClients; ++i)
         model.add(IloSum(supply[i]) == 1);
      // Constraint: For each location j, the capacity of the location must
      //             be respected:
      //   sum(i in nbClients) supply[i][j] <= capacity[j] * open[j]
      for (IloInt j = 0; j < nbLocations; ++j) {
         IloExpr v(env);
         for (IloInt i = 0; i < nbClients; ++i)
            v += supply[i][j];
         model.add(v <= capacity[j] * open[j]);
         v.end();
      }

      // Objective: Minimize the sum of fixed costs for using a location
      //            and the costs for serving a client from a specific location.
      IloExpr obj = IloScalProd(fixedCost, open);
      for (IloInt i = 0; i < nbClients; ++i) {
         obj += IloScalProd(cost[i], supply[i]);
      }
      model.add(IloMinimize(env, obj));
      obj.end();

      // Create a solver instance and extract the model to it.
      IloCplex cplex(env);
      cplex.extract(model);

      switch (doBenders) {
      case ANNO_BENDERS:
         {
            // We specify the structure for doing a Benders decomposition by
            // telling CPLEX which variables are in the master problem using
            // annotations. By default variables are assigned value
            // CPX_BENDERS_MASTERVALUE+1 and thus go into the workers.
            // Variables open[j] should go into the master and therefore
            // we assign them value CPX_BENDERS_MASTER_VALUE.
            IloCplex::LongAnnotation
               decomp = cplex.newLongAnnotation(IloCplex::BendersAnnotation,
                                                CPX_BENDERS_MASTERVALUE + 1);
            for (IloInt j = 0; j < nbLocations; ++j) {
               cplex.setAnnotation(decomp, open[j], CPX_BENDERS_MASTERVALUE);
            }
            cplex.out() << "Solving with explicit Benders decomposition."
                        << endl;
         }
         break;
      case AUTO_BENDERS:
         // Let CPLEX automatically decompose the problem.  In the case of
         // a capacitated facility location problem the variables of the
         // master problem should be the integer variables.  By setting the
         // Benders strategy parameter to Full, CPLEX will put all integer
         // variables into the master, all continuous varibles into a
         // subproblem, and further decompose that subproblem, if possible.
         cplex.setParam(IloCplex::Param::Benders::Strategy, 
                        IloCplex::BendersFull);
         cplex.out() << "Solving with automatic Benders decomposition." << endl;
         break;
      case NO_BENDERS:
         cplex.out() << "Solving without Benders decomposition." << endl;
         break;
      }

      // Solve the model and display solution.
      cplex.solve();
	
      cplex.out() << "Solution status: " << cplex.getStatus() << endl;
      cplex.out() << "Optimal value: " << cplex.getObjValue() << endl;
      IloNum const tolerance = cplex.getParam(
         IloCplex::Param::MIP::Tolerances::Integrality);
      for (IloInt j = 0; j < nbLocations; ++j) {
         if (cplex.getValue(open[j]) >= 1 - tolerance) {
            cplex.out() << "Facility " << j << " is open, it serves clients ";
            for (IloInt i = 0; i < nbClients; ++i) {
               if (cplex.getValue(supply[i][j]) >= 1 - tolerance)
                  cplex.out() << i << " ";
            }
            cplex.out() << endl; 
         }
      }
   }
   catch(IloException& e) {
      cerr  << " ERROR: " << e << endl;   
      throw;
   }
   catch(...) {
      cerr  << " ERROR" << endl;   
      throw;
   }
   env.end();
   return 0;
}

// Print usage message and throw exception.
static void usage(const char *progname)
{
   cerr << "Usage: " << progname << "[options] [inputfile]" << endl;
   cerr << "   where" << endl;
   cerr << "       inputfile describe a capacitated facility location" << endl;
   cerr << "       instance as in ../../../examples/data/facility.dat." << endl;
   cerr << "       If no input file is specified read the file in" << endl;
   cerr << "       example/data directory." <<endl;
   cerr << "       Options are:" << endl;
   cerr << "          -a solve problem with Benders letting CPLEX do the decomposition" << endl;
   cerr << "          -b solve problem with Benders specifying a decomposition" << endl;
   cerr << "          -d solve problem without using decomposition (default)" << endl;
   cerr << " Exiting..." << endl;
   throw (-1);
}
