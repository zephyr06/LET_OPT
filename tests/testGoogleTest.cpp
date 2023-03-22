// #include <gtest/gtest.h>
#include <ilcplex/ilocplex.h>

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/LPSolver_Cplex.h"

using ::testing::AtLeast;  // #1
using ::testing::Return;
using namespace std;
TEST(BasicExample, v1) {
    // Model Definition
    IloEnv myenv;                                  // environment object
    IloModel mymodel(myenv);                       // model object
    IloNumVar x(myenv, 0, IloInfinity, ILOFLOAT);  // variable x on [0,infinity)
    IloNumVar y(myenv, 0, IloInfinity, ILOFLOAT);  // variable y on [0,infinity)
    mymodel.add(x + y <= 200);                     // constraint x+y <= 200
    mymodel.add(x + 4 * y <= 400);                 // constraint 5x + 15y <= 400
    mymodel.add(IloMinimize(myenv, 5 * x + 15 * y));  // objective max 5x + 15y
    // Model Solution
    IloCplex mycplex(myenv);
    mycplex.extract(mymodel);
    IloBool feasible =
        mycplex.solve();  // solves model and stores whether or ...
    // not it is feasible in an IloBool variable called "feasible"
    // Printing the Solution
    if (feasible == IloTrue) {
        cout << "\nProblem feasible." << endl;
        cout << "x = " << mycplex.getValue(x) << endl;       // value of x
        cout << "y = " << mycplex.getValue(y) << endl;       // value of y
        cout << "cost = " << mycplex.getObjValue() << endl;  // objective
    } else
        cout << "\nProblem infeasible." << endl;
    // Closing the Model
    mycplex.clear();
    myenv.end();
}

TEST(BasicExample, v2) {
    const int n = 4;  // number of variables/constraints
    // Model Definition
    IloEnv myenv;             // environment object
    IloModel mymodel(myenv);  // model object
    IloNumVarArray x(myenv, n, 0.0, IloInfinity,
                     ILOFLOAT);  // array ...
    // of n unbounded variables x
    // Constraint Generation
    IloConstraintArray cons_array(myenv);
    for (int i = 0; i < n; i++) {
        IloExpr myexpr(myenv);  // empty expression
        for (int j = 0; j < n; j++) {
            if (j == i)
                myexpr += 2 * x[j];  // coefficient 2 on diagonal
            else
                myexpr += x[j];  // coefficient 1 elsewhere
        }
        IloConstraint cons = IloRange(myenv, 1, myexpr, IloInfinity);
        mymodel.add(cons);  // i-th constraint
        cons_array.add(cons);
        myexpr.end();  // clear memory
    }
    // Objective Generation
    IloExpr myexpr(myenv);                       // empty expression
    for (int i = 0; i < n; i++) myexpr += x[i];  // summing all variables
    mymodel.add(IloMinimize(myenv, myexpr));  // adding minimization objective
    myexpr.end();                             // clear memory

    mymodel.remove(cons_array[0]);
    mymodel.remove(cons_array[1]);
    for (int i = 0; i < 2; i++) {
        IloExpr myexpr(myenv);  // empty expression
        for (int j = 0; j < n; j++) {
            if (j == i)
                myexpr += 2 * x[j];  // coefficient 2 on diagonal
            else
                myexpr += x[j];  // coefficient 1 elsewhere
        }
        IloConstraint cons = IloRange(myenv, 1, myexpr, IloInfinity);
        mymodel.add(cons);  // i-th constraint
        cons_array.add(cons);
        myexpr.end();  // clear memory
    }
    mymodel.remove(cons_array[0]);
    mymodel.remove(cons_array[1]);

    IloCplex cplex(mymodel);
    cplex.extract(mymodel);
    cplex.exportModel("recourse.lp");

    // Model Solution
    IloCplex mycplex(myenv);
    mycplex.extract(mymodel);
    mycplex.solve();
    // Printing the Solution
    for (int i = 0; i < n; i++)
        cout << "x(" << i + 1 << ") = " << mycplex.getValue(x[i]) << endl;
    cout << "cost = " << mycplex.getObjValue() << endl;
    // Closing the Model
    mycplex.clear();
    myenv.end();
    // wait for user t
}

int main(int argc, char **argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}