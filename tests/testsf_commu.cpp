
#include <algorithm>  // for std::gcd

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Baseline/JobCommunications.h"
#include "sources/Baseline/Martinez18.h"
#include "sources/Baseline/StandardLET.h"
#include "sources/Optimization/Variable.h"
#include "testEnv.cpp"
using namespace DAG_SPACE;

class PermutationTest53_n5 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n5_v53"); }
};

TEST_F(PermutationTest53_n5, ReadSF_Fork) {
  EXPECT_EQ(2, dag_tasks.sf_forks_.size());
  EXPECT_EQ(0, dag_tasks.sf_forks_[0].source[0]);
  EXPECT_EQ(1, dag_tasks.sf_forks_[0].source[1]);
  EXPECT_EQ(2, dag_tasks.sf_forks_[0].sink);

  EXPECT_EQ(0, dag_tasks.sf_forks_[1].source[0]);
  EXPECT_EQ(1, dag_tasks.sf_forks_[1].source[1]);
  EXPECT_EQ(3, dag_tasks.sf_forks_[1].sink);
}
class PermutationTest54_n5 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n5_v54"); }
};

TEST_F(PermutationTest54_n5, ReadSF_Fork) {
  EXPECT_EQ(2, dag_tasks.sf_forks_.size());
  EXPECT_EQ(1, dag_tasks.sf_forks_[0].source[0]);
  EXPECT_EQ(2, dag_tasks.sf_forks_[0].source[1]);
  EXPECT_EQ(3, dag_tasks.sf_forks_[0].sink);

  EXPECT_EQ(0, dag_tasks.sf_forks_[1].source[0]);
  EXPECT_EQ(3, dag_tasks.sf_forks_[1].source[1]);
  EXPECT_EQ(4, dag_tasks.sf_forks_[1].sink);
}

class PermutationTest29_n3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v29");
    type_trait = "SensorFusion";
  }

 public:
  std::string type_trait;
};
class PermutationTest30_n3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v30");
    type_trait = "SensorFusion";
  }

 public:
  std::string type_trait;
};
std::vector<std::vector<int>> GetChainsForSF(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info) {
  std::vector<std::vector<int>> chains;
  std::unordered_set<Edge> edge_record;
  for (const auto &fork_curr : dag_tasks.sf_forks_) {
    int sink_task_id = fork_curr.sink;
    const std::vector<int> &source_tasks = fork_curr.source;
    for (int source_id : source_tasks) {
      Edge edge_curr(source_id, sink_task_id);
      if (edge_record.find(edge_curr) == edge_record.end()) {
        edge_record.insert(edge_curr);
        chains.push_back({source_id, sink_task_id});
      }
    }
  }
  return chains;
}
TEST_F(PermutationTest29_n3, GetChainsForSF) {
  auto chains = GetChainsForSF(dag_tasks, tasks_info);
  EXPECT_EQ(2, chains.size());
  EXPECT_EQ(1, chains[0][0]);
  EXPECT_EQ(3, chains[0][1]);
  EXPECT_EQ(2, chains[1][0]);
  EXPECT_EQ(3, chains[1][1]);
}
TEST_F(PermutationTest30_n3, GetChainsForSF) {
  auto chains = GetChainsForSF(dag_tasks, tasks_info);
  EXPECT_EQ(4, chains.size());
  EXPECT_EQ(1, chains[0][0]);
  EXPECT_EQ(3, chains[0][1]);
  EXPECT_EQ(2, chains[1][0]);
  EXPECT_EQ(3, chains[1][1]);

  EXPECT_EQ(0, chains[2][0]);
  EXPECT_EQ(4, chains[2][1]);
  EXPECT_EQ(3, chains[3][0]);
  EXPECT_EQ(4, chains[3][1]);
}

TEST_F(PermutationTest29_n3, SF_Obj_Implicit) {
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
  ChainsPermutation chains_perm = GetChainsPermFromVariable(
      dag_tasks, tasks_info, GetChainsForSF(dag_tasks, tasks_info), "DataAge",
      schedule);
  EXPECT_EQ(1,
            ObjSensorFusion::Obj(dag_tasks, tasks_info, chains_perm, schedule));
}
TEST_F(PermutationTest30_n3, SF_Obj_Implicit) {
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
  ChainsPermutation chains_perm = GetChainsPermFromVariable(
      dag_tasks, tasks_info, GetChainsForSF(dag_tasks, tasks_info), "DataAge",
      schedule);
  EXPECT_EQ(1 + 1,
            ObjSensorFusion::Obj(dag_tasks, tasks_info, chains_perm, schedule));
}

class PermutationTest31_n3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v31");
    type_trait = "SensorFusion";
  }

 public:
  std::string type_trait;
};
TEST_F(PermutationTest31_n3, SF_Obj_Implicit) {
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
  ChainsPermutation chains_perm = GetChainsPermFromVariable(
      dag_tasks, tasks_info, GetChainsForSF(dag_tasks, tasks_info), "DataAge",
      schedule);
  EXPECT_EQ(1 - (3 - 20),
            ObjSensorFusion::Obj(dag_tasks, tasks_info, chains_perm, schedule));
}
int main(int argc, char **argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}