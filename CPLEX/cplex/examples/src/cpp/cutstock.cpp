// -------------------------------------------------------------- -*- C++ -*-
// File: cutstock.cpp
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

#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

#define RC_EPS 1.0e-6


static void readData (const char* filename, IloNum& rollWidth,
                      IloNumArray& size, IloNumArray& amount);
static void report1 (IloCplex& cutSolver, IloNumVarArray Cut,
                     IloRangeArray Fill);
static void report2 (IloAlgorithm& patSolver, 
                     IloNumVarArray Use, 
                     IloObjective obj);
static void report3 (IloCplex& cutSolver, IloNumVarArray Cut);



/// MAIN PROGRAM ///

int
main(int argc, char **argv)
{
   IloEnv env;
   try {
      IloInt  i, j;

      IloNum      rollWidth;
      IloNumArray amount(env);
      IloNumArray size(env);

      if ( argc > 1 )
         readData(argv[1], rollWidth, size, amount);
      else
         readData("../../../examples/data/cutstock.dat",
                  rollWidth, size, amount);

      /// CUTTING-OPTIMIZATION PROBLEM ///

      IloModel cutOpt (env);

      IloObjective   RollsUsed = IloAdd(cutOpt, IloMinimize(env));
      IloRangeArray  Fill = IloAdd(cutOpt,
                                   IloRangeArray(env, amount, IloInfinity));
      IloNumVarArray Cut(env);

      IloInt nWdth = size.getSize();
      for (j = 0; j < nWdth; j++) {
         Cut.add(IloNumVar(RollsUsed(1) + Fill[j](int(rollWidth / size[j]))));
      }
      
      IloCplex cutSolver(cutOpt);

      /// PATTERN-GENERATION PROBLEM ///

      IloModel patGen (env);

      IloObjective ReducedCost = IloAdd(patGen, IloMinimize(env, 1));
      IloNumVarArray Use(env, nWdth, 0.0, IloInfinity, ILOINT);
      patGen.add(IloScalProd(size, Use) <= rollWidth);

      IloCplex patSolver(patGen);

      /// COLUMN-GENERATION PROCEDURE ///

      IloNumArray price(env, nWdth);
      IloNumArray newPatt(env, nWdth);

      /// COLUMN-GENERATION PROCEDURE ///

      for (;;) {
         /// OPTIMIZE OVER CURRENT PATTERNS ///
       
         cutSolver.solve();
         report1 (cutSolver, Cut, Fill);
       
         /// FIND AND ADD A NEW PATTERN ///
       
         for (i = 0; i < nWdth; i++) {
           price[i] = -cutSolver.getDual(Fill[i]);
         }
         ReducedCost.setLinearCoefs(Use, price);
       
         patSolver.solve();
         report2 (patSolver, Use, ReducedCost);
       
         if (patSolver.getValue(ReducedCost) > -RC_EPS) break;
       
         patSolver.getValues(newPatt, Use);
         Cut.add( IloNumVar(RollsUsed(1) + Fill(newPatt)) );
      }

      cutOpt.add(IloConversion(env, Cut, ILOINT));

      cutSolver.solve();
      cout << "Solution status: " << cutSolver.getStatus() << endl;
      report3 (cutSolver, Cut);
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


static void readData (const char* filename, IloNum& rollWidth,
                      IloNumArray& size, IloNumArray& amount)
{
   ifstream in(filename);
   if (in) {
      in >> rollWidth;
      in >> size;
      in >> amount;
   }
   else {
      cerr << "No such file: " << filename << endl;
      throw(1);
   }
}

static void report1 (IloCplex& cutSolver, IloNumVarArray Cut,
                     IloRangeArray Fill)
{
   cout << endl;
   cout << "Using " << cutSolver.getObjValue() << " rolls" << endl;
   cout << endl;
   for (IloInt j = 0; j < Cut.getSize(); j++) {
      cout << "  Cut" << j << " = " << cutSolver.getValue(Cut[j]) << endl;
   }
   cout << endl;
   for (IloInt i = 0; i < Fill.getSize(); i++) {
      cout << "  Fill" << i << " = " << cutSolver.getDual(Fill[i]) << endl;
   }
   cout << endl;
}

static void report2 (IloAlgorithm& patSolver, IloNumVarArray Use,
                     IloObjective obj)
{
   cout << endl;
   cout << "Reduced cost is " << patSolver.getValue(obj) << endl;
   cout << endl;
   if (patSolver.getValue(obj) <= -RC_EPS) {
      for (IloInt i = 0; i < Use.getSize(); i++)  {
         cout << "  Use" << i << " = " << patSolver.getValue(Use[i]) << endl;
      }
      cout << endl;
   }
}

static void report3 (IloCplex& cutSolver, IloNumVarArray Cut)
{
   cout << endl;
   cout << "Best integer solution uses " 
        << cutSolver.getObjValue() << " rolls" << endl;
   cout << endl;
   for (IloInt j = 0; j < Cut.getSize(); j++) {
      cout << "  Cut" << j << " = " << cutSolver.getValue(Cut[j]) << endl;
   }
}


/* Example Input file:
115
[25, 40, 50, 55, 70]
[50, 36, 24, 8, 30]
*/
