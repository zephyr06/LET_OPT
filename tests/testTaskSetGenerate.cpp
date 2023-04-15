// #include <CppUnitLite/TestHarness.h>
#include "gmock/gmock.h"  // Brings in gMock.
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
  string path = GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v8.csv";
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
  EXPECT_TRUE(dag_tasks.chains_.size() > 0);
  std::cout << dag_tasks.chains_.size() << std::endl;
}

TEST(DAG_Model, mapPrev) {
  string path = GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v4.csv";
  DAG_Model dag_tasks = ReadDAG_Tasks(path, "RM", 1);
  EXPECT_EQ(1, dag_tasks.mapPrev[0].size());
  EXPECT_EQ(2, dag_tasks.mapPrev[1].size());
  EXPECT_EQ(1, dag_tasks.mapPrev[2].size());
}

TEST(DAG_MODEL, generate_chains) {
  string path = GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v4.csv";
  DAG_Model dag_tasks =
      ReadDAG_Tasks(path, "RM", GlobalVariablesDAGOpt::CHAIN_NUMBER);
  dag_tasks.chains_ =
      dag_tasks.GetRandomChains(GlobalVariablesDAGOpt::CHAIN_NUMBER);
  EXPECT_TRUE(dag_tasks.chains_.size() > 0);
  EXPECT_TRUE(GlobalVariablesDAGOpt::CHAIN_NUMBER > 0);
  EXPECT_EQ(GlobalVariablesDAGOpt::CHAIN_NUMBER, dag_tasks.chains_.size());
}

TEST(DAG_MODEL, WhetherDAGChainsShareNodes) {
  string path = GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v4.csv";
  DAG_Model dag_tasks =
      ReadDAG_Tasks(path, "RM", GlobalVariablesDAGOpt::CHAIN_NUMBER);
  dag_tasks.chains_ = {{0, 1}, {2, 3}};
  EXPECT_TRUE(!WhetherDAGChainsShareNodes(dag_tasks));

  dag_tasks.chains_ = {{0, 1}, {1, 3}};
  EXPECT_TRUE(WhetherDAGChainsShareNodes(dag_tasks));

  dag_tasks.chains_ = {{0, 1}, {4, 5, 1}};
  EXPECT_TRUE(WhetherDAGChainsShareNodes(dag_tasks));
}

TEST(DAG_Model, read_chains) {
  string path =
      GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v20.csv";
  DAG_Model dag_tasks =
      ReadDAG_Tasks(path, "RM", GlobalVariablesDAGOpt::CHAIN_NUMBER);
  std::vector<int> chain0 = {0, 1, 4};
  std::vector<int> chain1 = {0, 1, 3, 2};
  AssertEqualVectorExact<int>(chain0, dag_tasks.chains_[0], 0, __LINE__);

  AssertEqualVectorExact<int>(chain1, dag_tasks.chains_[1], 0, __LINE__);
}

TEST(DAG_Model, GetHyperPeriod) {
  string path =
      GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v20.csv";
  DAG_Model dag_tasks =
      ReadDAG_Tasks(path, "RM", GlobalVariablesDAGOpt::CHAIN_NUMBER);
  const TaskSetInfoDerived tasks_info(dag_tasks.GetTaskSet());
  EXPECT_EQ(100, GetHyperPeriod(tasks_info, {0}));
  EXPECT_EQ(500, GetHyperPeriod(tasks_info, {0, 1}));
  EXPECT_EQ(1000, GetHyperPeriod(tasks_info, {0, 1, 4}));
  EXPECT_EQ(100, GetHyperPeriod(tasks_info, {0, 3}));
}

TEST(DAG_Model, ReadDAG_Tasks_chain_number) {
  string path =
      GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v42.csv";
  DAG_Model dag_tasks = ReadDAG_Tasks(path, "RM", 1);

  EXPECT_EQ(1, dag_tasks.chains_.size());
}

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}