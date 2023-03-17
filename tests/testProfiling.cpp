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

class PermutationTest_long_time : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v24.csv",
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
TEST_F(PermutationTest_long_time, ObjReactionTime) {
    // chain is 0 -> 1 -> 4
    TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
    TwoTaskPermutations perm14(1, 4, dag_tasks, tasks_info);

    ChainPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm14[0]);
    perm01[0].print();
    perm14[0].print();

    auto start = std::chrono::high_resolution_clock::now();
    EXPECT_EQ(20000 - 5470, ObjReactionTime::Obj(dag_tasks, tasks_info,
                                                 chain_perm, variable_od));
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