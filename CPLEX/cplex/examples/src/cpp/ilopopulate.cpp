// -------------------------------------------------------------- -*- C++ -*-
// File: ilopopulate.cpp 
// Version 22.1.0  
// --------------------------------------------------------------------------
// Licensed Materials - Property of IBM
// 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
// Copyright IBM Corporation 2007, 2022. All Rights Reserved.
//
// US Government Users Restricted Rights - Use, duplication or
// disclosure restricted by GSA ADP Schedule Contract with
// IBM Corp.
// --------------------------------------------------------------------------
//
// ilopopulate.cpp - Reading in and generating multiple solutions to
//                   a MIP problem
//
// To run this example, command line arguments are required.
// i.e.,   ilopopulate   filename
//
// Example:
//     ilopopulate  location.lp 
//


#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

#define EPSZERO        1.0E-10

static void usage (const char *progname);

int
main (int argc, char **argv)
{
   IloEnv env;
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
      IloRangeArray  lazy(env);
      IloRangeArray  cuts(env);

      cplex.importModel(model, argv[1], obj, var, rng, sos1, sos2, lazy, cuts);

      /* Set the solution pool relative gap parameter to obtain solutions
         of objective value within 10% of the optimal */

      cplex.setParam(IloCplex::Param::MIP::Pool::RelGap, 0.1);

      cplex.extract(model);

      if ( lazy.getSize() > 0 )  cplex.addLazyConstraints(lazy);
      if ( cuts.getSize() > 0 )  cplex.addUserCuts(cuts);

      cplex.populate();

      env.out() << "Solution status = " << cplex.getStatus() << endl;
      env.out() << "Incumbent objective value = " <<
                   cplex.getObjValue() << endl;

      IloNumArray incvals(env);
      cplex.getValues(incvals, var);
      env.out() << "Incumbent values         = " << incvals << endl << endl;

      /* Get the number of solutions in the solution pool */

      int numsol = cplex.getSolnPoolNsolns();
      env.out() << "The solution pool contains " << numsol << " solutions." <<
        endl;

      /* Some solutions are deleted from the pool because of the solution
         pool relative gap parameter */

      int numsolreplaced = cplex.getSolnPoolNreplaced();
      env.out() << numsolreplaced << " solutions were removed due to the "
        "solution pool relative gap parameter." << endl;

      env.out() << "In total, " << numsol + numsolreplaced <<
        " solutions were generated." << endl;

      /* Get the average objective value of solutions in the solution
         pool */

      env.out() << "The average objective value of the solutions is " <<
        cplex.getSolnPoolMeanObjValue() << "." << endl << endl;

      /* Write out the objective value of each solution and its
         difference to the incumbent */

      for (int i = 0; i < numsol; i++) {

        /* Write out objective value */

        env.out() << "Solution " << i << " with objective " 
                  << cplex.getObjValue(i) << " differs in ";
        
        IloNumArray vals(env);
        cplex.getValues(vals, var, i);

        /* Compute the number of variables that differ in the solution
           and in the incumbent */
        
        int numdiff = 0;
        for (int j = 0; j < vals.getSize(); j++) {
          if ( fabs(vals[j] - incvals[j]) > EPSZERO )
            numdiff++;
        }
        env.out() << numdiff << " of " << vals.getSize() << " variables."
                  << endl;
      }

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


static void
usage (const char *progname)
{
   cerr << "Usage: " << progname << " filename" << endl;
   cerr << "   where filename is a file with extension " << endl;
   cerr << "      MPS, SAV, or LP (lower case is allowed)" << endl;
   cerr << " Exiting..." << endl;
} // END usage
