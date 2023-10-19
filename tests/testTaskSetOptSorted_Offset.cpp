

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/TaskSetOptSorted.h"
#include "sources/Optimization/TaskSetOptSorted_Offset.h"
#include "sources/OptimizeMain.h"
#include "tests/testEnv.cpp"
using namespace DAG_SPACE;

class PermutationTest18_n3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v18");
    dag_tasks.chains_ = {{0, 1, 2}};
    type_trait = "DataAge";
  }

 public:
  std::string type_trait;
};

TEST_F(PermutationTest18_n3, TaskSetOptSorted_Offset) {
  TaskSetOptSorted_Offset task_set_perms_off(dag_tasks, {dag_tasks.chains_[0]});
  EXPECT_EQ(3, task_set_perms_off.adjacent_two_task_permutations_[0].size());
  TwoTaskPermutations perm01 =
      task_set_perms_off.adjacent_two_task_permutations_[0];
  EXPECT_EQ(0, perm01[0]->GetPrevTaskId());
  EXPECT_EQ(1, perm01[0]->GetNextTaskId());
  EXPECT_EQ(JobCEC(0, 0), perm01[0]->job_matches_.at(JobCEC(1, 0)));
  EXPECT_EQ(JobCEC(0, -1), perm01[1]->job_matches_.at(JobCEC(1, 0)));
  EXPECT_EQ(JobCEC(0, -2), perm01[2]->job_matches_.at(JobCEC(1, 0)));

  TwoTaskPermutations perm12 =
      task_set_perms_off.adjacent_two_task_permutations_[1];
  EXPECT_EQ(2, perm12.size());
  EXPECT_EQ(1, perm12[0]->GetPrevTaskId());
  EXPECT_EQ(2, perm12[0]->GetNextTaskId());
  EXPECT_EQ(JobCEC(1, -1), perm12[0]->job_matches_.at(JobCEC(2, 0)));
  EXPECT_EQ(JobCEC(1, -2), perm12[1]->job_matches_.at(JobCEC(2, 0)));
}

TEST_F(PermutationTest18_n3, optimize) {
  dag_tasks.chains_ = {dag_tasks.chains_[0]};
  Martinez18TaskSetPerms mart_task_perms(dag_tasks, dag_tasks.chains_[0]);
  int obj_mart = mart_task_perms.PerformOptimization();

  TaskSetOptSorted_Offset task_sets_perms(dag_tasks, {dag_tasks.chains_[0]});
  int obj = task_sets_perms.PerformOptimizationSort<ObjDataAge>().obj_;
  task_sets_perms.best_yet_chain_[Edge(0, 1)]->print();
  task_sets_perms.best_yet_chain_[Edge(1, 2)]->print();
  // EXPECT_THAT(obj, ::testing::Le(40));
  EXPECT_EQ(obj_mart, obj);
}
TEST_F(PermutationTest18_n3, optimize_overall) {
  // EXPECT_THAT(PerformTOM_OPTOffset_Sort(dag_tasks).obj_, ::testing::Le(50));
  EXPECT_EQ(50, PerformTOM_OPTOffset_Sort(dag_tasks).obj_);
}

class PermutationTest4_n3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v4");
    dag_tasks.chains_ = {{0, 1, 2}};
    type_trait = "DataAge";
  }

 public:
  std::string type_trait;
};

TEST_F(PermutationTest4_n3, GeneratePermIneqOnlyOffset) {
  auto variable_range_od = FindVariableRangeMartModel(dag_tasks);
  PermutationInequality perm_ineq = GeneratePermIneqOnlyOffset(
      0, 1, "DataAge", variable_range_od, tasks_info);
  EXPECT_EQ(-29, perm_ineq.lower_bound_);
  EXPECT_EQ(-6, perm_ineq.upper_bound_);
}

TEST_F(PermutationTest4_n3, TaskSetOptSorted_Offset) {
  TaskSetOptSorted_Offset task_set_perms_off(dag_tasks, {dag_tasks.chains_[0]});
  task_set_perms_off.adjacent_two_task_permutations_[0].print();
  // J_{1,2} cannot read from J_{0,1}
  EXPECT_EQ(5, task_set_perms_off.adjacent_two_task_permutations_[0].size());
  TwoTaskPermutations perm01 =
      task_set_perms_off.adjacent_two_task_permutations_[0];
  // EXPECT_EQ(2, perm01.size());
  // EXPECT_EQ(0, perm01[0]->GetPrevTaskId());
  // EXPECT_EQ(1, perm01[0]->GetNextTaskId());
  // EXPECT_EQ(JobCEC(0, -1), perm01[0]->job_matches_.at(JobCEC(1, 0)));
  // EXPECT_EQ(JobCEC(0, 0), perm01[0]->job_matches_.at(JobCEC(1, 1)));
  // EXPECT_EQ(JobCEC(0, 0), perm01[0]->job_matches_.at(JobCEC(1, 2)));

  // EXPECT_EQ(JobCEC(0, -1), perm01[1]->job_matches_.at(JobCEC(1, 0)));
  // EXPECT_EQ(JobCEC(0, -1), perm01[1]->job_matches_.at(JobCEC(1, 1)));
  // EXPECT_EQ(JobCEC(0, 0), perm01[1]->job_matches_.at(JobCEC(1, 2)));
  std::cout << "***********\n";
  TwoTaskPermutations perm12 =
      task_set_perms_off.adjacent_two_task_permutations_[1];
  perm12.print();
  EXPECT_EQ(5, perm12.size());
  EXPECT_EQ(1, perm12[0]->GetPrevTaskId());
  EXPECT_EQ(2, perm12[0]->GetNextTaskId());
  EXPECT_EQ(JobCEC(1, 0), perm12[0]->job_matches_.at(JobCEC(2, 0)));
  EXPECT_EQ(JobCEC(1, 1), perm12[0]->job_matches_.at(JobCEC(2, 1)));

  EXPECT_EQ(JobCEC(1, -1), perm12[1]->job_matches_.at(JobCEC(2, 0)));
  EXPECT_EQ(JobCEC(1, 1), perm12[1]->job_matches_.at(JobCEC(2, 1)));

  EXPECT_EQ(JobCEC(1, -1), perm12[2]->job_matches_.at(JobCEC(2, 0)));
  EXPECT_EQ(JobCEC(1, 0), perm12[2]->job_matches_.at(JobCEC(2, 1)));

  EXPECT_EQ(JobCEC(1, -2), perm12[3]->job_matches_.at(JobCEC(2, 0)));
  EXPECT_EQ(JobCEC(1, 0), perm12[3]->job_matches_.at(JobCEC(2, 1)));

  EXPECT_EQ(JobCEC(1, -2), perm12[4]->job_matches_.at(JobCEC(2, 0)));
  EXPECT_EQ(JobCEC(1, -1), perm12[4]->job_matches_.at(JobCEC(2, 1)));
}

TEST_F(PermutationTest4_n3, optimize) {
  dag_tasks.chains_ = {dag_tasks.chains_[0]};
  Martinez18TaskSetPerms mart_task_perms(dag_tasks, dag_tasks.chains_[0]);
  int obj_mart = mart_task_perms.PerformOptimization();

  TaskSetOptSorted_Offset task_sets_perms(dag_tasks, {dag_tasks.chains_[0]});
  int obj = task_sets_perms.PerformOptimizationSort<ObjDataAge>().obj_;
  EXPECT_EQ(obj_mart, obj);
}

class PermutationTest_n5_v59 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n5_v59"); }  // only read 2 chains
};
TEST_F(PermutationTest_n5_v59, optimize) {
  Martinez18TaskSetPerms mart_task_perms0(dag_tasks, dag_tasks.chains_[0]);
  int obj_mart0 = mart_task_perms0.PerformOptimization();
  Martinez18TaskSetPerms mart_task_perms1(dag_tasks, dag_tasks.chains_[1]);
  int obj_mart1 = mart_task_perms1.PerformOptimization();

  int obj_offset_opt_sort = PerformTOM_OPTOffset_Sort(dag_tasks).obj_;
  EXPECT_THAT(obj_mart0 + obj_mart1, obj_offset_opt_sort);
}

TEST_F(PermutationTest_n5_v59, FindVariableRangeMart) {
  VariableRange range = FindVariableRangeMartModel(dag_tasks);
  for (int i = 0; i < 5; i++) EXPECT_EQ(0, range.lower_bound[i].offset);
  EXPECT_EQ(39, range.upper_bound[0].deadline);
  EXPECT_EQ(199, range.upper_bound[1].deadline);
}
TEST_F(PermutationTest_n5_v59, analyze_perms) {
  TaskSetOptSorted_Offset task_sets_perms(dag_tasks, dag_tasks.chains_);
  EXPECT_EQ(2, task_sets_perms.adjacent_two_task_permutations_.size());
  EXPECT_EQ(5, task_sets_perms.adjacent_two_task_permutations_[0].size());
}

class PermutationTest_n5_v60 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n5_v60"); }
};

TEST_F(PermutationTest_n5_v60, Evaluate_LP) {
  TaskSetOptSorted_Offset task_sets_perms(dag_tasks, dag_tasks.chains_);
  ChainsPermutation chains_perm;
  chains_perm.push_back(task_sets_perms.adjacent_two_task_permutations_[0][0]);
  chains_perm.push_back(task_sets_perms.adjacent_two_task_permutations_[1][0]);
  chains_perm.print();
  std::pair<VariableOD, int> res =
      FindODWithLP(dag_tasks, tasks_info, chains_perm,
                   task_sets_perms.graph_of_all_ca_chains_, "DataAge",
                   task_sets_perms.rta_, true);
  EXPECT_EQ(2500, res.second);
}
TEST_F(PermutationTest_n5_v60, optimize) {
  int obj_offset_opt_sort = PerformTOM_OPTOffset_Sort(dag_tasks).obj_;
  // Martinez18TaskSetPerms mart_task_perms0(dag_tasks, dag_tasks.chains_[0]);
  // int obj_mart0 = mart_task_perms0.PerformOptimization();

  EXPECT_THAT(2500, obj_offset_opt_sort);
}

class PermutationTest_n5_v62 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n5_v62");
    dag_tasks.chains_ = {dag_tasks.chains_[0]};
  }
};
TEST_F(PermutationTest_n5_v62, Evaluate_LP) {
  TaskSetOptSorted_Offset task_sets_perms(dag_tasks, dag_tasks.chains_);
  ChainsPermutation chains_perm;
  chains_perm.push_back(task_sets_perms.adjacent_two_task_permutations_[0][0]);
  chains_perm.push_back(task_sets_perms.adjacent_two_task_permutations_[1][0]);
  chains_perm.print();
  std::pair<VariableOD, int> res =
      FindODWithLP(dag_tasks, tasks_info, chains_perm,
                   task_sets_perms.graph_of_all_ca_chains_, "DataAge",
                   task_sets_perms.rta_, true);
  EXPECT_EQ(500, res.second);
}
TEST_F(PermutationTest_n5_v62, optimize) {
  int obj_offset_opt_sort = PerformTOM_OPTOffset_Sort(dag_tasks).obj_;
  // Martinez18TaskSetPerms mart_task_perms0(dag_tasks, dag_tasks.chains_[0]);
  // int obj_mart0 = mart_task_perms0.PerformOptimization();

  EXPECT_THAT(500, obj_offset_opt_sort);
}

class PermutationTest_n5_v30 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n5_v30"); }
};

TEST_F(PermutationTest_n5_v30, FindBestPossibleVariableOD_offset_only) {
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info, "DataAge");
  TwoTaskPermutations perm24(2, 4, dag_tasks, tasks_info, "DataAge");
  TwoTaskPermutations perm23(2, 3, dag_tasks, tasks_info, "DataAge");
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm12[73]);
  chains_perm.push_back(perm24[11]);
  chains_perm.push_back(perm23[0]);
  chains_perm.print();

  TaskSetOptEnumWSkip task_sets_perms(dag_tasks, dag_tasks.chains_, "DataAge");
  VariableRange variable_range = FindPossibleVariableOD(
      dag_tasks, tasks_info, task_sets_perms.rta_, chains_perm, true);
  auto rta = GetResponseTimeTaskSet(dag_tasks);
  PrintRta(rta);

  EXPECT_EQ(0, variable_range.lower_bound[1].offset);
  EXPECT_EQ(1000, variable_range.lower_bound[1].deadline);
  EXPECT_EQ(9, variable_range.upper_bound[1].offset);
  EXPECT_EQ(1009, variable_range.upper_bound[1].deadline);

  EXPECT_EQ(0, variable_range.lower_bound[2].offset);
  EXPECT_EQ(10, variable_range.lower_bound[2].deadline);
  EXPECT_EQ(9, variable_range.upper_bound[2].offset);
  EXPECT_EQ(19, variable_range.upper_bound[2].deadline);

  EXPECT_EQ(190, variable_range.lower_bound[3].offset);
  EXPECT_EQ(190 + 200, variable_range.lower_bound[3].deadline);
  EXPECT_EQ(199, variable_range.upper_bound[3].offset);
  EXPECT_EQ(199 + 200, variable_range.upper_bound[3].deadline);

  EXPECT_EQ(10, variable_range.lower_bound[4].offset);
  EXPECT_EQ(1010, variable_range.lower_bound[4].deadline);
  EXPECT_EQ(28, variable_range.upper_bound[4].offset);
  EXPECT_EQ(1028, variable_range.upper_bound[4].deadline);

  VariableOD variable_od = FindBestPossibleVariableOD(
      dag_tasks, tasks_info, task_sets_perms.rta_, chains_perm, true);

  EXPECT_EQ(variable_range.upper_bound[1].offset, variable_od[1].offset);
  EXPECT_EQ(variable_range.lower_bound[1].deadline, variable_od[1].deadline);
  EXPECT_EQ(variable_range.upper_bound[2].offset, variable_od[2].offset);
  EXPECT_EQ(variable_range.lower_bound[2].deadline, variable_od[2].deadline);
  EXPECT_EQ(variable_range.upper_bound[3].offset, variable_od[3].offset);
  EXPECT_EQ(variable_range.lower_bound[3].deadline, variable_od[3].deadline);
  EXPECT_EQ(variable_range.upper_bound[4].offset, variable_od[4].offset);
  EXPECT_EQ(variable_range.lower_bound[4].deadline, variable_od[4].deadline);
}

// class PermutationTest_n5_v61 : public PermutationTestBase {
//   void SetUp() override { SetUpBase("test_n5_v61"); }
// };

// TEST_F(PermutationTest_n5_v61, Evaluate_LP) {
//   TaskSetOptSorted_Offset task_sets_perms(dag_tasks, dag_tasks.chains_);
//   ChainsPermutation chains_perm;
//   chains_perm.push_back(task_sets_perms.adjacent_two_task_permutations_[0][0]);
//   chains_perm.push_back(task_sets_perms.adjacent_two_task_permutations_[1][0]);
//   chains_perm.print();
//   std::pair<VariableOD, int> res =
//       FindODWithLP(dag_tasks, tasks_info, chains_perm,
//                    task_sets_perms.graph_of_all_ca_chains_, "DataAge",
//                    task_sets_perms.rta_, true);
//   EXPECT_EQ(2500, res.second);
// }

/*
 */
int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}