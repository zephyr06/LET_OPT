#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Permutations/TwoTaskPermutations.h"
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

TEST_F(PermutationTest18_n3, GetPossibleReadingJobs) {
  std::vector<JobCEC> read_jobs = GetPossibleReadingJobs(
      JobCEC(1, 0), dag_tasks.GetTask(0), 20, tasks_info);
  EXPECT_EQ(3, read_jobs.size());
  EXPECT_EQ(JobCEC(0, -1), read_jobs[0]);
  EXPECT_EQ(JobCEC(0, 0), read_jobs[1]);
  EXPECT_EQ(JobCEC(0, 1), read_jobs[2]);
}

TEST_F(PermutationTest18_n3, PermIneqDA) {
  PermutationInequality perm_ineq01(JobCEC(1, 0), JobCEC(0, 0), tasks_info,
                                    "DataAge");
  perm_ineq01.print();
  EXPECT_EQ(0, perm_ineq01.lower_bound_);
  EXPECT_EQ(10, perm_ineq01.upper_bound_);
}

TEST_F(PermutationTest18_n3, perm_AppendJobs) {
  PermutationInequality perm_ineq01(0, 1, "DataAge");
  SinglePairPermutation single_permutation(perm_ineq01, tasks_info, type_trait);
  auto variable_od_range = FindVariableRange(dag_tasks);
  variable_od_range.lower_bound.print();
  variable_od_range.upper_bound.print();
  EXPECT_TRUE(single_permutation.AppendJobs(JobCEC(1, 0), JobCEC(0, 1),
                                            tasks_info, variable_od_range));
  single_permutation.print();
  EXPECT_EQ(10, single_permutation.inequality_.lower_bound_);
  EXPECT_EQ(14, single_permutation.inequality_.upper_bound_);
}

TEST_F(PermutationTest18_n3, TwoTaskPerm) {
  TwoTaskPermutations two_task_permutation(1, 2, dag_tasks, tasks_info,
                                           "DataAge");
  two_task_permutation.print();
  EXPECT_EQ(2, two_task_permutation.size());
  SinglePairPermutation perm01 = *(two_task_permutation[0]);
  EXPECT_EQ(0, perm01.inequality_.lower_bound_);
  EXPECT_EQ(4 + 6 + 1, perm01.inequality_.upper_bound_);
}

class PermutationTest23_n3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v23");
    dag_tasks.chains_ = {{0, 1, 2}};
    type_trait = "DataAge";
  }

 public:
  std::string type_trait;
};

TEST_F(PermutationTest23_n3, GetPossibleReadingJobs) {
  std::vector<JobCEC> read_jobs = GetPossibleReadingJobs(
      JobCEC(2, 0), dag_tasks.GetTask(1), 600, tasks_info);
  EXPECT_EQ(3, read_jobs.size());
  EXPECT_EQ(JobCEC(1, -1), read_jobs[0]);
  EXPECT_EQ(JobCEC(1, 0), read_jobs[1]);
  EXPECT_EQ(JobCEC(1, 1), read_jobs[2]);
}

TEST_F(PermutationTest23_n3, GetPossibleReadingJobs_v2) {
  std::vector<JobCEC> read_jobs = GetPossibleReadingJobs(
      JobCEC(1, 1), dag_tasks.GetTask(2), 600, tasks_info);
  EXPECT_EQ(3, read_jobs.size());
  EXPECT_EQ(JobCEC(2, -1), read_jobs[0]);
  EXPECT_EQ(JobCEC(2, 0), read_jobs[1]);
  EXPECT_EQ(JobCEC(2, 1), read_jobs[2]);
}

TEST_F(PermutationTest23_n3, PermIneqDA) {
  PermutationInequality perm_ineq12(JobCEC(2, 1), JobCEC(1, 0), tasks_info,
                                    "DataAge");
  perm_ineq12.print();
  EXPECT_EQ(-200, perm_ineq12.lower_bound_);
  EXPECT_EQ(-50, perm_ineq12.upper_bound_);
}

TEST_F(PermutationTest23_n3, perm_AppendJobs) {
  PermutationInequality perm_ineq12(1, 2, "DataAge");
  SinglePairPermutation single_permutation(perm_ineq12, tasks_info, type_trait);
  auto variable_od_range = FindVariableRange(dag_tasks);
  variable_od_range.lower_bound.print();
  variable_od_range.upper_bound.print();
  EXPECT_TRUE(single_permutation.AppendJobs(JobCEC(2, 0), JobCEC(1, 0),
                                            tasks_info, variable_od_range));
  single_permutation.print();
  EXPECT_TRUE(single_permutation.AppendJobs(JobCEC(2, 1), JobCEC(1, 1),
                                            tasks_info, variable_od_range));
  single_permutation.print();
  EXPECT_TRUE(single_permutation.AppendJobs(JobCEC(2, 2), JobCEC(1, 2),
                                            tasks_info, variable_od_range));

  single_permutation.print();
  EXPECT_EQ(0, single_permutation.inequality_.lower_bound_);
  EXPECT_EQ(50, single_permutation.inequality_.upper_bound_);
}

TEST_F(PermutationTest23_n3, TwoTaskPerm) {
  TwoTaskPermutations two_task_permutation(1, 2, dag_tasks, tasks_info,
                                           "DataAge");
  two_task_permutation.print();
  EXPECT_EQ(7, two_task_permutation.size());

  EXPECT_EQ(-150, two_task_permutation[6]->inequality_.lower_bound_);
  EXPECT_EQ(-101 + 1, two_task_permutation[6]->inequality_.upper_bound_);

  EXPECT_EQ(-100, two_task_permutation[5]->inequality_.lower_bound_);
  EXPECT_EQ(-51 + 1, two_task_permutation[5]->inequality_.upper_bound_);

  EXPECT_EQ(-50, two_task_permutation[4]->inequality_.lower_bound_);
  EXPECT_EQ(-1 + 1, two_task_permutation[4]->inequality_.upper_bound_);

  EXPECT_EQ(0, two_task_permutation[3]->inequality_.lower_bound_);
  EXPECT_EQ(49 + 1, two_task_permutation[3]->inequality_.upper_bound_);

  EXPECT_EQ(50, two_task_permutation[2]->inequality_.lower_bound_);
  EXPECT_EQ(99 + 1, two_task_permutation[2]->inequality_.upper_bound_);

  EXPECT_EQ(100, two_task_permutation[1]->inequality_.lower_bound_);
  EXPECT_EQ(149 + 1, two_task_permutation[1]->inequality_.upper_bound_);

  EXPECT_EQ(150, two_task_permutation[0]->inequality_.lower_bound_);
  EXPECT_EQ(196 + 1, two_task_permutation[0]->inequality_.upper_bound_);
}

class PermutationTest6_n5 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n5_v6");
    type_trait = "DataAge";
  }

 public:
  std::string type_trait;
};

TEST_F(PermutationTest6_n5, GetEdgeIneqRangeDA) {
  VariableRange variable_range = FindVariableRange(dag_tasks);
  variable_range.lower_bound.print();
  variable_range.upper_bound.print();
  PermIneqBound_Range range = GetEdgeIneqRangeDA(Edge(0, 4), variable_range);
  EXPECT_EQ(50 - 69, range.lower_bound_s_upper_bound);
  EXPECT_EQ(0 - 200, range.upper_bound_s_lower_bound);
}

TEST_F(PermutationTest6_n5, overall_opt_Sort) {
  TaskSetOptSorted task_sets_perms =
      TaskSetOptSorted(dag_tasks, dag_tasks.chains_, "DataAge");
  task_sets_perms.adjacent_two_task_permutations_[0].print();
  task_sets_perms.adjacent_two_task_permutations_[1].print();
  // int obj_sort = task_sets_perms.PerformOptimizationSort<ObjDataAge>().obj_;

  TaskSetOptEnumWSkip task_sets_perms_enum =
      TaskSetOptEnumWSkip(dag_tasks, dag_tasks.chains_, "DataAge");
  // EXPECT_EQ(
  //     task_sets_perms_enum.PerformOptimizationSkipInfeasible<ObjDataAge>().obj_,
  //     obj_sort);
  EXPECT_THAT(
      task_sets_perms_enum.PerformOptimizationSkipInfeasible<ObjDataAge>().obj_,
      ::testing::Le(1361));
}

TEST_F(PermutationTest6_n5, overall_opt_brute_force) {
  TaskSetOptEnumerate task_sets_perms =
      TaskSetOptEnumerate(dag_tasks, dag_tasks.chains_, "DataAge");
  task_sets_perms.adjacent_two_task_permutations_[0].print();
  task_sets_perms.adjacent_two_task_permutations_[1].print();
  int obj_find_bf = task_sets_perms.PerformOptimizationBF<ObjDataAge>().obj_;

  TaskSetOptEnumWSkip task_sets_perms_enum =
      TaskSetOptEnumWSkip(dag_tasks, dag_tasks.chains_, "DataAge");
  EXPECT_THAT(
      task_sets_perms_enum.PerformOptimizationSkipInfeasible<ObjDataAge>().obj_,
      ::testing::Le(obj_find_bf));
}

class PermutationTest25_n3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v25");
    type_trait = "DataAge";
  }

 public:
  std::string type_trait;
};

TEST_F(PermutationTest25_n3, TwoTaskPerm) {
  // TaskSetOptEnumWSkip task_sets_perms =
  //     TaskSetOptEnumWSkip(dag_tasks, dag_tasks.chains_, "DataAge");
  // task_sets_perms.adjacent_two_task_permutations_[0].print();
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info, "DataAge");
  perm12.print();
  EXPECT_EQ(2, perm12.size());
}

TEST_F(PermutationTest25_n3, IsPermConflicted_CheckAllWithSameSource_DA) {
  TwoTaskPermutations perm10(1, 0, dag_tasks, tasks_info, "DataAge");
  TwoTaskPermutations perm02(0, 2, dag_tasks, tasks_info, "DataAge");
  perm10[0]->print();
  perm02[1]->print();
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm10[0]);
  // chains_perm.push_back(perm12[3]);
  dag_tasks.chains_ = {{1, 0}, {0, 2}};
  GraphOfChains graph_of_all_ca_chains(dag_tasks.chains_);
  VariableRange variable_od_range(FindVariableRange(dag_tasks));
  variable_od_range.lower_bound.print();
  variable_od_range.upper_bound.print();
  Interval intv1 =
      GetDeadlineRange_DAPerm(variable_od_range, perm10[0]->inequality_);
  // EXPECT_EQ(0 - 77, intv1.start);
  EXPECT_EQ(61, intv1.start);
  EXPECT_EQ(138, intv1.start + intv1.length);
  Interval intv_offset =
      GetOffsetRange_DAPerm(variable_od_range, perm10[0]->inequality_);
  EXPECT_EQ(61, intv_offset.start);
  EXPECT_EQ(138, intv_offset.start + intv_offset.length);

  Interval intv2 =
      GetDeadlineRange_DAPerm(variable_od_range, perm02[1]->inequality_);
  EXPECT_EQ(101, intv2.start);
  // EXPECT_EQ(40 + 200, intv2.start + intv2.length);
  EXPECT_EQ(200, intv2.start + intv2.length);

  Interval intv_offset2 =
      GetOffsetRange_DAPerm(variable_od_range, perm02[1]->inequality_);
  EXPECT_EQ(0, intv_offset2.start);
  EXPECT_EQ(40, intv_offset2.start + intv_offset2.length);

  EXPECT_FALSE(chains_perm.IsPermConflicted_CheckAllWithSameSource(
      variable_od_range, *perm02[1], graph_of_all_ca_chains));
}

TEST_F(PermutationTest25_n3, overall_opt) {
  TaskSetOptEnumWSkip task_sets_perms =
      TaskSetOptEnumWSkip(dag_tasks, dag_tasks.chains_, "DataAge");
  task_sets_perms.adjacent_two_task_permutations_[0].print();
  task_sets_perms.adjacent_two_task_permutations_[1].print();
  int obj_find =
      task_sets_perms.PerformOptimizationSkipInfeasible<ObjDataAge>().obj_;
  // EXPECT_EQ(300, obj_find);
  EXPECT_THAT(obj_find, ::testing::Le(300));
}

TEST_F(PermutationTest25_n3, overall_opt_bf) {
  TaskSetOptEnumerate task_sets_perms =
      TaskSetOptEnumerate(dag_tasks, dag_tasks.chains_, "DataAge");
  task_sets_perms.adjacent_two_task_permutations_[0].print();
  task_sets_perms.adjacent_two_task_permutations_[1].print();
  int obj_find = task_sets_perms.PerformOptimizationBF<ObjDataAge>().obj_;
  // EXPECT_EQ(300, obj_find);
  EXPECT_THAT(obj_find, ::testing::Le(300));
}

TEST_F(PermutationTest25_n3, GetEdgeIneqRangeDA) {
  VariableRange variable_range = FindVariableRange(dag_tasks);
  variable_range.lower_bound.print();
  variable_range.upper_bound.print();
  PermIneqBound_Range range = GetEdgeIneqRangeDA(Edge(0, 1), variable_range);
  EXPECT_EQ(138 - 61, range.lower_bound_s_upper_bound);
  EXPECT_EQ(0 - 200, range.upper_bound_s_lower_bound);
}
TEST_F(PermutationTest25_n3, SkipDAPerm) {
  TaskSetOptEnumWSkip task_sets_perms =
      TaskSetOptEnumWSkip(dag_tasks, dag_tasks.chains_, "DataAge");
  task_sets_perms.adjacent_two_task_permutations_[0].print();
  task_sets_perms.adjacent_two_task_permutations_[1].print();
  auto perm01 = task_sets_perms.adjacent_two_task_permutations_[0];
  auto perm12 = task_sets_perms.adjacent_two_task_permutations_[1];
  EXPECT_TRUE(IfSkipAnotherPermDA(*perm01[0], *perm01[1]));
  EXPECT_FALSE(IfSkipAnotherPermDA(*perm01[1], *perm01[0]));
  EXPECT_TRUE(IfSkipAnotherPermDA(*perm12[0], *perm12[1]));
  EXPECT_FALSE(IfSkipAnotherPermDA(*perm12[1], *perm12[0]));
}

TEST_F(PermutationTest25_n3, FindPossibleVariableOD) {
  TwoTaskPermutations perm10(1, 0, dag_tasks, tasks_info, "DataAge");
  TwoTaskPermutations perm02(0, 2, dag_tasks, tasks_info, "DataAge");
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm10[0]);
  chains_perm.print();
  perm02[0]->print();
  GraphOfChains graph_of_all_ca_chains(dag_tasks.chains_);
  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  VariableRange variable_range_w_chains =
      FindPossibleVariableOD(dag_tasks, tasks_info, rta, chains_perm, false);
  variable_range_w_chains.lower_bound.print();
  variable_range_w_chains.upper_bound.print();
  EXPECT_EQ(0, variable_range_w_chains.lower_bound[1].offset);
  EXPECT_EQ(77, variable_range_w_chains.upper_bound[1].offset);  // different
  EXPECT_EQ(61, variable_range_w_chains.lower_bound[0].offset);
  EXPECT_EQ(138, variable_range_w_chains.upper_bound[0].offset);
  EXPECT_EQ(0, variable_range_w_chains.lower_bound[2].offset);
  EXPECT_EQ(40, variable_range_w_chains.upper_bound[2].offset);

  EXPECT_EQ(61, variable_range_w_chains.lower_bound[1].deadline);
  EXPECT_EQ(138,
            variable_range_w_chains.upper_bound[1].deadline);  // different
  EXPECT_EQ(123,
            variable_range_w_chains.lower_bound[0].deadline);  // different
  EXPECT_EQ(200, variable_range_w_chains.upper_bound[0].deadline);
  EXPECT_EQ(60, variable_range_w_chains.lower_bound[2].deadline);
  EXPECT_EQ(100, variable_range_w_chains.upper_bound[2].deadline);
}

TEST_F(PermutationTest25_n3, IsValid) {
  TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info, "DataAge");
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info, "DataAge");
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm01[0]);
  chains_perm.print();
  perm12[0]->print();
  GraphOfChains graph_of_all_ca_chains(dag_tasks.chains_);
  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  VariableRange variable_range_w_chains =
      FindPossibleVariableOD(dag_tasks, tasks_info, rta, chains_perm, false);
  variable_range_w_chains.lower_bound[0].offset = 77;
  variable_range_w_chains.upper_bound[0].deadline = 77;
  variable_range_w_chains.lower_bound.print();
  variable_range_w_chains.upper_bound.print();
  EXPECT_TRUE(chains_perm.IsValid(variable_range_w_chains, *perm12[0],
                                  graph_of_all_ca_chains, rta));
}
TEST_F(PermutationTest25_n3, overall_opt_Sort) {
  TaskSetOptSorted task_sets_perms =
      TaskSetOptSorted(dag_tasks, dag_tasks.chains_, "DataAge");
  task_sets_perms.adjacent_two_task_permutations_[0].print();
  task_sets_perms.adjacent_two_task_permutations_[1].print();
  int obj_find = task_sets_perms.PerformOptimizationSort<ObjDataAge>().obj_;
  EXPECT_TRUE(task_sets_perms.ExamSchedulabilityOptSol());
  // TODO: the optimal solution seems to be 200!!
  // EXPECT_EQ(300, obj_find);
  EXPECT_THAT(obj_find, ::testing::Le(300));
}
int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}