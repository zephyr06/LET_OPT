// -------------------------------------------------------------- -*- C++ -*-
// File: iloadmipex1.cpp
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
// iloadmipex1.cpp -  Use the node and branch callbacks
//                    for optimizing a MIP problem
//
// To run this example, command line arguments are required.
// i.e.,   iloadmipex1   filename
//
// Example:
//     iloadmipex1  example.mps
//

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

static void usage (const char *progname);


ILOBRANCHCALLBACK1(MyBranch, IloNumVarArray, vars) {
   if ( getBranchType() != BranchOnVariable )
      return;

   // Branch on var with largest objective coefficient
   // among those with largest infeasibility

   IloNumArray x;
   IloNumArray obj;
   IntegerFeasibilityArray feas;

   try {
      x    = IloNumArray(getEnv());
      obj  = IloNumArray(getEnv());
      feas = IntegerFeasibilityArray(getEnv());
      getValues(x, vars);
      getObjCoefs(obj, vars);
      getFeasibilities(feas, vars);

      IloInt bestj  = -1;
      IloNum maxinf = 0.0;
      IloNum maxobj = 0.0;
      IloInt cols = vars.getSize();
      for (IloInt j = 0; j < cols; j++) {
         if ( feas[j] == Infeasible ) {
            IloNum xj_inf = x[j] - IloFloor (x[j]);
            if ( xj_inf > 0.5 )
               xj_inf = 1.0 - xj_inf;
            if ( xj_inf >= maxinf                              &&
                 (xj_inf > maxinf || IloAbs (obj[j]) >= maxobj)  ) {
               bestj  = j;
               maxinf = xj_inf;
               maxobj = IloAbs (obj[j]);
            }
         }
      }

      if ( bestj >= 0 ) {
         makeBranch(vars[bestj], x[bestj], IloCplex::BranchUp,   getObjValue());
         makeBranch(vars[bestj], x[bestj], IloCplex::BranchDown, getObjValue());
      }
   }
   catch (...) {
      x.end();
      obj.end();
      feas.end();
      throw;
   }
   x.end();
   obj.end();
   feas.end();
}

ILONODECALLBACK0(MySelect) {
   IloInt remainingNodes = getNremainingNodes();
   IloInt bestnode = -1;
   IloInt maxdepth = -1;
   IloNum maxiisum = 0.0;
   for (IloInt i = 0; i < remainingNodes; i++) {
      IloInt depth = getDepth(i);
      IloNum iisum = getInfeasibilitySum(i);
      if ( (depth >= maxdepth)                   &&
           (depth > maxdepth || iisum > maxiisum)  ) {
         bestnode = i;
         maxdepth = depth;
         maxiisum = iisum;
      }
   }
   if ( bestnode >= 0 ) selectNode(bestnode);
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

      cplex.use(MyBranch(env, var));
      cplex.use(MySelect(env));

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
