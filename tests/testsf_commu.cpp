
#include <algorithm>  // for std::gcd

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Baseline/JobCommunications.h"
#include "sources/Baseline/Martinez18.h"
#include "sources/Baseline/StandardLET.h"
#include "sources/Optimization/Variable.h"
#include "tests/testEnv.cpp"
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
    dag_tasks.chains_ = GetChainsForSF(dag_tasks);
  }

 public:
  std::string type_trait;
};
class PermutationTest30_n3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v30");
    type_trait = "SensorFusion";
    dag_tasks.chains_ = GetChainsForSF(dag_tasks);
  }

 public:
  std::string type_trait;
};
TEST_F(PermutationTest33_n3, GetChainsForSF) {
  auto chains = GetChainsForSF(dag_tasks);
  EXPECT_EQ(2, chains.size());
  EXPECT_EQ(1, chains[0][0]);
  EXPECT_EQ(3, chains[0][1]);
  EXPECT_EQ(2, chains[1][0]);
  EXPECT_EQ(3, chains[1][1]);
}
TEST_F(PermutationTest30_n3, GetChainsForSF) {
  auto chains = GetChainsForSF(dag_tasks);
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
      dag_tasks, tasks_info, GetChainsForSF(dag_tasks), "DataAge", schedule);
  EXPECT_EQ(1, ObjSensorFusion::Obj(dag_tasks, tasks_info, chains_perm,
                                    schedule, {{}}));
}

TEST_F(PermutationTest33_n3, SF_Obj_StanLET) {
  VariableOD variable_od_let(tasks);
  Schedule schedule_actual = Variable2Schedule(tasks_info, variable_od_let);
  ChainsPermutation chains_perm = GetChainsPermFromVariable(
      dag_tasks, tasks_info, dag_tasks.chains_, "DataAge", schedule_actual);
  EXPECT_EQ(0, ObjSensorFusion::Obj(dag_tasks, tasks_info, chains_perm,
                                    variable_od_let, {{}}));
}

TEST_F(PermutationTest30_n3, SF_Obj_Implicit) {
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
  ChainsPermutation chains_perm = GetChainsPermFromVariable(
      dag_tasks, tasks_info, GetChainsForSF(dag_tasks), "DataAge", schedule);
  EXPECT_EQ(18 + 1, ObjSensorFusion::Obj(dag_tasks, tasks_info, chains_perm,
                                         schedule, {{}}));
}

TEST_F(PermutationTest30_n3, SF_Obj_StanLET) {
  VariableOD variable_od_let(tasks);
  Schedule schedule_actual = Variable2Schedule(tasks_info, variable_od_let);
  ChainsPermutation chains_perm = GetChainsPermFromVariable(
      dag_tasks, tasks_info, GetChainsForSF(dag_tasks), "DataAge",
      schedule_actual);
  EXPECT_EQ(0, ObjSensorFusion::Obj(dag_tasks, tasks_info, chains_perm,
                                    variable_od_let, {{}}));
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
      dag_tasks, tasks_info, GetChainsForSF(dag_tasks), "DataAge", schedule);
  EXPECT_EQ(1, ObjSensorFusion::Obj(dag_tasks, tasks_info, chains_perm,
                                    schedule, {{}}));
}

class PermutationTest_n6_v1 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n6_v1");
    type_trait = "SensorFusion";
    dag_tasks.chains_ = GetChainsForSF(dag_tasks);
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
      dag_tasks, tasks_info, GetChainsForSF(dag_tasks), "DataAge",
      schedule_actual);
  std::cout << ObjSensorFusion::Obj(dag_tasks, tasks_info, chains_perm,
                                    variable_od_let, {{}})
            << "\n";
}
class PermutationTest_n3_v32 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v32");
    type_trait = "SensorFusion";
    dag_tasks.chains_ = GetChainsForSF(dag_tasks);
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

TEST_F(PermutationTest_n3_v32, SF_Obj_StanLET) {
  ScheduleResult res;
  res = PerformStandardLETAnalysis<DAG_SPACE::ObjSensorFusion>(dag_tasks);
  EXPECT_EQ(10, res.obj_);
}
TEST_F(PermutationTest_n3_v32, SF_Obj_Implicit) {
  ScheduleResult res;
  res = PerformImplicitCommuAnalysis<DAG_SPACE::ObjSensorFusion>(dag_tasks);
  EXPECT_EQ(9, res.obj_);
}
TEST_F(PermutationTest_n3_v32, SF_Obj_TOM_OPT_BF) {
  ScheduleResult res;
  res = PerformTOM_OPT_BF<DAG_SPACE::ObjSensorFusion>(dag_tasks);
  EXPECT_EQ(5, res.obj_);
}
TEST_F(PermutationTest_n3_v32, SF_Obj_TOM_OPT_EnumW_Skip) {
  ScheduleResult res;
  res = PerformTOM_OPT_EnumW_Skip<DAG_SPACE::ObjSensorFusion>(dag_tasks);
  EXPECT_EQ(5, res.obj_);
}

class PermutationTest_n3_v34 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v34");
    type_trait = "SensorFusion";
    dag_tasks.chains_ = GetChainsForSF(dag_tasks);
  }

 public:
  std::string type_trait;
};

TEST_F(PermutationTest_n3_v34, GetSF_JobFork) {
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
  SF_JobFork fork = GetSF_JobFork(JobCEC(2, 0), {0, 1}, tasks_info, schedule);
  EXPECT_EQ(JobCEC(0, 0), fork.source_jobs[0]);
  EXPECT_EQ(JobCEC(1, 1), fork.source_jobs[1]);
}
class PermutationTest_n3_v35 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v35");
    dag_tasks.chains_ = {{0, 1, 2}};
  }

 public:
  std::string type_trait;
};
TEST_F(PermutationTest_n3_v35, DataAgeObj) {
  ScheduleResult res;
  res = PerformImplicitCommuAnalysis<DAG_SPACE::ObjDataAge>(dag_tasks);
  EXPECT_EQ(36, res.obj_);
}
TEST_F(PermutationTest_n3_v34, SF_Obj_StanLET) {
  ScheduleResult res;
  res = PerformStandardLETAnalysis<DAG_SPACE::ObjSensorFusion>(dag_tasks);
  EXPECT_EQ(10, res.obj_);
}
TEST_F(PermutationTest_n3_v34, SF_Obj_Implicit) {
  ScheduleResult res;
  res = PerformImplicitCommuAnalysis<DAG_SPACE::ObjSensorFusion>(dag_tasks);
  EXPECT_EQ(1, res.obj_);
}
TEST_F(PermutationTest_n3_v34, SF_Obj_TOM_OPT_BF) {
  ScheduleResult res;
  res = PerformTOM_OPT_BF<DAG_SPACE::ObjSensorFusion>(dag_tasks);
  EXPECT_EQ(5, res.obj_);
}
TEST_F(PermutationTest_n3_v34, SF_Obj_TOM_OPT_EnumW_Skip) {
  ScheduleResult res;
  res = PerformTOM_OPT_EnumW_Skip<DAG_SPACE::ObjSensorFusion>(dag_tasks);
  EXPECT_EQ(5, res.obj_);
}

class PermutationTest_n4_v2 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n4_v2");
    type_trait = "SensorFusion";
    dag_tasks.chains_ = GetChainsForSF(dag_tasks);
  }

 public:
  std::string type_trait;
};
TEST_F(PermutationTest_n4_v2, SF_Obj_StanLET) {
  ScheduleResult res;
  res = PerformStandardLETAnalysis<DAG_SPACE::ObjSensorFusion>(dag_tasks);
  EXPECT_EQ(20, res.obj_);
}
TEST_F(PermutationTest_n4_v2, SF_Obj_Implicit) {
  ScheduleResult res;
  res = PerformImplicitCommuAnalysis<DAG_SPACE::ObjSensorFusion>(dag_tasks);
  EXPECT_EQ(14, res.obj_);
}
TEST_F(PermutationTest_n4_v2, SF_Obj_TOM_OPT_BF) {
  ScheduleResult res;
  res = PerformTOM_OPT_BF<DAG_SPACE::ObjSensorFusion>(dag_tasks);
  std::cout << "SF_Obj_TOM_OPT_BF on n4_v2: " << res.obj_ << std::endl;
  EXPECT_EQ(11, res.obj_);
}
TEST_F(PermutationTest_n4_v2, SF_Obj_TOM_OPT_EnumW_Skip) {
  ScheduleResult res;
  res = PerformTOM_OPT_EnumW_Skip<DAG_SPACE::ObjSensorFusion>(dag_tasks);
  std::cout << "SF_Obj_TOM_OPT_EnumW_Skip on n4_v2: " << res.obj_ << std::endl;
  EXPECT_EQ(11, res.obj_);
}

class PermutationTest_n5_v58 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n5_v58"); }
};
TEST_F(PermutationTest_n5_v58, implicit_obj) {
  ScheduleResult res =
      PerformImplicitCommuAnalysis<DAG_SPACE::ObjReactionTime>(dag_tasks);
}

class PermutationTest_n10_v10 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n10_v10"); }
};

TEST_F(PermutationTest_n10_v10, ForkIntersect) {
  auto forkss = ExtractDecoupledForks(dag_tasks.sf_forks_);
  EXPECT_FALSE(ForkIntersect(dag_tasks.sf_forks_[0], dag_tasks.sf_forks_[1]));
}
TEST_F(PermutationTest_n10_v10, ExtractDecoupledForks) {
  auto forkss = ExtractDecoupledForks(dag_tasks.sf_forks_);
  EXPECT_EQ(2, forkss.size());

  EXPECT_EQ(1, forkss[0].size());
  EXPECT_EQ(1, forkss[1].size());
}

TEST_F(PermutationTest_n10_v10, extractIndependentMerge) {
  auto dags = ExtractDAGsWithIndependentForks(dag_tasks);
  EXPECT_EQ(2, dags.size());
  EXPECT_EQ(2, dags[0].sf_forks_[0].sink);
  EXPECT_EQ(5, dags[1].sf_forks_[0].sink);

  EXPECT_EQ(1, dags[0].sf_forks_.size());
  EXPECT_EQ(1, dags[1].sf_forks_.size());
}

class PermutationTest_n10_v11 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n10_v11"); }
};

TEST_F(PermutationTest_n10_v11, extractIndependentMerge) {
  auto dags = ExtractDAGsWithIndependentForks(dag_tasks);
  EXPECT_EQ(1, dags.size());
  EXPECT_EQ(3, dags[0].sf_forks_.size());
}

TEST_F(PermutationTest_n10_v11, getSFInstance) {
  EXPECT_EQ(1000 / 100 + 1000 / 100 + 1000 / 100,
            dag_tasks.getSF_Fork_InstanceCount(tasks_info));
  EXPECT_EQ(0, dag_tasks.getSFInstance(0, 0, tasks_info));
  EXPECT_EQ(11, dag_tasks.getSFInstance(1, 1, tasks_info));
}

class PermutationTest_n10_v12 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n10_v12"); }
};

TEST_F(PermutationTest_n10_v12, extractIndependentMerge) {
  auto dags = ExtractDAGsWithIndependentForks(dag_tasks);
  EXPECT_EQ(1, dags.size());
  EXPECT_EQ(3, dags[0].sf_forks_.size());
}
class PermutationTest_n10_v13 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n10_v13"); }
};

TEST_F(PermutationTest_n10_v13, extractIndependentMerge) {
  auto dags = ExtractDAGsWithIndependentForks(dag_tasks);
  EXPECT_EQ(1, dags.size());
  EXPECT_EQ(3, dags[0].sf_forks_.size());
}
class PermutationTest_n10_v14 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n10_v14"); }
};

TEST_F(PermutationTest_n10_v14, extractIndependentMerge) {
  ScheduleResult res =
      PerformTOM_OPT_EnumW_Skip<DAG_SPACE::ObjSensorFusion>(dag_tasks);
  // EXPECT_EQ(801, res.obj_);
  EXPECT_THAT(res.obj_, ::testing::Le(801));
}
class PermutationTest_n3_v36 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v36");
    type_trait = "SensorFusion";
    dag_tasks.chains_ = GetChainsForSF(dag_tasks);
  }

 public:
  std::string type_trait;
};

TEST_F(PermutationTest_n3_v36, jitter_sf) {
  VariableOD variable_od = VariableOD(tasks);
  variable_od.print();
  TwoTaskPermutations perm02(0, 2, dag_tasks, tasks_info, "DataAge");
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info, "DataAge");

  ChainsPermutation chains_perm;
  chains_perm.clear();
  chains_perm.push_back(perm02[1]);
  chains_perm.push_back(perm12[0]);
  chains_perm.print();
  EXPECT_EQ(10, ObjSensorFusion::Jitter(dag_tasks, tasks_info, chains_perm,
                                        variable_od, dag_tasks.chains_));
}

TEST_F(PermutationTest_n3_v36, jitter_sf_v2) {
  VariableOD variable_od = VariableOD(tasks);
  variable_od[0].offset = 0;
  variable_od[0].deadline = 1;
  variable_od[1].offset = 1;
  variable_od[1].deadline = 3;
  variable_od[2].offset = 3;
  variable_od[2].deadline = 6;
  variable_od.print();
  TwoTaskPermutations perm02(0, 2, dag_tasks, tasks_info, "DataAge");
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info, "DataAge");

  ChainsPermutation chains_perm;
  chains_perm.clear();
  chains_perm.push_back(perm02[1]);
  chains_perm.push_back(perm12[1]);
  chains_perm.print();
  EXPECT_EQ(6, ObjSensorFusion::Jitter(dag_tasks, tasks_info, chains_perm,
                                       variable_od, dag_tasks.chains_));
}

TEST_F(PermutationTest_n3_v36, PerformStandardLETAnalysisDA) {
  ScheduleResult res = PerformStandardLETAnalysis<ObjSensorFusion>(dag_tasks);
  EXPECT_EQ(10, res.obj_);
  EXPECT_EQ(10, res.jitter_);
}

TEST_F(PermutationTest_n3_v36, PerformImplicitCommuAnalysis) {
  ScheduleResult res = PerformImplicitCommuAnalysis<ObjSensorFusion>(dag_tasks);
  EXPECT_EQ(9, res.obj_);
  EXPECT_EQ(8, res.jitter_);
}

class PermutationTest_n6_v3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n6_v3");
    type_trait = "SensorFusion";
    dag_tasks.chains_ = GetChainsForSF(dag_tasks);
  }

 public:
  std::string type_trait;
};

class PermutationTest_n21_v2 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n21_v2");
    type_trait = "SensorFusion";
    dag_tasks.chains_ = GetChainsForSF(dag_tasks);
    TaskSetOptEnumWSkip tasks_opt_wskip(dag_tasks, dag_tasks.chains_,
                                        type_trait);
    perms = tasks_opt_wskip.adjacent_two_task_permutations_;
  }

 public:
  std::string type_trait;
  std::vector<TwoTaskPermutations> perms;
};

TEST_F(PermutationTest_n21_v2, SelectPermWithMostOverlap) {
  std::vector<int> perm_to_add;
  for (uint i = 1; i < perms.size(); i++) perm_to_add.push_back(i);
  std::vector<int> task_id_count(tasks_info.N, 0);
  UpdateTaskIdCount(0, perms, task_id_count);
  EXPECT_EQ(1, task_id_count[1]);
  EXPECT_EQ(1, task_id_count[16]);

  int perm_id =
      SelectPermWithMostOverlap(perms, perm_to_add, task_id_count, 16);
  EXPECT_EQ(13, perms[perm_id].task_prev_id_);
  UpdateTaskIdCount(perm_id, perms, task_id_count);
  RemoveValue(perm_to_add, perm_id);

  perm_id = SelectPermWithMostOverlap(perms, perm_to_add, task_id_count, 16);
  EXPECT_EQ(10, perms[perm_id].task_prev_id_);
}

TEST_F(PermutationTest_n21_v2, ReOrderTwoTaskPermutations) {
  TaskSetOptEnumWSkip tasks_opt_wskip(dag_tasks, dag_tasks.chains_, type_trait);
  for (uint i = 0; i < tasks_opt_wskip.adjacent_two_task_permutations_.size();
       i++) {
    std::cout
        << tasks_opt_wskip.adjacent_two_task_permutations_[i].task_prev_id_
        << ", "
        << tasks_opt_wskip.adjacent_two_task_permutations_[i].task_next_id_
        << "\n";
  }
  std::cout << "\n\n\n";
  tasks_opt_wskip.ReOrderTwoTaskPermutations();
  for (uint i = 0; i < tasks_opt_wskip.adjacent_two_task_permutations_.size();
       i++) {
    std::cout
        << tasks_opt_wskip.adjacent_two_task_permutations_[i].task_prev_id_
        << ", "
        << tasks_opt_wskip.adjacent_two_task_permutations_[i].task_next_id_
        << "\n";
  }
}

class PermutationTest_test_PaperExample2Chain_v2 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_PaperExample2Chain_v2");
    type_trait = "SensorFusion";
    dag_tasks.chains_ = GetChainsForSF(dag_tasks);
    TaskSetOptEnumWSkip tasks_opt_wskip(dag_tasks, dag_tasks.chains_,
                                        type_trait);
    perms = tasks_opt_wskip.adjacent_two_task_permutations_;
  }

 public:
  std::string type_trait;
  std::vector<TwoTaskPermutations> perms;
};
TEST_F(PermutationTest_test_PaperExample2Chain_v2, ReOrderTwoTaskPermutations) {
  VariableOD variable(dag_tasks.tasks);
  variable[0].offset = 0;
  variable[0].deadline = 1;

  variable[1].offset = 15;
  variable[1].deadline = 20;

  variable[2].offset = 0;
  variable[2].deadline = 10;

  variable[3].offset = 0;
  variable[3].deadline = 15;

  TaskSetOptEnumerate task_sets_perms =
      TaskSetOptEnumerate(dag_tasks, GetChainsForSF(dag_tasks), "SensorFusion");
  auto res_cur = task_sets_perms.PerformOptimizationBF<ObjSensorFusion>();

  EXPECT_EQ(12, ObjSensorFusion::Jitter(
                    dag_tasks, tasks_info, task_sets_perms.best_yet_chain_,
                    task_sets_perms.best_yet_variable_od_, {}));
}

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}