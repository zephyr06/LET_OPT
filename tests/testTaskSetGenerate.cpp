#include <CppUnitLite/TestHarness.h>

#include "sources/TaskModel/GenerateRandomTaskset.h"
#include "sources/Utils/testMy.h"

using namespace std;
using namespace GlobalVariablesDAGOpt;
TEST(ExtractVariable, v1) {
    int N = 5;
    double sumUtil = 0.6;
    auto sth = Uunifast(N, sumUtil);
    double all = 0;
    for (int i = 0; i < N; i++) all += sth[i];
    AssertEqualScalar(sumUtil, all);
}

TEST(ReadDAG_Tasks, V1) {
    string path =
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v8.csv";
    DAG_Model dm = ReadDAG_Tasks(path);
    dm.print();
    AssertBool(true, dm.mapPrev[3].size() == 3);
    AssertBool(true, dm.mapPrev[4][0].id == 3);
}

TEST(ReadDAG_Tasks, v3) {
    string path =
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v41.csv";
    DAG_Model dm = ReadDAG_Tasks(path);
    dm.print();
    AssertEqualScalar(14.38, dm.GetTask(0).executionTime);
    AssertEqualScalar(0.223, dm.GetTask(2).executionTime);
}

TEST(ReadDAG_Tasks, v2) {
    string path =
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v15.csv";
    DAG_Model dm = ReadDAG_Tasks(path);
    dm.print();
    AssertBool(true, dm.mapPrev[1].size() == 2);
    AssertBool(true, dm.mapPrev[2][0].id == 4);
    AssertBool(true, dm.mapPrev[0][0].id == 4);
}
TEST(ReadDAG_Tasks, v5) {
    string path =
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v45.csv";
    DAG_Model dm = ReadDAG_Tasks(path, "orig");
    dm.print();
    for (int i = 0; i < 3; i++) AssertEqualScalar(i, dm.GetTask(i).id);
    AssertEqualScalar(100, dm.GetTask(2).deadline);
}
TEST(ReadDAG_Tasks, v7) {
    string path =
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v45.csv";
    DAG_Model dm = ReadDAG_Tasks(path, "orig");
    dm.print();
    for (int i = 0; i < 3; i++) AssertEqualScalar(0, dm.GetTask(i).taskType);
}

TEST(TaskSetInfoDerived, v1) {
    string path =
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v45.csv";
    DAG_Model dm = ReadDAG_Tasks(path, "orig");
    TaskSet tasks = dm.GetTaskSet();
    TaskSetInfoDerived tasksInfo(tasks);
    AssertEqualScalar(10, tasksInfo.GetTask(0).executionTime);
}

TEST(GetRandomChains, DAG_Model) {
    string path =
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v74.csv";
    DAG_Model dag_tasks = ReadDAG_Tasks(path, "orig");
    EXPECT(dag_tasks.chains_.size() > 0);
    std::cout << dag_tasks.chains_.size() << std::endl;
}
TEST(dag_model, GetSfBound) {
    string path =
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n30_v3.csv";
    DAG_Model dag_tasks = ReadDAG_Tasks(path, "orig");
    EXPECT_LONGS_EQUAL(6409, dag_tasks.GetSfBound());
    EXPECT_LONGS_EQUAL(253533, dag_tasks.GetRtdaBound());
}

TEST(DAG_Model, mapPrev) {
    string path =
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v4.csv";
    DAG_Model dag_tasks = ReadDAG_Tasks(path, "RM", 1);
    EXPECT_LONGS_EQUAL(1, dag_tasks.mapPrev[0].size());
    EXPECT_LONGS_EQUAL(2, dag_tasks.mapPrev[1].size());
    EXPECT_LONGS_EQUAL(1, dag_tasks.mapPrev[2].size());
}

TEST(DAG_MODEL, generate_chains) {
    string path =
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v4.csv";
    DAG_Model dag_tasks =
        ReadDAG_Tasks(path, "RM", GlobalVariablesDAGOpt::CHAIN_NUMBER);
    dag_tasks.chains_ = dag_tasks.GetRandomChains(1);
    EXPECT(dag_tasks.chains_.size() > 0)
    EXPECT(GlobalVariablesDAGOpt::CHAIN_NUMBER > 0);
    EXPECT_LONGS_EQUAL(GlobalVariablesDAGOpt::CHAIN_NUMBER,
                       dag_tasks.chains_.size());
}
int main() {
    TestResult tr;
    return TestRegistry::runAllTests(tr);
}
