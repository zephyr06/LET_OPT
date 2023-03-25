
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

    std::vector<int> rta = {1, 3, 6};
    LPOptimizer lp_optimizer(dag_tasks, tasks_info, graph_chains,
                             "ReactionTime", rta);
    auto res = lp_optimizer.Optimize(chain_perm);
    EXPECT_EQ(20, res.second);
}
TEST_F(PermutationTest1, Incremental) {
    dag_tasks.chains_ = {{0, 1, 2}};
    GraphOfChains graph_chains(dag_tasks.chains_);
    std::vector<int> rta = {1, 3, 6};
    TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
    TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);

    ChainsPermutation chain_perm1;
    chain_perm1.push_back(perm01[0]);
    chain_perm1.push_back(perm12[0]);

    ChainsPermutation chain_perm2;
    chain_perm2.push_back(perm01[1]);
    chain_perm2.push_back(perm12[0]);

    LPOptimizer lp_optimizer(dag_tasks, tasks_info, graph_chains,
                             "ReactionTime", rta);
    auto res = lp_optimizer.OptimizeWithoutClear(chain_perm2);
    lp_optimizer.WriteModelToFile("recourse1.lp");

    lp_optimizer.UpdateSystem(chain_perm1);
    lp_optimizer.WriteModelToFile("recourse2.lp");
    res = lp_optimizer.OptimizeAfterUpdate(chain_perm1);

    EXPECT_EQ(20, res.second);
}

TEST_F(PermutationTest1, FindMinOffset) {
    dag_tasks.chains_ = {{0, 1, 2}};
    TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
    TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);

    ChainsPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[0]);
    chain_perm.print();

    GraphOfChains graph_chains(dag_tasks.chains_);

    std::vector<int> rta = {1, 3, 6};
    LPOptimizer lp_optimizer(dag_tasks, tasks_info, graph_chains,
                             "ReactionTime", rta);
    auto range = lp_optimizer.FindMinOffset(2, chain_perm);
    EXPECT_EQ(14, range);
    // EXPECT_EQ(14, range.start + range.length);
    lp_optimizer.ClearCplexMemory();

    LPOptimizer lp_optimizer1(dag_tasks, tasks_info, graph_chains,
                              "ReactionTime", rta);
    range = lp_optimizer1.FindMinOffset(1, chain_perm);
    EXPECT_EQ(11, range);
    // EXPECT_EQ(11, range.start + range.length);
    lp_optimizer1.ClearCplexMemory();

    LPOptimizer lp_optimizer0(dag_tasks, tasks_info, graph_chains,
                              "ReactionTime", rta);
    range = lp_optimizer0.FindMinOffset(0, chain_perm);
    EXPECT_EQ(0, range);
    // EXPECT_EQ(0, range.start + range.length);
    lp_optimizer0.ClearCplexMemory();
}

int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}