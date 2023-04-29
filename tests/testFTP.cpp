
#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Baseline/ImplicitCommunication/ScheduleSimulation.h"
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
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
  AddTasksToRunQueue(run_queue, dag_tasks, processor_id, 0);
  EXPECT_EQ(3, run_queue.size());

  run_queue.RunJobHigestPriority(0);
  EXPECT_TRUE(run_queue.front().running);

  run_queue.RemoveFinishedJob(1);
  EXPECT_EQ(2, run_queue.size());

  run_queue.RunJobHigestPriority(1);
  EXPECT_TRUE(run_queue.front().running);
}

TEST_F(PermutationTest18_n3, Variable2Schedule) {
  VariableOD variable_od(dag_tasks.GetTaskSet());
  variable_od[0].offset = 1;
  variable_od[2].deadline = 19;
  Schedule schedule_actual = Variable2Schedule(tasks_info, variable_od);

  EXPECT_EQ(JobStartFinish(1, 10), schedule_actual[JobCEC(0, 0)]);
  EXPECT_EQ(JobStartFinish(11, 20), schedule_actual[JobCEC(0, 1)]);
  EXPECT_EQ(JobStartFinish(0, 20), schedule_actual[JobCEC(1, 0)]);
  EXPECT_EQ(JobStartFinish(0, 19), schedule_actual[JobCEC(2, 0)]);
}

TEST_F(PermutationTest18_n3, GetStartTime) {
  VariableOD variable_od(dag_tasks.GetTaskSet());
  variable_od[0].offset = 1;
  variable_od[2].deadline = 19;
  Schedule schedule_actual = Variable2Schedule(tasks_info, variable_od);
  // EXPECT_EQ(JobStartFinish(1, 10), schedule_actual[JobCEC(0, 0)]);
  // EXPECT_EQ(JobStartFinish(11, 20), schedule_actual[JobCEC(0, 1)]);
  // EXPECT_EQ(JobStartFinish(0, 20), schedule_actual[JobCEC(1, 0)]);
  // EXPECT_EQ(JobStartFinish(0, 19), schedule_actual[JobCEC(2, 0)]);
  EXPECT_EQ(1, GetStartTime(JobCEC(0, 0), schedule_actual, tasks_info));
  EXPECT_EQ(1 - 10, GetStartTime(JobCEC(0, -1), schedule_actual, tasks_info));
  EXPECT_EQ(1 - 20, GetStartTime(JobCEC(0, -2), schedule_actual, tasks_info));

  EXPECT_EQ(20, GetStartTime(JobCEC(1, 1), schedule_actual, tasks_info));
  EXPECT_EQ(-20, GetStartTime(JobCEC(1, -1), schedule_actual, tasks_info));
  EXPECT_EQ(-40, GetStartTime(JobCEC(1, -2), schedule_actual, tasks_info));
}
TEST_F(PermutationTest18_n3, GetFinishTime) {
  VariableOD variable_od(dag_tasks.GetTaskSet());
  variable_od[0].offset = 1;
  variable_od[2].deadline = 19;
  Schedule schedule_actual = Variable2Schedule(tasks_info, variable_od);
  // EXPECT_EQ(JobStartFinish(1, 10), schedule_actual[JobCEC(0, 0)]);
  // EXPECT_EQ(JobStartFinish(11, 20), schedule_actual[JobCEC(0, 1)]);
  // EXPECT_EQ(JobStartFinish(0, 20), schedule_actual[JobCEC(1, 0)]);
  // EXPECT_EQ(JobStartFinish(0, 19), schedule_actual[JobCEC(2, 0)]);
  EXPECT_EQ(19, GetFinishTime(JobCEC(2, 0), schedule_actual, tasks_info));
  EXPECT_EQ(19 - 20, GetFinishTime(JobCEC(2, -1), schedule_actual, tasks_info));
  EXPECT_EQ(19 - 40, GetFinishTime(JobCEC(2, -2), schedule_actual, tasks_info));
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
TEST_F(PermutationTest18_n3, PerformImplicitCommuAnalysis_DA) {
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
  ChainsPermutation chains_perm =
      GetChainsPermFromVariable(dag_tasks, tasks_info, dag_tasks.chains_,
                                ObjDataAge::type_trait, schedule);
  int obj_actual = ObjDataAge::Obj(dag_tasks, tasks_info, chains_perm, schedule,
                                   dag_tasks.chains_);
  EXPECT_EQ(24, obj_actual);
}

TEST_F(PermutationTest18_n3, GetFirstReactJob) {
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
  EXPECT_EQ(
      0, GetFirstReactJob(JobCEC(0, 0), task1, 20, tasks_info, schedule).jobId);
  EXPECT_EQ(
      1, GetFirstReactJob(JobCEC(0, 1), task1, 20, tasks_info, schedule).jobId);

  EXPECT_EQ(
      -1,
      GetFirstReactJob(JobCEC(0, -3), task1, 20, tasks_info, schedule).jobId);
  EXPECT_EQ(
      -2,
      GetFirstReactJob(JobCEC(0, -4), task1, 20, tasks_info, schedule).jobId);
  EXPECT_EQ(
      4, GetFirstReactJob(JobCEC(1, 3), task1, 20, tasks_info, schedule).jobId);
}

TEST_F(PermutationTest18_n3, PerformImplicitCommuAnalysis_RT) {
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
  ChainsPermutation chains_perm =
      GetChainsPermFromVariable(dag_tasks, tasks_info, dag_tasks.chains_,
                                ObjReactionTime::type_trait, schedule);
  int obj_actual = ObjReactionTime::Obj(dag_tasks, tasks_info, chains_perm,
                                        schedule, dag_tasks.chains_);
  EXPECT_EQ(44 - 10, obj_actual);
  EXPECT_EQ(44 - 10,
            PerformImplicitCommuAnalysis<ObjReactionTime>(dag_tasks).obj_);
}

TEST_F(PermutationTest18_n3, PerformImplicitCommuAnalysis) {
  auto res = PerformImplicitCommuAnalysis<ObjDataAge>(dag_tasks);
  EXPECT_EQ(24, res.obj_);
}

class PermutationTest26_n3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v26");
    dag_tasks.chains_ = {{0, 1, 2}};
  }

 public:
};

TEST_F(PermutationTest26_n3, RemoveFinishedJob) {
  RunQueue run_queue(tasks);
  int processor_id = 1;
  AddTasksToRunQueue(run_queue, dag_tasks, processor_id, 0);
  EXPECT_EQ(3, run_queue.size());

  run_queue.RunJobHigestPriority(0);
  EXPECT_TRUE(run_queue.front().running);

  run_queue.RemoveFinishedJob(3);
  EXPECT_EQ(2, run_queue.size());

  run_queue.RunJobHigestPriority(3);
  EXPECT_TRUE(run_queue.front().running);

  run_queue.RemoveFinishedJob(5);
  EXPECT_EQ(1, run_queue.size());

  run_queue.RunJobHigestPriority(5);
  EXPECT_TRUE(run_queue.front().running);

  // 10~20
  AddTasksToRunQueue(run_queue, dag_tasks, processor_id, 10);
  EXPECT_EQ(3, run_queue.size());
  run_queue.RunJobHigestPriority(10);
  EXPECT_TRUE(run_queue.front().running);

  run_queue.RemoveFinishedJob(13);
  EXPECT_EQ(2, run_queue.size());

  run_queue.RunJobHigestPriority(13);
  EXPECT_TRUE(run_queue.front().running);

  run_queue.RemoveFinishedJob(15);
  EXPECT_EQ(1, run_queue.size());

  run_queue.RunJobHigestPriority(15);
  run_queue.RemoveFinishedJob(20);
  EXPECT_EQ(0, run_queue.size());
}

TEST_F(PermutationTest26_n3, FTP_Schedule) {
  Schedule schedule_expected;
  schedule_expected[JobCEC(0, 0)] = JobStartFinish(5, 20);
  schedule_expected[JobCEC(1, 0)] = JobStartFinish(3, 5);
  schedule_expected[JobCEC(2, 0)] = JobStartFinish(0, 3);

  schedule_expected[JobCEC(1, 1)] = JobStartFinish(13, 15);
  schedule_expected[JobCEC(2, 1)] = JobStartFinish(10, 13);

  schedule_expected[JobCEC(1, 2)] = JobStartFinish(23, 25);
  schedule_expected[JobCEC(2, 2)] = JobStartFinish(20, 23);
  Schedule schedule_actual = SimulatedFTP_SingleCore(dag_tasks, tasks_info, 1);

  EXPECT_EQ(schedule_expected[JobCEC(0, 0)], schedule_actual[JobCEC(0, 0)]);
  EXPECT_EQ(schedule_expected[JobCEC(1, 0)], schedule_actual[JobCEC(1, 0)]);
  EXPECT_EQ(schedule_expected[JobCEC(2, 0)], schedule_actual[JobCEC(2, 0)]);

  EXPECT_EQ(schedule_expected[JobCEC(1, 1)], schedule_actual[JobCEC(1, 1)]);
  EXPECT_EQ(schedule_expected[JobCEC(2, 1)], schedule_actual[JobCEC(2, 1)]);

  EXPECT_EQ(schedule_expected[JobCEC(1, 2)], schedule_actual[JobCEC(1, 2)]);
  EXPECT_EQ(schedule_expected[JobCEC(2, 2)], schedule_actual[JobCEC(2, 2)]);
}

class PermutationTest27_n3 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n3_v27"); }
};

TEST_F(PermutationTest27_n3, FTP_Schedule) {
  Schedule schedule_actual = SimulatedFTP_SingleCore(dag_tasks, tasks_info, 1);

  EXPECT_EQ(JobStartFinish(7, 69), schedule_actual[JobCEC(0, 0)]);
  EXPECT_EQ(JobStartFinish(0, 1), schedule_actual[JobCEC(1, 0)]);
  EXPECT_EQ(JobStartFinish(5, 6), schedule_actual[JobCEC(1, 1)]);
  EXPECT_EQ(JobStartFinish(1, 7), schedule_actual[JobCEC(2, 0)]);

  EXPECT_EQ(JobStartFinish(10, 11), schedule_actual[JobCEC(1, 2)]);
  EXPECT_EQ(JobStartFinish(15, 16), schedule_actual[JobCEC(1, 3)]);
  EXPECT_EQ(JobStartFinish(11, 17), schedule_actual[JobCEC(2, 1)]);

  EXPECT_EQ(JobStartFinish(20, 21), schedule_actual[JobCEC(1, 4)]);
  EXPECT_EQ(JobStartFinish(25, 26), schedule_actual[JobCEC(1, 5)]);
  EXPECT_EQ(JobStartFinish(21, 27), schedule_actual[JobCEC(2, 2)]);
}

TEST_F(PermutationTest27_n3, SimulateFixedPrioritySched) {
  Schedule schedule_actual = SimulateFixedPrioritySched(dag_tasks, tasks_info);

  EXPECT_EQ(JobStartFinish(7, 69), schedule_actual[JobCEC(0, 0)]);
  EXPECT_EQ(JobStartFinish(0, 1), schedule_actual[JobCEC(1, 0)]);
  EXPECT_EQ(JobStartFinish(5, 6), schedule_actual[JobCEC(1, 1)]);
  EXPECT_EQ(JobStartFinish(1, 7), schedule_actual[JobCEC(2, 0)]);

  EXPECT_EQ(JobStartFinish(10, 11), schedule_actual[JobCEC(1, 2)]);
  EXPECT_EQ(JobStartFinish(15, 16), schedule_actual[JobCEC(1, 3)]);
  EXPECT_EQ(JobStartFinish(11, 17), schedule_actual[JobCEC(2, 1)]);

  EXPECT_EQ(JobStartFinish(20, 21), schedule_actual[JobCEC(1, 4)]);
  EXPECT_EQ(JobStartFinish(25, 26), schedule_actual[JobCEC(1, 5)]);
  EXPECT_EQ(JobStartFinish(21, 27), schedule_actual[JobCEC(2, 2)]);
}

class PermutationTest28_n3 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n3_v28"); }
};

TEST_F(PermutationTest28_n3, SimulateFixedPrioritySched) {
  Schedule schedule_actual = SimulateFixedPrioritySched(dag_tasks, tasks_info);

  EXPECT_EQ(JobStartFinish(5, 40), schedule_actual[JobCEC(0, 0)]);
  EXPECT_EQ(JobStartFinish(0, 1), schedule_actual[JobCEC(1, 0)]);
  EXPECT_EQ(JobStartFinish(5, 6), schedule_actual[JobCEC(1, 1)]);
  EXPECT_EQ(JobStartFinish(0, 5), schedule_actual[JobCEC(2, 0)]);

  EXPECT_EQ(JobStartFinish(10, 11), schedule_actual[JobCEC(1, 2)]);
  EXPECT_EQ(JobStartFinish(15, 16), schedule_actual[JobCEC(1, 3)]);
  EXPECT_EQ(JobStartFinish(10, 15), schedule_actual[JobCEC(2, 1)]);

  EXPECT_EQ(JobStartFinish(20, 21), schedule_actual[JobCEC(1, 4)]);
  EXPECT_EQ(JobStartFinish(25, 26), schedule_actual[JobCEC(1, 5)]);
  EXPECT_EQ(JobStartFinish(20, 25), schedule_actual[JobCEC(2, 2)]);
}
int main(int argc, char **argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}