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
        tasks = dag_tasks.tasks;
        tasks_info = TaskSetInfoDerived(tasks);
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
};

int GetResponseTime(const TaskSet& tasks, int index, int warm_start = 0) {
    RTA_LL r(tasks);
    if (warm_start < tasks[index].executionTime)
        warm_start = tasks[index].executionTime;
    return r.RTA_Common_Warm(warm_start, index);
}
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
        tasks = dag_tasks.tasks;
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

int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}