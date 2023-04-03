// -------------------------------------------------------------- -*- C++ -*-
// File: ilomipex4.cpp 
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
// ilomipex4.cpp - Reading in and optimizing a problem using a callback
//                 to log or interrupt or an IloCplex::Aborter to interrupt
//
// To run this example, command line arguments are required.
// i.e.,   ilomipex4   filename option 
//                        where option is one of 
//                           t to use the time-limit-gap callback
//                           l to use the logging callback
//                           a to use the aborter
//
// Example:
//     ilomipex4  mexample.mps l 
//

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN


static void usage (const char *progname);

// Spend at least timeLimit sec. on optimization, but once
// this limit is reached, quit as soon as the solution is acceptable

ILOMIPINFOCALLBACK5(timeLimitCallback,
                    IloCplex, cplex,
                    IloBool,  aborted,
                    IloNum,   timeStart,
                    IloNum,   timeLimit,
                    IloNum,   acceptableGap)
{
   if ( !aborted  &&  hasIncumbent() ) {
      IloNum gap = 100.0 * getMIPRelativeGap();
      IloNum timeUsed = cplex.getCplexTime() - timeStart;
      if ( timeUsed > 1 )
         getEnv().out() << timeUsed << endl;
      if ( timeUsed > timeLimit && gap < acceptableGap ) {
         getEnv().out() << endl
                        << "Good enough solution at "
                        << timeUsed << " sec., gap = "
                        << gap << "%, quitting." << endl;
         aborted = IloTrue;
         abort();
      }
   }
}

// Log new incumbents if they are at better than the old by a
// relative tolerance of 1e-5; also log progress info every
// 100 nodes.

ILOMIPINFOCALLBACK5(loggingCallback,
                    IloNumVarArray, vars,
                    IloNum,         lastLog, 
                    IloNum,         lastIncumbent,
                    IloNum,         startTime,
                    IloNum,         startDetTime)
{
   int newIncumbent = 0;
   int nodes = getNnodes();

   if ( hasIncumbent()                                  && 
        fabs(lastIncumbent - getIncumbentObjValue())
              > 1e-5*(1.0 + fabs(getIncumbentObjValue())) ) {
      lastIncumbent = getIncumbentObjValue();
      newIncumbent = 1;
   }
     
   if ( nodes >= lastLog + 100  ||  newIncumbent ) {  

      if ( !newIncumbent )  lastLog = nodes;
      getEnv().out() << "Time = " << getCplexTime() - startTime
                     << "  Dettime = " << getDetTime() - startDetTime
                     << "  Nodes = " << nodes
                     << '(' << getNremainingNodes() << ')'
                     << "  Best objective = " << getBestObjValue();

      if ( hasIncumbent() ) {
         getEnv().out() << "  Incumbent objective = " << getIncumbentObjValue()
                        << endl;
      }
      else {
         getEnv().out() << endl;
      }

   }
   if ( newIncumbent ) {
      IloNumArray val(vars.getEnv());
      getIncumbentValues(val, vars);
      val[0] = getIncumbentValue(vars[0]);
      getEnv().out() << "New incumbent variable values: " << endl
                     << val << endl;
      val.end();
   }
}


int
main (int argc, char **argv)
{
   IloEnv env;
   try {
      IloModel model(env);
      IloCplex cplex(env);

      IloBool useLoggingCallback = IloFalse;
      IloBool useTimeLimitCallback = IloFalse;
      IloBool useAborter = IloFalse;


      if (( argc != 3 )                              ||
          ( strchr ("lat", argv[2][0]) == NULL )  ) {
         usage (argv[0]);
         throw(-1);
      }

      switch (argv[2][0]) {
         case 'l':
            useLoggingCallback = IloTrue;
            break;
         case 't':
            useTimeLimitCallback = IloTrue;
            break;
         case 'a':
            useAborter = IloTrue;
            break;
         default:
            break;
      }

      IloObjective   obj;
      IloNumVarArray var(env);
      IloRangeArray  rng(env);
      IloSOS1Array   sos1(env);
      IloSOS2Array   sos2(env);
      IloRangeArray  lazy(env);
      IloRangeArray  cuts(env);

      IloCplex::Aborter myAborter;

      cplex.importModel(model, argv[1], obj, var, rng, sos1, sos2,
                        lazy, cuts);

      cplex.extract(model);

      if ( lazy.getSize() > 0 )  cplex.addLazyConstraints (lazy);
      if ( cuts.getSize() > 0 )  cplex.addUserCuts (cuts);

      if ( useLoggingCallback ) {
         // Set an overall node limit in case callback conditions
         // are not met.
         cplex.setParam(IloCplex::Param::MIP::Limits::Nodes, 5000);

         IloNum lastObjVal = (obj.getSense() == IloObjective::Minimize ) ?
                                 IloInfinity : -IloInfinity;
         cplex.use(loggingCallback(env, var, -100000, lastObjVal,
                                   cplex.getCplexTime(), cplex.getDetTime()));
         // Turn off CPLEX logging
         cplex.setParam(IloCplex::Param::MIP::Display, 0);
      }
      else if ( useTimeLimitCallback ) {
         cplex.use(timeLimitCallback(env, cplex, IloFalse, cplex.getCplexTime(),
                                     1.0, 10.0));
      }
      else if ( useAborter ) {
         myAborter = IloCplex::Aborter(env); 
         cplex.use(myAborter);
         // Typically, you would pass the Aborter object to
         // another thread or pass it to an interrupt handler,
         // and  monitor for some event to occur.  When it does,
         // call the Aborter's abort method.
         //
         // To illustrate its use without creating a thread or
         // an interrupt handler, abort immediately by calling
         // abort before the solve.
         //  
         myAborter.abort();
      }


      cplex.solve();

      env.out() << endl;
      env.out() << "Solution status = " << cplex.getStatus() << endl;
      env.out() << "CPLEX status =    " << cplex.getCplexStatus() << endl;

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
   cerr << "Usage: " << progname << "filename option" << endl;
   cerr << "   where option is one of " << endl;
   cerr << "      t to use the time-limit-gap callback" << endl;
   cerr << "      l to use the logging callback" << endl;
   cerr << "      a to use the aborter" << endl;
   cerr << "   where filename is a file with extension " << endl;
   cerr << "      MPS, SAV, or LP (lower case is allowed)" << endl;
   cerr << " Exiting..." << endl;
} // END usage
