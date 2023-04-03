// -------------------------------------------------------------- -*- C++ -*-
// File: ilobenders.cpp
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
// Read in a model from a file and solve it using Benders decomposition.
//
// If an annotation file is provided, use that annotation file.
// Otherwise, auto-decompose the problem and dump the annotation
// to the file 'benders.ann'.
//
// To run this example, command line arguments are required.
// i.e.,   ilobenders   filename   [annofile]
// where 
//     filename is the name of the file, with .mps, .lp, or .sav extension
//     annofile is an optional .ann file with model annotations
//
// Example:
//     ilobenders  UFL_25_35_1.mps UFL_25_35_1.ann
//

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

static void usage (const char *progname);

int
main (int argc, char **argv)
{
   IloEnv env;
   try {
      IloModel model(env);
      IloCplex cpx(env);
      bool hasannofile = false;

      // Check the arguments.
      if ( argc == 3 ) {
         hasannofile = true;
      }
      else if ( argc != 2 ) {
         usage (argv[0]);
         throw(-1);
      }

      IloObjective   obj;
      IloNumVarArray var(env);
      IloRangeArray  rng(env);

      // Read the problem file.
      cpx.importModel(model, argv[1], obj, var, rng);

      // Extract the model.
      cpx.extract(model);

      // If provided, read the annotation file.
      if ( hasannofile ) {
         // Generate default annotations if annofile is "create".
         if ( strcmp (argv[2], "create") == 0 ) {
            IloCplex::LongAnnotation
            benders = cpx.newLongAnnotation (CPX_BENDERS_ANNOTATION,
                                             CPX_BENDERS_MASTERVALUE);
            for (IloInt j = 0; j < var.getSize(); ++j) {
               if ( var[j].getType() == IloNumVar::Float ) {
                  cpx.setAnnotation (benders, var[j],
                                     CPX_BENDERS_MASTERVALUE+1);
               }
            }
         }
         else {
            // Otherwise, read the annotation file.
            cpx.readAnnotations(argv[2]);
         }
      }
      else {
         // Set benders strategy to auto-generate a decomposition.
         cpx.setParam(IloCplex::Param::Benders::Strategy,
                      IloCplex::BendersFull);

         // Write out the auto-generated annotation.
         cpx.writeBendersAnnotation("benders.ann");
      }

      // Solve the problem using Benders' decomposition.
      if ( !cpx.solve() ) {
         env.error() << "Failed to optimize." << endl;
         throw(-1);
      }

      IloAlgorithm::Status status = cpx.getStatus();
      double bestObjValue = cpx.getBestObjValue();
      double objValue = cpx.getObjValue();
      env.out() << "Solution status: " << status << endl;
      env.out() << "Best bound:      " << bestObjValue << endl;
      env.out() << "Best integer:    " << objValue << endl;
   }
   catch (IloException& e) {
      cerr << "Concert exception caught: " << e << endl;
      throw;
   }
   catch (...) {
      cerr << "Unknown exception caught" << endl;
      throw;
   }

   env.end();
   return 0;
}  // END main


static void usage (const char *progname)
{
   cerr << "Usage: " << progname << " filename [annofile]" << endl;
   cerr << "   where filename is a file with extension " << endl;
   cerr << "      MPS, SAV, or LP (lower case is allowed)" << endl;
   cerr << "   and annofile is an optional .ann file with model annotations" << endl;
   cerr << "      If \"create\" is used, the annotation is computed." << endl;
   cerr << " Exiting..." << endl;
} // END usage
