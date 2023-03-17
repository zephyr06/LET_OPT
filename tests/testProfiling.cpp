#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Baseline/StandardLET.h"
#include "sources/Optimization/ObjectiveFunction.h"
#include "sources/Optimization/PermutationInequality.h"
#include "sources/Optimization/TaskSetPermutation.h"
#include "sources/Optimization/TwoTaskPermutations.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Interval.h"
#include "sources/Utils/JobCEC.h"
using namespace DAG_SPACE;

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
TEST_F(PermutationTest_long_time23, ObjReactionTime) {
    int REPEAT = 10000;
    // chain is 0 -> 1 -> 4
    dag_tasks.chains_ = {{0, 1, 2, 3}};
    TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
    TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
    TwoTaskPermutations perm23(2, 3, dag_tasks, tasks_info);
    ChainsPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[0]);
    chain_perm.push_back(perm23[0]);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < REPEAT; i++)
        ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm, variable_od);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken: " << double(duration.count()) / 1e6 << "\n";
    PrintTimer();
}
class PermutationTest_long_time22 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v22.csv",
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
TEST_F(PermutationTest_long_time22, ObjReactionTime_v2) {
    int REPEAT = 10000;
    // chain is 0 -> 1 -> 4
    // dag_tasks.chains_.push_back({3, 2, 1, 0});
    dag_tasks.chains_ = {{3, 2, 1, 0}};
    TwoTaskPermutations perm10(1, 0, dag_tasks, tasks_info);
    TwoTaskPermutations perm21(2, 1, dag_tasks, tasks_info);
    TwoTaskPermutations perm32(3, 2, dag_tasks, tasks_info);
    ChainsPermutation chain_perm;
    chain_perm.push_back(perm10[0]);
    chain_perm.push_back(perm21[0]);
    chain_perm.push_back(perm32[0]);
    // perm01[0].print();
    // perm14[0].print();

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < REPEAT; i++)
        ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm, variable_od);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken: " << double(duration.count()) / 1e6 << "\n";
    PrintTimer();
}
int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}