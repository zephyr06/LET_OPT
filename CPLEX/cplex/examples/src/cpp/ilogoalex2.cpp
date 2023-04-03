// -------------------------------------------------------------- -*- C++ -*-
// File: ilogoalex2.cpp
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
// ilogoalex2.cpp -- Solve noswot by adding cuts through the goal API
//

#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

ILOCPLEXGOAL3(CutGoal, IloExprArray, lhs, IloNumArray, rhs, IloNum, eps) {
   if ( isIntegerFeasible() )
      return 0;

   IloCplex::Goal goal = this;
   IloInt n = lhs.getSize();
   IloBool foundCut = IloFalse;
   for (IloInt i = 0; i < n; i++) {
      IloNum xrhs = rhs[i];
      if ( xrhs < IloInfinity  &&  getValue(lhs[i]) > xrhs + eps ) {
         goal = AndGoal(GlobalCutGoal(lhs[i] <= xrhs), goal);
         rhs[i] = IloInfinity;
         foundCut = IloTrue;
      }

      if ( !foundCut ) {
         goal = AndGoal(BranchAsCplexGoal(getEnv()), goal);
      }
   }
   return goal;
}

void
makeCuts(const IloNumVarArray vars, IloExprArray lhs, IloNumArray rhs)
{
   IloNumVar x11, x12, x13, x14, x15;
   IloNumVar x21, x22, x23, x24, x25;
   IloNumVar x31, x32, x33, x34, x35;
   IloNumVar x41, x42, x43, x44, x45;
   IloNumVar x51, x52, x53, x54, x55;
   IloNumVar w11, w12, w13, w14, w15;
   IloNumVar w21, w22, w23, w24, w25;
   IloNumVar w31, w32, w33, w34, w35;
   IloNumVar w41, w42, w43, w44, w45;
   IloNumVar w51, w52, w53, w54, w55;
   IloInt num = vars.getSize();

   for (IloInt i = 0; i < num; i++) {
      if      ( strcmp(vars[i].getName(), "X11") == 0 ) x11 = vars[i];
      else if ( strcmp(vars[i].getName(), "X12") == 0 ) x12 = vars[i];
      else if ( strcmp(vars[i].getName(), "X13") == 0 ) x13 = vars[i];
      else if ( strcmp(vars[i].getName(), "X14") == 0 ) x14 = vars[i];
      else if ( strcmp(vars[i].getName(), "X15") == 0 ) x15 = vars[i];
      else if ( strcmp(vars[i].getName(), "X21") == 0 ) x21 = vars[i];
      else if ( strcmp(vars[i].getName(), "X22") == 0 ) x22 = vars[i];
      else if ( strcmp(vars[i].getName(), "X23") == 0 ) x23 = vars[i];
      else if ( strcmp(vars[i].getName(), "X24") == 0 ) x24 = vars[i];
      else if ( strcmp(vars[i].getName(), "X25") == 0 ) x25 = vars[i];
      else if ( strcmp(vars[i].getName(), "X31") == 0 ) x31 = vars[i];
      else if ( strcmp(vars[i].getName(), "X32") == 0 ) x32 = vars[i];
      else if ( strcmp(vars[i].getName(), "X33") == 0 ) x33 = vars[i];
      else if ( strcmp(vars[i].getName(), "X34") == 0 ) x34 = vars[i];
      else if ( strcmp(vars[i].getName(), "X35") == 0 ) x35 = vars[i];
      else if ( strcmp(vars[i].getName(), "X41") == 0 ) x41 = vars[i];
      else if ( strcmp(vars[i].getName(), "X42") == 0 ) x42 = vars[i];
      else if ( strcmp(vars[i].getName(), "X43") == 0 ) x43 = vars[i];
      else if ( strcmp(vars[i].getName(), "X44") == 0 ) x44 = vars[i];
      else if ( strcmp(vars[i].getName(), "X45") == 0 ) x45 = vars[i];
      else if ( strcmp(vars[i].getName(), "X51") == 0 ) x51 = vars[i];
      else if ( strcmp(vars[i].getName(), "X52") == 0 ) x52 = vars[i];
      else if ( strcmp(vars[i].getName(), "X53") == 0 ) x53 = vars[i];
      else if ( strcmp(vars[i].getName(), "X54") == 0 ) x54 = vars[i];
      else if ( strcmp(vars[i].getName(), "X55") == 0 ) x55 = vars[i];
      else if ( strcmp(vars[i].getName(), "W11") == 0 ) w11 = vars[i];
      else if ( strcmp(vars[i].getName(), "W12") == 0 ) w12 = vars[i];
      else if ( strcmp(vars[i].getName(), "W13") == 0 ) w13 = vars[i];
      else if ( strcmp(vars[i].getName(), "W14") == 0 ) w14 = vars[i];
      else if ( strcmp(vars[i].getName(), "W15") == 0 ) w15 = vars[i];
      else if ( strcmp(vars[i].getName(), "W21") == 0 ) w21 = vars[i];
      else if ( strcmp(vars[i].getName(), "W22") == 0 ) w22 = vars[i];
      else if ( strcmp(vars[i].getName(), "W23") == 0 ) w23 = vars[i];
      else if ( strcmp(vars[i].getName(), "W24") == 0 ) w24 = vars[i];
      else if ( strcmp(vars[i].getName(), "W25") == 0 ) w25 = vars[i];
      else if ( strcmp(vars[i].getName(), "W31") == 0 ) w31 = vars[i];
      else if ( strcmp(vars[i].getName(), "W32") == 0 ) w32 = vars[i];
      else if ( strcmp(vars[i].getName(), "W33") == 0 ) w33 = vars[i];
      else if ( strcmp(vars[i].getName(), "W34") == 0 ) w34 = vars[i];
      else if ( strcmp(vars[i].getName(), "W35") == 0 ) w35 = vars[i];
      else if ( strcmp(vars[i].getName(), "W41") == 0 ) w41 = vars[i];
      else if ( strcmp(vars[i].getName(), "W42") == 0 ) w42 = vars[i];
      else if ( strcmp(vars[i].getName(), "W43") == 0 ) w43 = vars[i];
      else if ( strcmp(vars[i].getName(), "W44") == 0 ) w44 = vars[i];
      else if ( strcmp(vars[i].getName(), "W45") == 0 ) w45 = vars[i];
      else if ( strcmp(vars[i].getName(), "W51") == 0 ) w51 = vars[i];
      else if ( strcmp(vars[i].getName(), "W52") == 0 ) w52 = vars[i];
      else if ( strcmp(vars[i].getName(), "W53") == 0 ) w53 = vars[i];
      else if ( strcmp(vars[i].getName(), "W54") == 0 ) w54 = vars[i];
      else if ( strcmp(vars[i].getName(), "W55") == 0 ) w55 = vars[i];
   }

   lhs.add(x21 - x22);
   rhs.add(0.0);

   lhs.add(x22 - x23);
   rhs.add(0.0);

   lhs.add(x23 - x24);
   rhs.add(0.0);

   lhs.add(2.08*x11 + 2.98*x21 + 3.47*x31 + 2.24*x41 + 2.08*x51 +
           0.25*w11 + 0.25*w21 + 0.25*w31 + 0.25*w41 + 0.25*w51);
   rhs.add(20.25);

   lhs.add(2.08*x12 + 2.98*x22 + 3.47*x32 + 2.24*x42 + 2.08*x52 +
           0.25*w12 + 0.25*w22 + 0.25*w32 + 0.25*w42 + 0.25*w52);
   rhs.add(20.25);

   lhs.add(2.08*x13 + 2.98*x23 + 3.47*x33 + 2.24*x43 + 2.08*x53 +
           0.25*w13 + 0.25*w23 + 0.25*w33 + 0.25*w43 + 0.25*w53);
   rhs.add(20.25);

   lhs.add(2.08*x14 + 2.98*x24 + 3.47*x34 + 2.24*x44 + 2.08*x54 +
           0.25*w14 + 0.25*w24 + 0.25*w34 + 0.25*w44 + 0.25*w54);
   rhs.add(20.25);

   lhs.add(2.08*x15 + 2.98*x25 + 3.47*x35 + 2.24*x45 + 2.08*x55 +
           0.25*w15 + 0.25*w25 + 0.25*w35 + 0.25*w45 + 0.25*w55);
   rhs.add(16.25);
}


int
main(int argc, char** argv)
{
   IloEnv env;
   try {
      IloModel m(env);
      IloCplex cplex(env);
    
      IloObjective   obj;
      IloNumVarArray var(env);
      IloRangeArray  con(env);
    
      const char* datadir = (argc >= 2) ? argv[1] : "../../../examples/data";
      char *fname = new char[strlen(datadir) + 1 + strlen("noswot.mps") + 1];
      sprintf(fname, "%s/noswot.mps", datadir);
      env.out() << "reading " << fname << endl;
      cplex.importModel(m, fname, obj, var, con);
      delete[] fname;

      env.out() << "extracting model ..." << endl;
      cplex.extract(m);
      cplex.setParam(IloCplex::Param::MIP::Interval, 1000);
      cplex.setParam(IloCplex::Param::MIP::Strategy::Search,
                     IloCplex::Traditional);
    
      env.out() << "initializing cuts ..." << endl;
      IloExprArray lhs(env);
      IloNumArray  rhs(env);
      makeCuts(var, lhs, rhs);
    
      env.out() << "solving model ...\n";
      cplex.solve(CutGoal(env, lhs, rhs, cplex.getParam(
         IloCplex::Param::Simplex::Tolerances::Feasibility)));
      env.out() << "solution status is " << cplex.getStatus() << endl;
      env.out() << "solution value  is " << cplex.getObjValue() << endl;
   }
   catch (IloException& ex) {
      cerr << "Error: " << ex << endl;
   }
   env.end();
   return 0;
}
