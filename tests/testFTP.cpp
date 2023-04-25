
#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Baseline/ImplicitCommunication/ScheduleSimulation.h"
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

TEST_F(PermutationTest18_n3, RemoveFinishedJob) {
  RunQueue run_queue(tasks);
  int processor_id = 1;
  AddTasksToRunQueue(run_queue, tasks, processor_id, 0);
  EXPECT_EQ(3, run_queue.size());

  run_queue.RunJobHigestPriority(0);
  EXPECT_TRUE(run_queue.front().running);

  run_queue.RemoveFinishedJob(1);
  EXPECT_EQ(2, run_queue.size());

  run_queue.RunJobHigestPriority(1);
  EXPECT_TRUE(run_queue.front().running);
}

TEST_F(PermutationTest18_n3, FTP_Schedule) {
  Schedule schedule_expected;
  schedule_expected[JobCEC(0, 0)] = JobStartFinish(0, 1);
  schedule_expected[JobCEC(2, 0)] = JobStartFinish(1, 4);
  schedule_expected[JobCEC(1, 0)] = JobStartFinish(4, 6);
  Schedule schedule_actual = SimulatedFTP_SingleCore(dag_tasks, tasks_info, 1);

  EXPECT_EQ(schedule_expected[JobCEC(0, 0)], schedule_actual[JobCEC(0, 0)]);
  EXPECT_EQ(schedule_expected[JobCEC(1, 0)], schedule_actual[JobCEC(1, 0)]);
  EXPECT_EQ(schedule_expected[JobCEC(2, 0)], schedule_actual[JobCEC(2, 0)]);
}

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}