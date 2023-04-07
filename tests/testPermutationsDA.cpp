#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Permutations/TwoTaskPermutations.h"
#include "tests/testEnv.cpp"

using namespace DAG_SPACE;

class PermutationTest18_n3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v18");
    dag_tasks.chains_ = {{0, 1, 2}};
  }
};

TEST_F(PermutationTest18_n3, GetPossibleReadingJobs) {
  std::vector<JobCEC> read_jobs = GetPossibleReadingJobs(
      JobCEC(1, 0), dag_tasks.GetTask(0), 20, tasks_info);
  EXPECT_EQ(3, read_jobs.size());
  EXPECT_EQ(JobCEC(0, -1), read_jobs[0]);
  EXPECT_EQ(JobCEC(0, 0), read_jobs[1]);
  EXPECT_EQ(JobCEC(0, 1), read_jobs[2]);
}

class PermutationTest23_n3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v23");
    dag_tasks.chains_ = {{0, 1, 2}};
  }
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
int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}