// -------------------------------------------------------------- -*- C++ -*-
// File: etsp.cpp
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
// etsp.cpp -  Solving an earliness-tardiness scheduling problem
//             using CPLEX linearization capabilities.
//                   
// A command line argument is required to run this example.
//
// Example:
//     etsp ../../../examples/data/etsp.dat
//
// For more detail about this example, see the topic "Early tardy 
// scheduling" part of "Discrete optimization" in the CPLEX User's Manual.
// 

#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

typedef IloArray<IloIntArray>    IntMatrix;
typedef IloArray<IloNumArray>    NumMatrix;
typedef IloArray<IloNumVarArray> NumVarMatrix;

const IloInt Horizon = 10000;

int
main(int argc, char** argv)
{
   IloEnv env;
   try {
      IloInt i, j;

      const char* filename;
      if (argc > 1)
         filename = argv[1];
      else
         filename = "../../../examples/data/etsp.dat";
      ifstream f(filename, ios::in);
      if (!f) {
         cerr << "No such file: " << filename << endl;
         throw(1);
      }

      IntMatrix   activityOnAResource(env);
      NumMatrix   duration(env);
      IloNumArray jobDueDate(env);
      IloNumArray jobEarlinessCost(env);
      IloNumArray jobTardinessCost(env);

      f >> activityOnAResource;
      f >> duration;
      f >> jobDueDate;
      f >> jobEarlinessCost;
      f >> jobTardinessCost;

      IloInt nbJob      = jobDueDate.getSize();
      IloInt nbResource = activityOnAResource.getSize();

      IloModel model(env);

      // Create start variables
      NumVarMatrix s(env, nbJob);
      for (j = 0; j < nbJob; j++) {
         s[j] = IloNumVarArray(env, nbResource, 0.0, Horizon);
      }

      // State precedence constraints
      for (j = 0; j < nbJob; j++) {
         for (i = 1; i < nbResource; i++) {
            model.add(s[j][i] >= s[j][i-1] + duration[j][i-1]);
         }
      }

      // State disjunctive constraints for each resource
      for (i = 0; i < nbResource; i++) {
         IloInt end = nbJob - 1;
         for (j = 0; j < end; j++) {
            IloInt a = activityOnAResource[i][j];
            for (IloInt k = j + 1; k < nbJob; k++) {
              IloInt b = activityOnAResource[i][k];
              model.add(s[j][a] >= s[k][b] + duration[k][b] ||
                        s[k][b] >= s[j][a] + duration[j][a]);
            }
         }
      }

      // The cost is the sum of earliness or tardiness costs of each job 
      IloInt last = nbResource - 1;
      IloExpr costSum(env);
      for (j = 0; j < nbJob; j++) {
         costSum += IloPiecewiseLinear(s[j][last] + duration[j][last],
            IloNumArray(env, 1, jobDueDate[j]),
            IloNumArray(env, 2, -jobEarlinessCost[j], jobTardinessCost[j]),
            jobDueDate[j], 0);
      }
      model.add(IloMinimize(env, costSum));
      costSum.end();

      IloCplex cplex(env);

      cplex.extract(model);

      cplex.setParam(IloCplex::Param::Emphasis::MIP, 4);

      if (cplex.solve()) {
          cout << "Solution status: " << cplex.getStatus() << endl;
          cout << " Optimal Value = " << cplex.getObjValue() << endl;
      }
   }
   catch (IloException& ex) {
      cerr << "Error: " << ex << endl;
   }
   catch (...) {
      cerr << "Error" << endl;
   }
   env.end();
   return 0;
}

/* 
Example input file:
[[1, 3, 4, 1, 2, 4, 2, 4],
 [3, 0, 0, 3, 0, 3, 4, 2],
 [0, 1, 2, 4, 1, 2, 0, 1],
 [4, 4, 1, 2, 3, 0, 3, 0],
 [2, 2, 3, 0, 4, 1, 1, 3],
 [6, 5, 6, 6, 5, 6, 5, 5],
 [7, 6, 7, 7, 7, 7, 7, 7],
 [5, 7, 5, 5, 6, 5, 6, 6]]
[[41, 32, 72, 65, 53, 35, 53, 2],
 [21, 44, 75, 58, 66, 7, 9, 16],
 [21, 70, 3, 85, 70, 39, 86, 96],
 [95, 78, 57, 65, 57, 58, 35, 9],
 [35, 34, 5, 34, 1, 12, 11, 86],
 [73, 98, 22, 56, 31, 40, 53, 71],
 [42, 95, 27, 17, 61, 92, 30, 66],
 [70, 49, 13, 71, 77, 5, 34, 99]]
[983, 1017, 1353, 1071, 1211, 1039, 1368, 1139]
[3, 9, 13, 19, 20, 20, 6, 9]
[10, 11, 4, 12, 15, 7, 6, 5]
*/
