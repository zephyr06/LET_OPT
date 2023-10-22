
#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Baseline/ImplicitCommunication/ScheduleSimulation.h"
#include "sources/Baseline/Maia23.h"
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
#include "sources/Optimization/Variable.h"
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

TEST_F(PermutationTest18_n3, SimulateFixedPrioritySched) {
  Schedule schedule_actual = SimulateFixedPrioritySched(dag_tasks, tasks_info);

  EXPECT_EQ(JobStartFinish(0, 1), schedule_actual[JobCEC(0, 0)]);
  EXPECT_EQ(JobStartFinish(10, 11), schedule_actual[JobCEC(0, 1)]);
  EXPECT_EQ(JobStartFinish(4, 6), schedule_actual[JobCEC(1, 0)]);
  EXPECT_EQ(JobStartFinish(1, 4), schedule_actual[JobCEC(2, 0)]);
}

TEST_F(PermutationTest18_n3, SimulateFixedPrioritySched_OD_v1) {
  VariableOD variable(dag_tasks.tasks);
  variable[0].offset = 0;
  variable[0].deadline = 1;
  variable[1].offset = 5;
  variable[0].deadline = 7;
  variable[2].offset = 8;
  variable[0].deadline = 12;
  Schedule schedule_actual =
      SimulateFixedPrioritySched_OD(dag_tasks, tasks_info, variable);

  EXPECT_EQ(JobStartFinish(0, 1), schedule_actual[JobCEC(0, 0)]);
  EXPECT_EQ(JobStartFinish(10, 11), schedule_actual[JobCEC(0, 1)]);
  EXPECT_EQ(JobStartFinish(5, 7), schedule_actual[JobCEC(1, 0)]);
  EXPECT_EQ(JobStartFinish(8, 12), schedule_actual[JobCEC(2, 0)]);
}

class PermutationTest_n3_v38 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v38");
    dag_tasks.chains_ = {{0, 1, 2}};
    type_trait = "DataAge";
  }

 public:
  std::string type_trait;
};

class PermutationTest_n5_v66 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n5_v66");
    dag_tasks.chains_ = {{0, 1, 2}};
    type_trait = "ReactionTime";
  }

 public:
  std::string type_trait;
};

TEST_F(PermutationTest_n5_v66, SimulateFixedPrioritySched) {
  VariableOD variable(dag_tasks.tasks);
  variable[0].offset = 0;
  variable[0].deadline = 20;

  variable[1].offset = 0;
  variable[1].deadline = 40;

  variable[2].offset = 9;
  variable[2].deadline = 34;

  variable[3].offset = 14;
  variable[3].deadline = 20;

  variable[4].offset = 0;
  variable[4].deadline = 10;
  Schedule schedule_cur =
      SimulateFixedPrioritySched_OD(dag_tasks, tasks_info, variable);
  PrintSchedule(schedule_cur);
  VariableOD variable_after_Maia =
      GetMaia23VariableOD(dag_tasks, tasks_info, schedule_cur);
  EXPECT_EQ(0, variable_after_Maia[0].offset);
  EXPECT_EQ(5, variable_after_Maia[0].deadline);

  EXPECT_EQ(0, variable_after_Maia[1].offset);
  EXPECT_EQ(9, variable_after_Maia[1].deadline);

  EXPECT_EQ(9, variable_after_Maia[2].offset);
  EXPECT_EQ(11, variable_after_Maia[2].deadline);

  EXPECT_EQ(14, variable_after_Maia[3].offset);
  EXPECT_EQ(15, variable_after_Maia[3].deadline);

  EXPECT_EQ(0, variable_after_Maia[4].offset);
  EXPECT_EQ(5, variable_after_Maia[4].deadline);
  EXPECT_TRUE(CheckSchedulability(dag_tasks, tasks_info, schedule_cur,
                                  variable_after_Maia));
}

TEST_F(PermutationTest_n3_v38, ObtainObjAfterMaia) {
  VariableOD variable(dag_tasks.tasks);
  variable[0].offset = 0;
  variable[0].deadline = 1;
  variable[1].offset = 11;
  variable[1].deadline = 16;
  variable[2].offset = 6;
  variable[2].deadline = 9;
  Schedule schedule_actual =
      SimulateFixedPrioritySched_OD(dag_tasks, tasks_info, variable);

  EXPECT_EQ(JobStartFinish(0, 1), schedule_actual[JobCEC(0, 0)]);
  EXPECT_EQ(JobStartFinish(5, 6), schedule_actual[JobCEC(0, 1)]);
  EXPECT_EQ(JobStartFinish(11, 13), schedule_actual[JobCEC(1, 0)]);
  EXPECT_EQ(JobStartFinish(6, 8), schedule_actual[JobCEC(2, 0)]);
  EXPECT_EQ(JobStartFinish(16, 18), schedule_actual[JobCEC(2, 1)]);
  EXPECT_EQ(18,
            ObtainObjAfterMaia<ObjDataAge>(variable, dag_tasks, tasks_info));
}

TEST_F(PermutationTest18_n3, CheckSchedulability) {
  VariableOD variable(dag_tasks.tasks);
  variable[0].offset = 0;
  variable[0].deadline = 1;
  variable[1].offset = 5;
  variable[1].deadline = 7;
  variable[2].offset = 8;
  variable[2].deadline = 12;
  Schedule schedule =
      SimulateFixedPrioritySched_OD(dag_tasks, tasks_info, variable);
  EXPECT_TRUE(CheckSchedulability(dag_tasks, tasks_info, schedule, variable));
  schedule[JobCEC(0, 0)].finish = 11;
  EXPECT_FALSE(CheckSchedulability(dag_tasks, tasks_info, schedule, variable));
}
TEST_F(PermutationTest18_n3, ObtainObjAfterMaia) {
  VariableOD variable(dag_tasks.tasks);
  variable[0].offset = 0;
  variable[0].deadline = 1;
  variable[1].offset = 5;
  variable[1].deadline = 7;
  variable[2].offset = 8;
  variable[2].deadline = 12;
  Schedule schedule_actual =
      SimulateFixedPrioritySched_OD(dag_tasks, tasks_info, variable);

  EXPECT_EQ(JobStartFinish(0, 1), schedule_actual[JobCEC(0, 0)]);
  EXPECT_EQ(JobStartFinish(10, 11), schedule_actual[JobCEC(0, 1)]);
  EXPECT_EQ(JobStartFinish(5, 7), schedule_actual[JobCEC(1, 0)]);
  EXPECT_EQ(JobStartFinish(8, 12), schedule_actual[JobCEC(2, 0)]);
  EXPECT_EQ(12,
            ObtainObjAfterMaia<ObjDataAge>(variable, dag_tasks, tasks_info));
}

TEST_F(PermutationTest18_n3, SimulateFixedPrioritySched_OD_v2) {
  VariableOD variable(dag_tasks.tasks);
  variable[0].offset = 0;
  variable[1].offset = 3;
  variable[2].offset = 5;
  Schedule schedule_actual =
      SimulateFixedPrioritySched_OD(dag_tasks, tasks_info, variable);

  EXPECT_EQ(JobStartFinish(0, 1), schedule_actual[JobCEC(0, 0)]);
  EXPECT_EQ(JobStartFinish(10, 11), schedule_actual[JobCEC(0, 1)]);
  EXPECT_EQ(JobStartFinish(3, 5), schedule_actual[JobCEC(1, 0)]);
  EXPECT_EQ(JobStartFinish(5, 8), schedule_actual[JobCEC(2, 0)]);
}
TEST_F(PermutationTest18_n3, SimulateFixedPrioritySched_OD_v3) {
  VariableOD variable(dag_tasks.tasks);
  variable[0].offset = 0;
  variable[1].offset = 3;
  variable[2].offset = 3;
  Schedule schedule_actual =
      SimulateFixedPrioritySched_OD(dag_tasks, tasks_info, variable);

  EXPECT_EQ(JobStartFinish(0, 1), schedule_actual[JobCEC(0, 0)]);
  EXPECT_EQ(JobStartFinish(10, 11), schedule_actual[JobCEC(0, 1)]);
  EXPECT_EQ(JobStartFinish(6, 8), schedule_actual[JobCEC(1, 0)]);
  EXPECT_EQ(JobStartFinish(3, 6), schedule_actual[JobCEC(2, 0)]);
}

TEST_F(PermutationTest18_n3, GetMaia23VariableOD) {
  VariableOD variable_maia = GetMaia23VariableOD(dag_tasks, tasks_info);
  EXPECT_EQ(0, variable_maia[0].offset);
  EXPECT_EQ(1, variable_maia[0].deadline);

  EXPECT_EQ(4, variable_maia[1].offset);
  EXPECT_EQ(6, variable_maia[1].deadline);

  EXPECT_EQ(1, variable_maia[2].offset);
  EXPECT_EQ(4, variable_maia[2].deadline);
}

TEST_F(PermutationTest18_n3, RT_OBJ) {
  ScheduleResult res = PerformMaia23Analysis<ObjReactionTime>(dag_tasks);
  EXPECT_EQ(34, res.obj_);
  res = PerformMaia23Analysis<ObjDataAge>(dag_tasks);
  EXPECT_EQ(24, res.obj_);
}

class PermutationTest28_n3 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n3_v28"); }
};

TEST_F(PermutationTest28_n3, SimulateFixedPrioritySched) {
  Schedule schedule_actual = SimulateFixedPrioritySched(dag_tasks, tasks_info);

  EXPECT_EQ(JobStartFinish(5, 40), schedule_actual[JobCEC(0, 0)]);

  EXPECT_EQ(JobStartFinish(0, 1), schedule_actual[JobCEC(1, 0)]);
  EXPECT_EQ(JobStartFinish(5, 6), schedule_actual[JobCEC(1, 1)]);
  EXPECT_EQ(JobStartFinish(10, 11), schedule_actual[JobCEC(1, 2)]);
  EXPECT_EQ(JobStartFinish(15, 16), schedule_actual[JobCEC(1, 3)]);
  EXPECT_EQ(JobStartFinish(20, 21), schedule_actual[JobCEC(1, 4)]);
  EXPECT_EQ(JobStartFinish(25, 26), schedule_actual[JobCEC(1, 5)]);

  EXPECT_EQ(JobStartFinish(0, 5), schedule_actual[JobCEC(2, 0)]);
  EXPECT_EQ(JobStartFinish(10, 15), schedule_actual[JobCEC(2, 1)]);
  EXPECT_EQ(JobStartFinish(20, 25), schedule_actual[JobCEC(2, 2)]);
}

TEST_F(PermutationTest28_n3, GetMaia23VariableOD) {
  VariableOD variable_maia = GetMaia23VariableOD(dag_tasks, tasks_info);
  EXPECT_EQ(5, variable_maia[0].offset);
  EXPECT_EQ(40, variable_maia[0].deadline);

  EXPECT_EQ(0, variable_maia[1].offset);
  EXPECT_EQ(1, variable_maia[1].deadline);

  EXPECT_EQ(0, variable_maia[2].offset);
  EXPECT_EQ(5, variable_maia[2].deadline);
}
TEST_F(PermutationTest28_n3, RT_OBJ) {
  ScheduleResult res = PerformMaia23Analysis<ObjReactionTime>(dag_tasks);
  EXPECT_EQ(55 - 5, res.obj_);
  res = PerformMaia23Analysis<ObjDataAge>(dag_tasks);
  EXPECT_EQ(145 - 5, res.obj_);
}

class PermutationTest_n5_v65 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n5_v65"); }
};

TEST_F(PermutationTest_n5_v65, CategorizeTaskSet) {
  EXPECT_EQ(0, dag_tasks.task_id2task_index_within_processor_[3]);
  EXPECT_EQ(1, dag_tasks.task_id2task_index_within_processor_[0]);
  EXPECT_EQ(3, dag_tasks.processor2taskset_[2][0].id);
  EXPECT_EQ(0, dag_tasks.processor2taskset_[2][1].id);
}

TEST_F(PermutationTest_n5_v65, RT_OBJ) {
  VariableOD variable_od_Maia = GetMaia23VariableOD(dag_tasks, tasks_info);
  Schedule schedule_actual = Variable2Schedule(tasks_info, variable_od_Maia);
  variable_od_Maia.print();
  PrintSchedule(schedule_actual);
  // EXPECT_TRUE(
  //     ExamSchedulabilityVariable(variable_od_Maia, dag_tasks, tasks_info));
}

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}