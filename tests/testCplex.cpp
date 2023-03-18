
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
    dag_tasks.chains_ = {{0, 1, 2}};
    TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
    TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);

    ChainsPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[0]);

    GraphOfChains graph_chains(dag_tasks.chains_);

    std::unordered_map<JobCEC, JobCEC> react_chain_map =
        GetFirstReactMap(dag_tasks, tasks_info, chain_perm,
                         dag_tasks.chains_[0]);  // not useful for now
    std::vector<int> rta = {1, 3, 6};
    LPOptimizer lp_optimizer(dag_tasks, tasks_info, chain_perm, graph_chains,
                             "ReactionTime", react_chain_map, rta);
    VectorDynamic startTimeVectorOptmized = lp_optimizer.Optimize();
}

int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}