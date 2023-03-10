#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/ChainPermutation.h"
#include "sources/Optimization/ObjectiveFunction.h"
#include "sources/Optimization/PermutationInequality.h"
#include "sources/Optimization/TaskSetPermutation.h"
#include "sources/Optimization/TwoTaskPermutations.h"
#include "sources/Optimization/Variable.h"
#include "sources/RTA/RTA_LL.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Interval.h"
#include "sources/Utils/JobCEC.h"

using namespace DAG_SPACE;

class PermutationTest1 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v18.csv",
            "orig", 1);
        tasks = dag_tasks.tasks;
        tasks_info = TaskSetInfoDerived(tasks);
        task0 = tasks[0];
        task1 = tasks[1];
        task2 = tasks[2];
        job00 = JobCEC(0, 0);
        job01 = JobCEC(0, 1);
        job10 = JobCEC(1, 0);
        job20 = JobCEC(2, 0);

        perm01 = TwoTaskPermutations(0, 1, tasks_info);
        perm12 = TwoTaskPermutations(1, 2, tasks_info);
        task_sest_perms = TaskSetPermutation(dag_tasks, task_chain);
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

    TwoTaskPermutations perm01;
    TwoTaskPermutations perm12;
    TaskSetPermutation task_sest_perms;
    std::vector<int> task_chain = {0, 1, 2};
};

TEST_F(PermutationTest1, Iteration) {
    int obj_find = task_sest_perms.PerformOptimization();
    task_sest_perms.best_yet_chain_[0].print();
    task_sest_perms.best_yet_chain_[1].print();
    EXPECT_EQ(6, task_sest_perms.iteration_count_);
    EXPECT_EQ(20, obj_find);
}
int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}