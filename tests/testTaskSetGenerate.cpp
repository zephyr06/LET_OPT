#include <CppUnitLite/TestHarness.h>
#include "sources/TaskModel/GenerateRandomTaskset.h"
#include "sources/Utils/testMy.h"

using namespace std;
using namespace GlobalVariablesDAGOpt;
TEST(ExtractVariable, v1)
{
    int N = 5;
    double sumUtil = 0.6;
    auto sth = Uunifast(N, sumUtil);
    double all = 0;
    for (int i = 0; i < N; i++)
        all += sth[i];
    AssertEqualScalar(sumUtil, all);
}

TEST(ReadDAG_Tasks, V1)
{
    string path = GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v8.csv";
    DAG_Model dm = ReadDAG_Tasks(path);
    dm.print();
    AssertBool(true, dm.mapPrev[3].size() == 3);
    AssertBool(true, dm.mapPrev[4][0].id == 3);
}

TEST(ReadDAG_Tasks, v3)
{
    string path = GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v41.csv";
    DAG_Model dm = ReadDAG_Tasks(path);
    dm.print();
    AssertEqualScalar(14.38, dm.tasks[0].executionTime);
    AssertEqualScalar(0.223, dm.tasks[2].executionTime);
}

TEST(ReadDAG_Tasks, v2)
{
    string path = GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v15.csv";
    DAG_Model dm = ReadDAG_Tasks(path);
    dm.print();
    AssertBool(true, dm.mapPrev[1].size() == 2);
    AssertBool(true, dm.mapPrev[2][0].id == 4);
    AssertBool(true, dm.mapPrev[0][0].id == 4);
}
TEST(ReadDAG_Tasks, v4)
{
    string path = GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v1.csv";
    DAG_Model dm = ReadDAG_Tasks(path, "orig");
    dm.print();
    AssertEqualScalar(1, dm.tasks[0].period);
    for (int i = 0; i < 3; i++)
        AssertEqualScalar(i, dm.tasks[i].id);
    AssertEqualScalar(53.7653, dm.tasks[2].executionTime);
    AssertEqualScalar(200, dm.tasks[2].deadline);
}
TEST(ReadDAG_Tasks, v5)
{
    string path = GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v45.csv";
    DAG_Model dm = ReadDAG_Tasks(path, "orig");
    dm.print();
    for (int i = 0; i < 3; i++)
        AssertEqualScalar(i, dm.tasks[i].id);
    AssertEqualScalar(4, dm.tasks[2].coreRequire);
    AssertEqualScalar(100, dm.tasks[2].deadline);
}
TEST(ReadDAG_Tasks, v6)
{
    string path = GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v49.csv";
    DAG_Model dm = ReadDAG_Tasks(path, "orig");
    dm.print();
    for (int i = 0; i < 3; i++)
        AssertEqualScalar(i, dm.tasks[i].id);
    AssertEqualScalar(1, dm.tasks[0].taskType);
    AssertEqualScalar(1, dm.tasks[1].taskType);
    AssertEqualScalar(0, dm.tasks[2].taskType);
}
TEST(ReadDAG_Tasks, v7)
{
    string path = GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v45.csv";
    DAG_Model dm = ReadDAG_Tasks(path, "orig");
    dm.print();
    for (int i = 0; i < 3; i++)
        AssertEqualScalar(0, dm.tasks[i].taskType);
}

TEST(TaskSetInfoDerived, v1)
{
    string path = GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v45.csv";
    DAG_Model dm = ReadDAG_Tasks(path, "orig");
    TaskSet tasks = dm.tasks;
    TaskSetInfoDerived tasksInfo(tasks);
    AssertEqualScalar(10, tasksInfo.tasks[0].executionTime);
}

TEST(GetRandomChains, DAG_Model)
{
    string path = GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v74.csv";
    DAG_Model dagTasks = ReadDAG_Tasks(path, "orig");
    EXPECT(dagTasks.chains_.size() > 0);
    std::cout << dagTasks.chains_.size() << std::endl;
}
TEST(dag_model, GetSfBound)
{
    string path = GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n30_v3.csv";
    DAG_Model dagTasks = ReadDAG_Tasks(path, "orig");
    EXPECT_LONGS_EQUAL(6409, dagTasks.GetSfBound());
    EXPECT_LONGS_EQUAL(253533, dagTasks.GetRtdaBound());
}
int main()
{
    TestResult tr;
    return TestRegistry::runAllTests(tr);
}
