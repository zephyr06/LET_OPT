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
  EXPECT_EQ(17, single_permutation.inequality_.upper_bound_);
}

TEST_F(PermutationTest18_n3, TwoTaskPerm) {
  TwoTaskPermutations two_task_permutation(1, 2, dag_tasks, tasks_info,
                                           "DataAge");
  two_task_permutation.print();
  EXPECT_EQ(2, two_task_permutation.size());
  SinglePairPermutation perm01 = *(two_task_permutation[1]);
  EXPECT_EQ(0, perm01.inequality_.lower_bound_);
  EXPECT_EQ(12, perm01.inequality_.upper_bound_);
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

  EXPECT_EQ(-150, two_task_permutation[0]->inequality_.lower_bound_);
  EXPECT_EQ(-101 + 1, two_task_permutation[0]->inequality_.upper_bound_);

  EXPECT_EQ(-100, two_task_permutation[1]->inequality_.lower_bound_);
  EXPECT_EQ(-51 + 1, two_task_permutation[1]->inequality_.upper_bound_);

  EXPECT_EQ(-50, two_task_permutation[2]->inequality_.lower_bound_);
  EXPECT_EQ(-1 + 1, two_task_permutation[2]->inequality_.upper_bound_);

  EXPECT_EQ(0, two_task_permutation[3]->inequality_.lower_bound_);
  EXPECT_EQ(49 + 1, two_task_permutation[3]->inequality_.upper_bound_);

  EXPECT_EQ(50, two_task_permutation[4]->inequality_.lower_bound_);
  EXPECT_EQ(99 + 1, two_task_permutation[4]->inequality_.upper_bound_);

  EXPECT_EQ(100, two_task_permutation[5]->inequality_.lower_bound_);
  EXPECT_EQ(149 + 1, two_task_permutation[5]->inequality_.upper_bound_);

  EXPECT_EQ(150, two_task_permutation[6]->inequality_.lower_bound_);
  EXPECT_EQ(195 + 1, two_task_permutation[6]->inequality_.upper_bound_);
}

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}