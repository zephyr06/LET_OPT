

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/TaskSetOptSorted.h"
#include "sources/Optimization/TaskSetOptSorted_Offset.h"
#include "sources/OptimizeMain.h"
#include "testEnv.cpp"
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
  int obj = task_sets_perms.PerformOptimizationSort<ObjDataAge>();
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
  int obj = task_sets_perms.PerformOptimizationSort<ObjDataAge>();
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
/*
 */
int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}