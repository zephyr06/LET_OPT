
#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Baseline/ImplicitCommunication/ScheduleSimulation.h"
#include "sources/Baseline/Maia23.h"
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
#include "sources/Optimization/Variable.h"
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

TEST_F(PermutationTest18_n3, SimulateFixedPrioritySched) {
  Schedule schedule_actual = SimulateFixedPrioritySched(dag_tasks, tasks_info);

  EXPECT_EQ(JobStartFinish(0, 1), schedule_actual[JobCEC(0, 0)]);
  EXPECT_EQ(JobStartFinish(10, 11), schedule_actual[JobCEC(0, 1)]);
  EXPECT_EQ(JobStartFinish(4, 6), schedule_actual[JobCEC(1, 0)]);
  EXPECT_EQ(JobStartFinish(1, 4), schedule_actual[JobCEC(2, 0)]);
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

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}