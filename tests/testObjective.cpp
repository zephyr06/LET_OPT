#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/ObjectiveFunction.h"
#include "sources/Optimization/PermutationInequality.h"
#include "sources/Optimization/TaskSetPermutation.h"
#include "sources/Optimization/TwoTaskPermutations.h"
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

        perm01[0].print();
        perm01[1].print();
        perm01[2].print();
        perm12[0].print();
        perm12[1].print();

        variable_od = VariableOD(tasks);
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
    VariableOD variable_od;
};

TEST_F(PermutationTest1, GetTaskId) {
    ChainPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[0]);
    EXPECT_EQ(0, chain_perm.GetTaskId(0));
    EXPECT_EQ(1, chain_perm.GetTaskId(1));
    EXPECT_EQ(2, chain_perm.GetTaskId(2));
}

TEST_F(PermutationTest1, GetFirstReactJob) {
    // chain is 0 -> 1 -> 2
    TwoTaskPermutations perm01(0, 1, tasks_info);
    TwoTaskPermutations perm12(1, 2, tasks_info);

    ChainPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[0]);

    // with the same super-period
    EXPECT_EQ(JobCEC(1, 0),
              GetFirstReactJobWithSuperPeriod(JobCEC(0, 0), chain_perm, 0));
    EXPECT_EQ(JobCEC(1, 0),
              GetFirstReactJobWithSuperPeriod(JobCEC(0, 1), chain_perm, 0));
    EXPECT_EQ(JobCEC(2, 0),
              GetFirstReactJobWithSuperPeriod(JobCEC(1, 0), chain_perm, 1));

    // span super-periods
    EXPECT_EQ(JobCEC(1, 1),
              GetFirstReactJob(JobCEC(0, 2), chain_perm, 0, tasks_info));
    EXPECT_EQ(JobCEC(1, 1),
              GetFirstReactJob(JobCEC(0, 3), chain_perm, 0, tasks_info));
    EXPECT_EQ(JobCEC(2, 1),
              GetFirstReactJob(JobCEC(1, 1), chain_perm, 1, tasks_info));
}

TEST_F(PermutationTest1, ChainPermutation_v1) {
    // chain is 0 -> 1 -> 2
    TwoTaskPermutations perm01(0, 1, tasks_info);
    TwoTaskPermutations perm12(1, 2, tasks_info);

    ChainPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[0]);
    EXPECT_EQ(20, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm,
                                       variable_od));

    chain_perm.permutation_chain_.clear();
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[1]);
    EXPECT_EQ(40, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm,
                                       variable_od));

    chain_perm.permutation_chain_.clear();
    chain_perm.push_back(perm01[1]);
    chain_perm.push_back(perm12[1]);
    EXPECT_EQ(50, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm,
                                       variable_od));
}

class PermutationTest_Non_Har : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v2.csv",
            "orig", 1);
        tasks = dag_tasks.tasks;
        tasks_info = TaskSetInfoDerived(tasks);
        task0 = tasks[0];
        task1 = tasks[1];
        task2 = tasks[2];

        variable_od = VariableOD(tasks);
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
    Task task0;
    Task task1;
    Task task2;
    VariableOD variable_od;
};

TEST_F(PermutationTest_Non_Har, ChainPermutation_v1) {
    // chain is 0 -> 1 -> 2
    TwoTaskPermutations perm01(0, 1, tasks_info);
    TwoTaskPermutations perm12(1, 2, tasks_info);

    ChainPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[0]);
    perm01[0].print();
    perm12[0].print();
    EXPECT_EQ(15, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm,
                                       variable_od));

    chain_perm.permutation_chain_.clear();
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[1]);
    perm01[0].print();
    perm12[1].print();
    EXPECT_EQ(30, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm,
                                       variable_od));

    chain_perm.permutation_chain_.clear();
    chain_perm.push_back(perm01[1]);
    chain_perm.push_back(perm12[1]);
    perm01[1].print();
    perm12[1].print();
    EXPECT_EQ(35, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm,
                                       variable_od));
}

TEST_F(PermutationTest_Non_Har, diff_deadline_from_variable) {
    // chain is 0 -> 1 -> 2
    TwoTaskPermutations perm01(0, 1, tasks_info);
    TwoTaskPermutations perm12(1, 2, tasks_info);

    ChainPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[0]);
    perm01[0].print();
    perm12[0].print();
    variable_od.variables_[2].deadline = 10;
    EXPECT_EQ(10, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm,
                                       variable_od));
}

int main(int argc, char **argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}