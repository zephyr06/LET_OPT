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

        perm01[0].print();
        perm01[1].print();
        perm01[2].print();
        perm12[0].print();
        perm12[1].print();

        variable_od = VariableOD(tasks);
        dag_tasks.chains_[0] = {0, 1, 2};
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

// TEST_F(PermutationTest1, GetTaskId) {
//     ChainsPermutation chain_perm;
//     chain_perm.push_back(perm01[0]);
//     chain_perm.push_back(perm12[0]);
//     EXPECT_EQ(0, chain_perm.GetTaskId(0));
//     EXPECT_EQ(1, chain_perm.GetTaskId(1));
//     EXPECT_EQ(2, chain_perm.GetTaskId(2));
// }

TEST_F(PermutationTest1, GetFirstReactJob) {
    // chain is 0 -> 1 -> 2
    TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
    TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);

    ChainsPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[0]);

    // with the same super-period
    EXPECT_EQ(JobCEC(1, 0), GetFirstReactJobWithSuperPeriod(
                                JobCEC(0, 0), chain_perm[Edge(0, 1)]));
    EXPECT_EQ(JobCEC(1, 0), GetFirstReactJobWithSuperPeriod(
                                JobCEC(0, 1), chain_perm[Edge(0, 1)]));
    EXPECT_EQ(JobCEC(2, 0), GetFirstReactJobWithSuperPeriod(
                                JobCEC(1, 0), chain_perm[Edge(1, 2)]));

    // span super-periods
    EXPECT_EQ(JobCEC(1, 1), GetFirstReactJob(JobCEC(0, 2), chain_perm,
                                             Edge(0, 1), tasks_info));
    EXPECT_EQ(JobCEC(1, 1), GetFirstReactJob(JobCEC(0, 3), chain_perm,
                                             Edge(0, 1), tasks_info));
    EXPECT_EQ(JobCEC(2, 1), GetFirstReactJob(JobCEC(1, 1), chain_perm,
                                             Edge(1, 2), tasks_info));
}

TEST_F(PermutationTest1, ChainsPermutation_v1) {
    // chain is 0 -> 1 -> 2
    TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
    TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);

    ChainsPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[0]);
    EXPECT_EQ(20, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm,
                                       variable_od));

    chain_perm.clear();
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[1]);
    EXPECT_EQ(40, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm,
                                       variable_od));

    chain_perm.clear();
    chain_perm.push_back(perm01[1]);
    chain_perm.push_back(perm12[1]);
    EXPECT_EQ(50, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm,
                                       variable_od));
}

// TEST_F(PermutationTest1, FindFirstReactJob) {
//
//     // chain is 0 -> 1 -> 2
//     EXPECT_EQ(1, FindFirstReactJob(tasks[0], tasks[1], 0));
//     EXPECT_EQ(1, FindFirstReactJob(tasks[0], tasks[1], 1));
//     EXPECT_EQ(1, FindFirstReactJob(tasks[1], tasks[2], 0));
// }

TEST_F(PermutationTest1, GetPossibleReactingJobsLET) {
    // chain is 0 -> 1 -> 2
    EXPECT_EQ(
        1, GetPossibleReactingJobsLET(JobCEC(0, 0), tasks[1], 20, tasks_info)[0]
               .jobId);

    EXPECT_EQ(
        1, GetPossibleReactingJobsLET(JobCEC(0, 1), tasks[1], 20, tasks_info)[0]
               .jobId);

    EXPECT_EQ(
        1, GetPossibleReactingJobsLET(JobCEC(1, 0), tasks[2], 20, tasks_info)[0]
               .jobId);
}

TEST_F(PermutationTest1, GetJobMatch) {
    // chain is 0 -> 1 -> 2
    auto job_match_map01 = GetJobMatch(dag_tasks, tasks_info, 0, 1);
    EXPECT_EQ(1, job_match_map01[JobCEC(0, 0)][0].jobId);
    EXPECT_EQ(1, job_match_map01[JobCEC(0, 1)][0].jobId);
    auto job_match_map10 = GetJobMatch(dag_tasks, tasks_info, 1, 2);
    EXPECT_EQ(1, job_match_map10[JobCEC(1, 0)][0].jobId);
}

TEST_F(PermutationTest1, PerformStandardLETAnalysis) {
    // chain is 0 -> 1 -> 2
    dag_tasks.chains_[0] = {0, 1, 2};
    ScheduleResult res = PerformStandardLETAnalysis<ObjReactionTime>(dag_tasks);
    EXPECT_EQ(60, res.obj_);
}

class PermutationTest_Non_Har : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v2.csv",
            "orig", 1);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
        task0 = tasks[0];
        task1 = tasks[1];
        task2 = tasks[2];

        variable_od = VariableOD(tasks);
        dag_tasks.chains_[0] = {0, 1, 2};
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
    Task task0;
    Task task1;
    Task task2;
    VariableOD variable_od;
};

TEST_F(PermutationTest_Non_Har, ChainsPermutation_v1) {
    // chain is 0 -> 1 -> 2
    TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
    TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);

    ChainsPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[0]);
    perm01[0].print();
    perm12[0].print();
    EXPECT_EQ(15, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm,
                                       variable_od));

    chain_perm.clear();
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[1]);
    perm01[0].print();
    perm12[1].print();
    EXPECT_EQ(30, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm,
                                       variable_od));

    chain_perm.clear();
    chain_perm.push_back(perm01[1]);
    chain_perm.push_back(perm12[1]);
    perm01[1].print();
    perm12[1].print();
    EXPECT_EQ(35, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm,
                                       variable_od));
}

TEST_F(PermutationTest_Non_Har, diff_deadline_from_variable) {
    // chain is 0 -> 1 -> 2
    TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
    TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);

    ChainsPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[0]);
    perm01[0].print();
    perm12[0].print();
    variable_od[2].deadline = 10;

    auto start = std::chrono::high_resolution_clock::now();
    EXPECT_EQ(10, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm,
                                       variable_od));
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken: " << double(duration.count()) / 1e6 << "\n";
}

TEST_F(PermutationTest_Non_Har, GetPossibleReactingJobsLET) {
    // chain is 0 -> 1 -> 2
    EXPECT_EQ(
        1, GetPossibleReactingJobsLET(JobCEC(0, 0), tasks[1], 30, tasks_info)[0]
               .jobId);
    EXPECT_EQ(
        2, GetPossibleReactingJobsLET(JobCEC(0, 1), tasks[1], 30, tasks_info)[0]
               .jobId);
    EXPECT_EQ(
        2, GetPossibleReactingJobsLET(JobCEC(0, 2), tasks[1], 30, tasks_info)[0]
               .jobId);
    EXPECT_EQ(
        1, GetPossibleReactingJobsLET(JobCEC(1, 0), tasks[2], 15, tasks_info)[0]
               .jobId);
}

TEST_F(PermutationTest_Non_Har, PerformStandardLETAnalysis) {
    // chain is 0 -> 1 -> 2
    dag_tasks.chains_[0] = {0, 1, 2};
    ScheduleResult res = PerformStandardLETAnalysis<ObjReactionTime>(dag_tasks);
    EXPECT_EQ(50, res.obj_);
}

class PermutationTest_Non_Har2 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v8.csv",
            "orig", 1);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
        task0 = tasks[0];
        task1 = tasks[1];
        task2 = tasks[2];

        variable_od = VariableOD(tasks);
        dag_tasks.chains_[0] = {0, 1, 2};
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
    Task task0;
    Task task1;
    Task task2;
    VariableOD variable_od;
};

TEST_F(PermutationTest_Non_Har2, GetPossibleReactingJobsLET) {
    // chain is 0 -> 1 -> 2
    EXPECT_EQ(
        2, GetPossibleReactingJobsLET(JobCEC(0, 0), tasks[1], 30, tasks_info)[0]
               .jobId);

    EXPECT_EQ(
        3, GetPossibleReactingJobsLET(JobCEC(0, 1), tasks[1], 30, tasks_info)[0]
               .jobId);
}
TEST_F(PermutationTest_Non_Har2, PerformStandardLETAnalysis) {
    // chain is 0 -> 1 -> 2
    dag_tasks.chains_[0] = {0, 1, 2};
    ScheduleResult res = PerformStandardLETAnalysis<ObjReactionTime>(dag_tasks);
    EXPECT_EQ(40, res.obj_);
}

class PermutationTest_2chain_v1 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v18.csv",
            "orig", 2);
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
        perm03 = TwoTaskPermutations(0, 3, dag_tasks, tasks_info);
        perm34 = TwoTaskPermutations(3, 4, dag_tasks, tasks_info);
        perm13 = TwoTaskPermutations(1, 3, dag_tasks, tasks_info);

        perm01[0].print();
        perm03[0].print();
        perm34[0].print();
        perm13[0].print();
        // perm12[1].print();

        variable_od = VariableOD(tasks);
        dag_tasks.chains_[0] = {0, 3, 4};
        dag_tasks.chains_.push_back({1, 3, 4});
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
    TwoTaskPermutations perm03;
    TwoTaskPermutations perm34;
    TwoTaskPermutations perm13;
    VariableOD variable_od;
};
TEST_F(PermutationTest_2chain_v1, Obj_RT) {
    // chain is 0 -> 3 -> 4
    // chain is 1 -> 3 -> 4
    ChainsPermutation chain_perm;
    chain_perm.push_back(perm03[0]);
    chain_perm.push_back(perm34[0]);
    chain_perm.push_back(perm13[0]);
    EXPECT_EQ(200 + 200, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm,
                                              variable_od));
    dag_tasks.chains_.push_back({0, 1, 3});
    chain_perm.push_back(perm01[0]);
    EXPECT_EQ(200 + 200 + 200, ObjReactionTime::Obj(dag_tasks, tasks_info,
                                                    chain_perm, variable_od));
}

TEST_F(PermutationTest_2chain_v1, PerformStandardLETAnalysis) {
    // chain is 0 -> 3 -> 4
    // chain is 1 -> 3 -> 4
    ScheduleResult res = PerformStandardLETAnalysis<ObjReactionTime>(dag_tasks);
    EXPECT_EQ(600 + 600, res.obj_);
}

// class PermutationTest_long_time : public ::testing::Test {
//    protected:
//     void SetUp() override {
//         dag_tasks = ReadDAG_Tasks(
//             GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v24.csv",
//             "RM", 1);
//         tasks = dag_tasks.GetTaskSet();
//         tasks_info = TaskSetInfoDerived(tasks);
//         task0 = tasks[0];
//         task1 = tasks[1];
//         task2 = tasks[2];
//         job00 = JobCEC(0, 0);
//         job01 = JobCEC(0, 1);
//         job10 = JobCEC(1, 0);
//         job20 = JobCEC(2, 0);

//         variable_od = VariableOD(tasks);
//     };

//     DAG_Model dag_tasks;
//     TaskSet tasks;
//     TaskSetInfoDerived tasks_info;
//     Task task0;
//     Task task1;
//     Task task2;
//     JobCEC job00;
//     JobCEC job01;
//     JobCEC job10;
//     JobCEC job20;

//     VariableOD variable_od;
// };
// TEST_F(PermutationTest_long_time, ObjReactionTime) {
//     // chain is 0 -> 1 -> 4
//     TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
//     TwoTaskPermutations perm14(1, 4, dag_tasks, tasks_info);

//     ChainsPermutation chain_perm;
//     chain_perm.push_back(perm01[0]);
//     chain_perm.push_back(perm14[0]);
//     perm01[0].print();
//     perm14[0].print();

//     auto start = std::chrono::high_resolution_clock::now();
//     EXPECT_EQ(20000 - 5470, ObjReactionTime::Obj(dag_tasks, tasks_info,
//                                                  chain_perm, variable_od));
//     auto stop = std::chrono::high_resolution_clock::now();
//     auto duration =
//         std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
//     std::cout << "Time taken: " << double(duration.count()) / 1e6 << "\n";
//     PrintTimer();
// }
int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}