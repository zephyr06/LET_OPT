
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

class PermutationTest33_n3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v33");
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
TEST_F(PermutationTest33_n3, GetChainsForSF) {
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

TEST_F(PermutationTest33_n3, SF_Obj_Implicit) {
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
  ChainsPermutation chains_perm = GetChainsPermFromVariable(
      dag_tasks, tasks_info, GetChainsForSF(dag_tasks, tasks_info), "DataAge",
      schedule);
  EXPECT_EQ(1,
            ObjSensorFusion::Obj(dag_tasks, tasks_info, chains_perm, schedule));
}

TEST_F(PermutationTest33_n3, SF_Obj_StanLET) {
  VariableOD variable_od_let(tasks);
  Schedule schedule_actual = Variable2Schedule(tasks_info, variable_od_let);
  ChainsPermutation chains_perm = GetChainsPermFromVariable(
      dag_tasks, tasks_info, dag_tasks.chains_, "DataAge", schedule_actual);
  EXPECT_EQ(0, ObjSensorFusion::Obj(dag_tasks, tasks_info, chains_perm,
                                    variable_od_let));
}

TEST_F(PermutationTest30_n3, SF_Obj_Implicit) {
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
  ChainsPermutation chains_perm = GetChainsPermFromVariable(
      dag_tasks, tasks_info, GetChainsForSF(dag_tasks, tasks_info), "DataAge",
      schedule);
  EXPECT_EQ(1 + 1,
            ObjSensorFusion::Obj(dag_tasks, tasks_info, chains_perm, schedule));
}

TEST_F(PermutationTest30_n3, SF_Obj_StanLET) {
  VariableOD variable_od_let(tasks);
  Schedule schedule_actual = Variable2Schedule(tasks_info, variable_od_let);
  ChainsPermutation chains_perm = GetChainsPermFromVariable(
      dag_tasks, tasks_info, GetChainsForSF(dag_tasks, tasks_info), "DataAge",
      schedule_actual);
  EXPECT_EQ(0, ObjSensorFusion::Obj(dag_tasks, tasks_info, chains_perm,
                                    variable_od_let));
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

class PermutationTest_n6_v1 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n6_v1");
    type_trait = "SensorFusion";
  }

 public:
  std::string type_trait;
};

TEST_F(PermutationTest_n6_v1, Optimize) {
  TwoTaskPermutations perm05(0, 5, dag_tasks, tasks_info, "DataAge");
  TwoTaskPermutations perm45(4, 5, dag_tasks, tasks_info, "DataAge");

  ChainsPermutation chains_perm;
  chains_perm.push_back(perm05[0]);
  chains_perm.push_back(perm45[0]);
  chains_perm.print();

  GraphOfChains graph_chains(dag_tasks.chains_);

  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  PrintRta(rta);
  LPOptimizer lp_optimizer(dag_tasks, tasks_info, graph_chains, "SensorFusion",
                           rta);
  auto res = lp_optimizer.Optimize(chains_perm);
  EXPECT_EQ(0, res.second);
}
TEST_F(PermutationTest_n6_v1, SF_Obj_StanLET) {
  VariableOD variable_od_let(tasks);
  Schedule schedule_actual = Variable2Schedule(tasks_info, variable_od_let);
  ChainsPermutation chains_perm = GetChainsPermFromVariable(
      dag_tasks, tasks_info, GetChainsForSF(dag_tasks, tasks_info), "DataAge",
      schedule_actual);
  std::cout << ObjSensorFusion::Obj(dag_tasks, tasks_info, chains_perm,
                                    variable_od_let)
            << "\n";
}
class PermutationTest_n3_v32 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v32");
    type_trait = "SensorFusion";
  }

 public:
  std::string type_trait;
};
TEST_F(PermutationTest_n3_v32, Optimize) {
  TwoTaskPermutations perm02(0, 2, dag_tasks, tasks_info, "DataAge");
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info, "DataAge");

  ChainsPermutation chains_perm;
  chains_perm.push_back(perm02[0]);
  chains_perm.push_back(perm12[0]);
  chains_perm.print();

  GraphOfChains graph_chains(dag_tasks.chains_);

  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  PrintRta(rta);
  LPOptimizer lp_optimizer(dag_tasks, tasks_info, graph_chains, "SensorFusion",
                           rta);
  auto res = lp_optimizer.Optimize(chains_perm);
  EXPECT_EQ(5, res.second);  // From Pycharm's Gurobi
}
int main(int argc, char **argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}