// -------------------------------------------------------------- -*- C++ -*-
// File: ilotuneset.cpp 
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
// ilotuneset.cpp - Tune parameters for a set of problems
//
// To run this example, command line arguments are required.
// i.e.,   ilotuneset [options] file1 file2 ... filen
// where
//     each filei is the name of a file, with .mps, .lp, or
//        .sav extension
//     options are described in usage().
//
// Example:
//     ilotuneset  mexample.mps
//

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

static void usage (const char *progname);

static const int BUFSZ = 128;

int
main (int argc, char **argv)
{
   IloEnv env;
   try {
      IloCplex cplex(env);

      if ( argc < 2 ) {
         usage (argv[0]);
         throw(-1);
      }

      char fixedfile[BUFSZ] = "";
      char tunedfile[BUFSZ] = "";
      int i;
      int tunemeasure;
      int mset = false;
      IloArray<const char *> filenames(env);
      for (i = 1; i < argc; i++) {
         if ( argv[i][0] != '-' )
            filenames.add(argv[i]);
         switch ( argv[i][1] ) {
         case 'a':
            tunemeasure = CPX_TUNE_AVERAGE;
            mset = true;
            break;
         case 'm':
            tunemeasure = CPX_TUNE_MINMAX;
            mset = true;
            break;
         case 'f':
            strncpy (fixedfile, argv[++i], BUFSZ);
            break;
         case 'o':
            strncpy (tunedfile, argv[++i], BUFSZ);
            break;
         }
      }

      cout << "Problem set:" << endl;
      for (i = 0; i < filenames.getSize(); ++i)
          cout << "  " << filenames[i] << endl;

      if ( mset )
         cplex.setParam(IloCplex::Param::Tune::Measure, tunemeasure);

      IloCplex::ParameterSet paramset(env);

      if ( *fixedfile ) {
          cplex.readParam(fixedfile);
          paramset = cplex.getParameterSet();
          cplex.setDefaults();
      }

      IloInt tunestat = cplex.tuneParam(filenames, paramset);

      if      ( tunestat == IloCplex::TuningComplete)
         cout << "Tuning complete." << endl;
      else if ( tunestat == IloCplex::TuningAbort)
         cout << "Tuning abort." << endl;
      else if ( tunestat == IloCplex::TuningTimeLim)
         cout << "Tuning time limit." << endl;
      else
         cout << "Tuning status unknown." << endl;



      if ( *tunedfile ) {
          cplex.writeParam(tunedfile);
          cout << "Tuned parameters written to file '" << tunedfile << "'" <<
              endl;
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
   cerr << "Usage: " << progname << " [options] file1 file2 ... filen" << endl;
   cerr << "   where" << endl;
   cerr << "      filei is a file with extension MPS, SAV, or LP" << endl;
   cerr << "      and options are:" << endl;
   cerr << "         -a for average measure" << endl; 
   cerr << "         -m for minmax measure" << endl;
   cerr << "         -f <file> where file is a fixed parameter file" << endl;
   cerr << "         -o <file> where file is the tuned parameter file" << endl;
   cerr << " Exiting..." << endl;
} // END usage
