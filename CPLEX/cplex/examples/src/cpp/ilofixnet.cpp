// --------------------------------------------------------------------------
// File: ilofixnet.cpp
// Version 22.1.0
// --------------------------------------------------------------------------
// Licensed Materials - Property of IBM
// 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
// Copyright IBM Corporation 2018, 2022. All Rights Reserved.
//
// US Government Users Restricted Rights - Use, duplication or
// disclosure restricted by GSA ADP Schedule Contract with
// IBM Corp.
// --------------------------------------------------------------------------
//

/** ilofixnet.cpp - Use logical constraints to avoid numerical trouble in a
 *                  fixed charge network flow problem.
 *
 *  Find a minimum cost flow in a fixed charge network flow problem.
 *  The network is as follows:
 *
 *       1 -- 3 ---> demand = 1,000,000
 *      / \  /       
 *     /   \/        
 *    0    /\        
 *     \  /  \       
 *      \/    \      
 *       2 -- 4 ---> demand = 1
 *
 *  A fixed charge of one is incurred for every edge with non-zero flow,
 *  with the exception of edge <1,4>, which has a fixed charge of ten.
 *  The cost per unit of flow on an edge is zero, with the exception of
 *  edge <2,4>, where the cost per unit of flow is five.
 */

#include <iomanip>
#include <iostream>
#include <ilcplex/ilocplex.h>

using std::cout;
using std::cerr;
using std::endl;
using std::setw;

// The network has 5 nodes and 6 edges. */
#define NUMNODES 5
#define NUMEDGES 6

// Define origin and destination nodes for each edge, as well as
// unit costs and fixed costs for transporting flow on each edge.
// Note that by defining a fixed cost of 0 for each arc you just
// get a regular min-cost flow problem.
static int const    orig[]      = {0,0,1,1,2,2};
static int const    dest[]      = {1,2,3,4,3,4};
static double const unitcost[]  = {0,0,0,0,0,5};
static double const fixedcost[] = {1,1,1,10,1,1};

// Define demand (supply) at each node.
static double const demand[] = {-1000001, 0, 0, 1000000, 1};


int
main (void)
{
   IloEnv env;

   try {
      // Create model instance and populate it with objective and constraints.
      IloModel model(env);

      // Create the variables.
      // x variables are continuous variables in [0, infinity[,
      // f variables are binary variables.
      IloNumVarArray x(env, NUMEDGES, 0.0, IloInfinity, ILOFLOAT);
      x.setNames("x");
      IloNumVarArray f(env, NUMEDGES, 0.0, 1.0, ILOBOOL);
      f.setNames("f");

      // Create objective function.
      IloNumExpr obj(env);
      for (IloInt i = 0; i < x.getSize(); ++i)
         obj += x[i] * unitcost[i];
      for (IloInt i = 0; i < f.getSize(); ++i)
         obj += f[i] * fixedcost[i];
      model.add(IloMinimize(env, obj));
      obj.end();

      // Create constraints.
      // There is one constraint for each node. The constraint for a node i
      // states that the flow leaving i and the flow entering i must differ
      // by at least the demand for i.
      for (IloInt i = 0; i < NUMNODES; ++i) {
         IloNumExpr sum(env);
         for (IloInt j = 0; j < NUMEDGES; ++j) {
            if ( orig[j] == i )
               sum -= x[j];
            if ( dest[j] == i)
               sum += x[j];
         }
         model.add(sum >= demand[i]);
         sum.end();
      }

      // Add logical constraints that require x[i]==0 if f[i] is 0.
      for (IloInt i = 0; i < x.getSize(); ++i)
         model.add(IloIfThen(env, f[i] == 0, x[i] == 0));

      // Extract model to solver and solve it.
      IloCplex cplex(model);
      cplex.solve();

      // Write solution value and objective to the screen.
      cout.precision(6);
      cout.setf(std::ios::fixed);
      cout << "Solution status: " << cplex.getStatus() << endl;
      cout << "Solution value  = " << cplex.getObjValue() << endl;
      cout << "Solution vector:" << endl;
      for (IloInt i = 0; i < x.getSize(); ++i)
         cout << x[i].getName() << ": " << setw(15) << cplex.getValue(x[i])
              << endl;
      for (IloInt i = 0; i < f.getSize(); ++i)
         cout << f[i].getName() << ": " << setw(15) << cplex.getValue(f[i])
              << endl;

      // Finally dump the model
      cplex.exportModel("ilofixnet.lp");

      model.end();
      cplex.end();
   }
   catch (IloException &e) {
      cerr << e << endl;
      throw;
   }
   catch (...) {
      cerr << "Unkown exception" << endl;
      throw;
   }
   env.end();

   return 0;
}
