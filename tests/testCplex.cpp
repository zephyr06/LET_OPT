
#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/LPSolver_Cplex.h"
using namespace DAG_SPACE;

class PermutationTest1 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v18.csv",
            "RM", 1);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
};
TEST_F(PermutationTest1, mapPrev) {
    TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
    TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);

    ChainsPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[0]);
    LPOptimizer lp_optimizer(dag_tasks, tasks_info, chain_perm);
    VectorDynamic startTimeVectorOptmized = sfOrderLPOptimizer.Optimize();
}

int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}