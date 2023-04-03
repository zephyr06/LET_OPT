// -------------------------------------------------------------- -*- C++ -*-
// File: iloglobalqpex1.cpp
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
// iloglobalqpex1.cpp - Reading in and optimizing a convex or nonconvex
// (mixed-integer) QP with convex, first order or global optimizer.
//
// To run this example, command line arguments are required.
// That is:   iloglobalqpex1 filename optimalitytarget
// where 
//     filename is the name of the file, with .mps, .lp, or .sav extension
//     optimalitytarget   is the optimality target
//                 c          for a convex qp
//                 f          for a first order solution (only continuous QP)
//                 g          for the global optimum
//
//
// Example:
//     iloglobalqpex1  nonconvexqp.lp  g
//

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

static void usage (const char *progname);

int
main (int argc, char **argv)
{
   IloEnv env;
   IloBool ismip = false;
   try {
      IloModel model(env);
      IloCplex cplex(env);

      if (( argc != 3 )                         ||
          ( strchr ("cfg", argv[2][0]) == NULL )  ) {
         usage (argv[0]);
         throw(-1);
      }

      switch (argv[2][0]) {
         case 'c':
            cplex.setParam(IloCplex::Param::OptimalityTarget,
                           IloCplex::OptimalityOptimalConvex);
            break;
         case 'f':
            cplex.setParam(IloCplex::Param::OptimalityTarget,
                           IloCplex::OptimalityFirstOrder);
            break;
         case 'g':
            cplex.setParam(IloCplex::Param::OptimalityTarget,
                           IloCplex::OptimalityOptimalGlobal);
            break;
         default:
            break;
      }

      IloObjective   obj;
      IloNumVarArray var(env);
      IloRangeArray  rng(env);
      cplex.importModel(model, argv[1], obj, var, rng);

      cplex.extract(model);
      cplex.exportModel("nonconvexqpex.lp");
      ismip = cplex.isMIP();
      if ( !cplex.solve() ) {
         env.error() << "Failed to optimize QP" << endl;
         throw(-1);
      }

      IloNumArray vals(env);
      cplex.getValues(vals, var);
      env.out() << "Solution status = " << cplex.getStatus() << endl;
      env.out() << "Solution value  = " << cplex.getObjValue() << endl;
      env.out() << "Solution vector = " << vals << endl;
   }
   catch (IloCplex::Exception& e) {
      if ( argv[2][0] == 'c'      &&
           e.getStatus() == CPXERR_Q_NOT_POS_DEF ) {
        if (ismip) {
           env.out()<<"Problem is not convex. Use argument g to get global optimum."<<std::endl;
        }
        else {
           env.out()<<"Problem is not convex. Use argument f to get local optimum "
                <<"or g to get global optimum."<<std::endl;
        }
      }
      else if ( argv[2][0] == 'f'              &&
           e.getStatus() == CPXERR_NOT_FOR_MIP &&
           ismip                                 ) {
         env.out()<<"Problem is a MIP, cannot compute local optima satifying "
                  <<"the first order KKT."<<std::endl;
         env.out()<<"Use argument g to get the global optimum."<<std::endl;
      }
      else  {
         std::cerr<<"Cplex exception caught; "<< e <<std::endl;
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


static void usage (const char *progname)
{
   cerr << "Usage: " << progname << " filename optimalitytarget" << endl;
   cerr << "   where filename is a file with extension " << endl;
   cerr << "      MPS, SAV, or LP (lower case is allowed)" << endl;
   cerr << "   and optimalitytarget is one of the letters" << endl;
   cerr << "          c       for convex QP" << endl;
   cerr << "          f       for first order solution "
           "(only for continuous problems" << endl;
   cerr << "          g       for global optimum" << endl;
   cerr << " Exiting..." << endl;
} // END usage
