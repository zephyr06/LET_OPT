#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Baseline/StandardLET.h"
#include "sources/Optimization/TaskSetPermutation.h"
using namespace DAG_SPACE;

int REPEAT = 10000;
class PermutationTest_long_time23 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v23.csv",
            "RM", 1);
        // dag_tasks = ReadDAG_Tasks(
        //     GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v22.csv",
        //     "RM", 1);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
        task0 = tasks[0];
        task1 = tasks[1];
        task2 = tasks[2];
        job00 = JobCEC(0, 0);
        job01 = JobCEC(0, 1);
        job10 = JobCEC(1, 0);
        job20 = JobCEC(2, 0);

        variable_od = VariableOD(tasks);
        dag_tasks.chains_[0] = {0, 1, 4};
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
    Task task0;
    Task task1;
    Task task2;
    JobCEC job00;
    JobCEC job01;
    JobCEC job10;
    JobCEC job20;

    VariableOD variable_od;
};

TEST_F(PermutationTest_long_time23, Optimize_direct) {
    // chain is 0 -> 1 -> 4
    dag_tasks.chains_ = {{0, 1, 2, 3}};
    TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
    TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
    TwoTaskPermutations perm23(2, 3, dag_tasks, tasks_info);
    ChainsPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[0]);
    chain_perm.push_back(perm23[0]);

    GraphOfChains graph_of_all_ca_chains(dag_tasks.chains_);

    std::unordered_map<JobCEC, JobCEC> react_chain_map =
        GetFirstReactMap(dag_tasks, tasks_info, chain_perm,
                         dag_tasks.chains_[0]);  // not useful for now
    std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < REPEAT; i++) {
        FindODWithLP(dag_tasks, tasks_info, chain_perm, graph_of_all_ca_chains,
                     "ReactionTime", react_chain_map, rta);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken during Optimize_direct: "
              << double(duration.count()) / 1e6 << "\n";
    PrintTimer();
}

TEST_F(PermutationTest_long_time23, Optimize_Incre) {
    // chain is 0 -> 1 -> 4
    dag_tasks.chains_ = {{0, 1, 2, 3}};
    TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
    TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
    TwoTaskPermutations perm23(2, 3, dag_tasks, tasks_info);
    ChainsPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[0]);
    chain_perm.push_back(perm23[0]);

    ChainsPermutation chain_perm2;
    chain_perm2.push_back(perm01[1]);
    chain_perm2.push_back(perm12[1]);
    chain_perm2.push_back(perm23[1]);

    GraphOfChains graph_of_all_ca_chains(dag_tasks.chains_);

    std::unordered_map<JobCEC, JobCEC> react_chain_map =
        GetFirstReactMap(dag_tasks, tasks_info, chain_perm,
                         dag_tasks.chains_[0]);  // not useful for now
    std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);

    LPOptimizer lp_optimizer(dag_tasks, tasks_info, chain_perm2,
                             graph_of_all_ca_chains, "ReactionTime",
                             react_chain_map, rta);
    lp_optimizer.OptimizeWithoutClear();

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < REPEAT; i++) {
        lp_optimizer.UpdateSystem(chain_perm);
        lp_optimizer.OptimizeAfterUpdate();
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken during Incre_Optimize: "
              << double(duration.count()) / 1e6 << "\n";
    PrintTimer();
}

// TEST_F(PermutationTest_long_time23, ExamObj) {
//     // chain is 0 -> 1 -> 4
//     dag_tasks.chains_ = {{0, 1, 2, 3}};
//     TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
//     TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
//     TwoTaskPermutations perm23(2, 3, dag_tasks, tasks_info);
//     ChainsPermutation chain_perm;
//     chain_perm.push_back(perm01[0]);
//     chain_perm.push_back(perm12[0]);
//     chain_perm.push_back(perm23[0]);

//     GraphOfChains graph_of_all_ca_chains(dag_tasks.chains_);

//     std::unordered_map<JobCEC, JobCEC> react_chain_map =
//         GetFirstReactMap(dag_tasks, tasks_info, chain_perm,
//                          dag_tasks.chains_[0]);  // not useful for now
//     std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
//     auto start = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < REPEAT; i++) {
//         ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm, variable_od,
//                              dag_tasks.chains_);
//     }
//     auto stop = std::chrono::high_resolution_clock::now();
//     auto duration =
//         std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
//     std::cout << "Time taken during objective function evaluation: "
//               << double(duration.count()) / 1e6 << "\n";
//     PrintTimer();
// }

// TEST_F(PermutationTest_long_time23, Optimize_constant) {
//     // chain is 0 -> 1 -> 4
//     dag_tasks.chains_ = {{0, 1, 2, 3}};
//     TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
//     TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
//     TwoTaskPermutations perm23(2, 3, dag_tasks, tasks_info);
//     ChainsPermutation chain_perm;
//     chain_perm.push_back(perm01[0]);
//     chain_perm.push_back(perm12[0]);
//     chain_perm.push_back(perm23[0]);

//     GraphOfChains graph_of_all_ca_chains(dag_tasks.chains_);

//     std::unordered_map<JobCEC, JobCEC> react_chain_map =
//         GetFirstReactMap(dag_tasks, tasks_info, chain_perm,
//                          dag_tasks.chains_[0]);  // not useful for now
//     std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
//     auto start = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < REPEAT; i++) {
//         LPOptimizer lp_optimizer(dag_tasks, tasks_info, chain_perm,
//                                  graph_of_all_ca_chains, "ReactionTime",
//                                  react_chain_map, rta);
//         lp_optimizer.OptimizeConstant();
//     }
//     auto stop = std::chrono::high_resolution_clock::now();
//     auto duration =
//         std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
//     std::cout << "Time taken during Optimize_direct: "
//               << double(duration.count()) / 1e6 << "\n";
//     PrintTimer();
// }
int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}