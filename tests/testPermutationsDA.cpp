#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Permutations/TwoTaskPermutations.h"
#include "tests/testEnv.cpp"

using namespace DAG_SPACE;

class PermutationTest18 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v18");
    dag_tasks.chains_ = {{0, 1, 2}};
  }
};

std::vector<JobCEC> GetPossibleReadingJobs(
    const JobCEC& job_curr, const Task& task_prev, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasksInfo) {
  int job_min_finish = GetActivationTime(job_curr, tasksInfo) +
                       GetExecutionTime(job_curr, tasksInfo);
  int job_max_finish = GetDeadline(job_curr, tasksInfo);

  int period_next = tasksInfo.GetTask(task_next.id).period;
  std::vector<JobCEC> reactingJobs;
  reactingJobs.reserve(superperiod / tasksInfo.GetTask(job_curr.taskId).period);
  for (int i = std::floor(float(job_min_finish) / period_next);
       i <= std::ceil(float(job_max_finish) / period_next); i++) {
    JobCEC job_next_i(task_next.id, i);
    int max_start_next = GetDeadline(job_next_i, tasksInfo) -
                         GetExecutionTime(job_next_i, tasksInfo);
    if (max_start_next < job_min_finish) continue;
    reactingJobs.push_back(job_next_i);
  }

  return reactingJobs;
}

TEST_F(PermutationTest18, GetPossibleReadingJobs) {
  std::vector<JobCEC> read_jobs = GetPossibleReadingJobs(
      JobCEC(1, 0), dag_tasks.GetTask(0), 20, tasks_info);
  //   EXPECT_EQ(3, read_jobs.size());
  //   EXPECT_EQ(JobCEC(0, -1), read_jobs[0]);
  //   EXPECT_EQ(JobCEC(0, 0), read_jobs[1]);
  //   EXPECT_EQ(JobCEC(0, 1), read_jobs[2]);
}

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}