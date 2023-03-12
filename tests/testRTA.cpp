#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/RTA/RTA_LL.h"
#include "sources/TaskModel/DAG_Model.h"
using namespace DAG_SPACE;

class PermutationTest1 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v3.csv",
            "orig", 1);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
};

TEST_F(PermutationTest1, v1) {
    EXPECT_EQ(12, GetResponseTime(tasks, 0));
    EXPECT_EQ(265, GetResponseTime(tasks, 1));
    EXPECT_EQ(282, GetResponseTime(tasks, 2));
}

class PermutationTest2 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v10.csv",
            "orig", 1);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
};

TEST_F(PermutationTest2, v1) {
    EXPECT_EQ(10, GetResponseTime(tasks, 0));
    EXPECT_EQ(21, GetResponseTime(tasks, 1));
    EXPECT_EQ(33, GetResponseTime(tasks, 2));
    EXPECT_EQ(46, GetResponseTime(tasks, 3));
    EXPECT_EQ(60, GetResponseTime(tasks, 4));
}

class PermutationTest3 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v4.csv",
            "RM", 1);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
};

TEST_F(PermutationTest3, v1) {
    EXPECT_EQ(2, GetResponseTime(tasks, 0));
    EXPECT_EQ(3, GetResponseTime(tasks, 1));
    EXPECT_EQ(6, GetResponseTime(tasks, 2));
}

class PermutationTest4 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v7.csv",
            "orig", 1);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
};

TEST_F(PermutationTest4, constructor_dag) {
    EXPECT_EQ(0, dag_tasks.task_id2task_index_within_processor_[0]);
    EXPECT_EQ(1, dag_tasks.task_id2task_index_within_processor_[1]);
    EXPECT_EQ(0, dag_tasks.task_id2task_index_within_processor_[2]);

    EXPECT_EQ(1, dag_tasks.processor2taskset_[0].size());
    EXPECT_EQ(2, dag_tasks.processor2taskset_[1].size());

    EXPECT_EQ(0, dag_tasks.processor2taskset_[0][0].processorId);
    EXPECT_EQ(3, dag_tasks.processor2taskset_[0][0].executionTime);
}

TEST_F(PermutationTest4, v1) {
    EXPECT_EQ(1, GetResponseTime(dag_tasks, 0));
    EXPECT_EQ(3, GetResponseTime(dag_tasks, 1));
    EXPECT_EQ(3, GetResponseTime(dag_tasks, 2));
}

class PermutationTest5 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v1.csv",
            "orig", 1);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
};

TEST_F(PermutationTest5, v1) {
    EXPECT_EQ(10, GetResponseTime(dag_tasks, 0));
    EXPECT_EQ(11, GetResponseTime(dag_tasks, 1));
    EXPECT_EQ(24, GetResponseTime(dag_tasks, 3));
    EXPECT_EQ(12, GetResponseTime(dag_tasks, 2));
    EXPECT_EQ(26, GetResponseTime(dag_tasks, 4));
}

class PermutationTest6 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v1.csv",
            "RM", 1);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
};

TEST_F(PermutationTest6, GetTask) {
    EXPECT_EQ(0, dag_tasks.GetTaskIndex(0));
    EXPECT_EQ(3, dag_tasks.GetTaskIndex(1));
    EXPECT_EQ(2, dag_tasks.GetTaskIndex(2));
    EXPECT_EQ(1, dag_tasks.GetTaskIndex(3));
    EXPECT_EQ(4, dag_tasks.GetTaskIndex(4));
}

TEST_F(PermutationTest6, v1) {
    EXPECT_EQ(10, GetResponseTime(dag_tasks, 0));
    EXPECT_EQ(24, GetResponseTime(dag_tasks, 1));
    EXPECT_EQ(12, GetResponseTime(dag_tasks, 2));
    EXPECT_EQ(13, GetResponseTime(dag_tasks, 3));
    EXPECT_EQ(26, GetResponseTime(dag_tasks, 4));
}

int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}