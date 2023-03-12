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
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
        task0 = tasks[0];
        task1 = tasks[1];
        task2 = tasks[2];
        job00 = JobCEC(0, 0);
        job01 = JobCEC(0, 1);
        job10 = JobCEC(1, 0);
        job20 = JobCEC(2, 0);

        perm01 = TwoTaskPermutations(0, 1, dag_tasks, tasks_info);
        perm12 = TwoTaskPermutations(1, 2, dag_tasks, tasks_info);
        task_sets_perms = TaskSetPermutation(dag_tasks, task_chain);
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
    TaskSetPermutation task_sets_perms;
    std::vector<int> task_chain = {0, 1, 2};
};

TEST_F(PermutationTest1, Iteration) {
    int obj_find = task_sets_perms.PerformOptimization();
    task_sets_perms.best_yet_chain_[0].print();
    task_sets_perms.best_yet_chain_[1].print();
    EXPECT_EQ(6, task_sets_perms.iteration_count_);
    EXPECT_EQ(20, obj_find);
}

TEST_F(PermutationTest1, GenerateBoxPermutationConstraints) {
    // RTA 1 3 6
    VariableRange variable_range = FindVariableRange(dag_tasks);
    PermutationInequality perm_ineq =
        GenerateBoxPermutationConstraints(0, 1, variable_range);
    EXPECT_EQ(1 - 17, perm_ineq.lower_bound_);
    EXPECT_EQ(10 - 0, perm_ineq.upper_bound_);

    perm_ineq = GenerateBoxPermutationConstraints(1, 2, variable_range);
    EXPECT_EQ(3 - 14, perm_ineq.lower_bound_);
    EXPECT_EQ(20 - 0, perm_ineq.upper_bound_);
}

class PermutationTest2 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v14.csv",
            "RM", 1);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
        task0 = tasks[0];
        task1 = tasks[1];
        task2 = tasks[2];
        job00 = JobCEC(0, 0);
        job01 = JobCEC(0, 1);
        job10 = JobCEC(1, 0);
        job20 = JobCEC(2, 0);

        perm01 = TwoTaskPermutations(0, 1, dag_tasks, tasks_info);
        perm12 = TwoTaskPermutations(1, 2, dag_tasks, tasks_info);
        task_sets_perms = TaskSetPermutation(dag_tasks, task_chain);
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
    TaskSetPermutation task_sets_perms;
    std::vector<int> task_chain = {4, 3, 0};
};

TEST_F(PermutationTest2, FindVariableRange) {
    // RTA 17 19 53
    VariableRange variable_range = FindVariableRange(dag_tasks);
    EXPECT_EQ(213, variable_range.lower_bound[1].deadline);
    EXPECT_EQ(25, variable_range.lower_bound[2].deadline);
    EXPECT_EQ(27, variable_range.lower_bound[3].deadline);
    EXPECT_EQ(61, variable_range.lower_bound[4].deadline);
}

TEST_F(PermutationTest2, GenerateBoxPermutationConstraints) {
    // RTA 17 19 53
    VariableRange variable_range = FindVariableRange(dag_tasks);
    PermutationInequality perm_ineq =
        GenerateBoxPermutationConstraints(4, 3, variable_range);
    EXPECT_EQ(61 - (100 - 27), perm_ineq.lower_bound_);
    EXPECT_EQ(100 - 0, perm_ineq.upper_bound_);

    perm_ineq = GenerateBoxPermutationConstraints(3, 0, variable_range);
    EXPECT_EQ(27 - (100 - 17), perm_ineq.lower_bound_);
    EXPECT_EQ(100 - 0, perm_ineq.upper_bound_);
}

int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}