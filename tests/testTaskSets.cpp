
#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/RTA/RTA_LL.h"
#include "sources/TaskModel/DAG_Model.h"
using namespace DAG_SPACE;

class PermutationTest1 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v4.csv",
            "RM", 1);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
};
class PermutationTest2 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v11.csv",
            "RM", 1);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
};
TEST_F(PermutationTest2, read) {
    EXPECT_EQ(2, dag_tasks.GetTask(2).id);
    EXPECT_EQ(200, dag_tasks.GetTask(2).period);
    EXPECT_EQ(6, dag_tasks.GetTask(2).executionTime);
    EXPECT_EQ(200, dag_tasks.GetTask(2).deadline);
    EXPECT_EQ(0, dag_tasks.GetTask(2).processorId);
    EXPECT_EQ(1, dag_tasks.GetTask(4).processorId);
}

TEST_F(PermutationTest1, mapPrev) {
    EXPECT_EQ(1, dag_tasks.mapPrev[0].size());
    EXPECT_EQ(2, dag_tasks.mapPrev[1].size());
    EXPECT_EQ(1, dag_tasks.mapPrev[2].size());
}

int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}