
#include <algorithm>  // for std::gcd

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Baseline/JobCommunications.h"
#include "sources/Baseline/Martinez18.h"
#include "sources/Baseline/StandardLET.h"
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

TEST_F(PermutationTest18_n3, FindPossibleOffsets) {
  Martinez18TaskSetPerms mart_task_perms(dag_tasks, dag_tasks.chains_[0]);
  EXPECT_EQ(3, mart_task_perms.possible_offsets_map_.size());
  EXPECT_EQ(1, mart_task_perms.possible_offsets_map_[0].size());
  EXPECT_EQ(0, mart_task_perms.possible_offsets_map_[0][0]);
  EXPECT_EQ(11, mart_task_perms.possible_offsets_map_[1].size());
  EXPECT_EQ(9, mart_task_perms.possible_offsets_map_[1][9]);
  EXPECT_EQ(21, mart_task_perms.possible_offsets_map_[2].size());
}
TEST_F(PermutationTest18_n3, Martinez18Perm) {
  Martinez18Perm mart_perm;
  mart_perm.push_back(0);
  mart_perm.push_back(1);
  mart_perm.push_back(3);
  EXPECT_EQ(3, mart_perm.size());
  EXPECT_EQ(1, mart_perm[1]);
  EXPECT_EQ(3, mart_perm[2]);
}

TEST_F(PermutationTest18_n3, GetLastReadJob) {
  std::vector<int> chain = {0, 1, 2};
  Martinez18Perm mart_perm;
  mart_perm.push_back(0);
  mart_perm.push_back(10);
  mart_perm.push_back(10);
  VariableOD variable_od = MartPerm2VariableOD(mart_perm, dag_tasks);
  Schedule schedule_actual = VariableMart2Schedule(tasks_info, variable_od);
  EXPECT_EQ(JobCEC(0, 0), GetLastReadJob(JobCEC(1, 0), task0, 20, tasks_info,
                                         schedule_actual));
}
TEST_F(PermutationTest18_n3, GetLastReadJobv2) {
  std::vector<int> chain = {0, 1, 2};
  Martinez18Perm mart_perm;
  mart_perm.push_back(1);
  mart_perm.push_back(10);
  mart_perm.push_back(10);
  VariableOD variable_od = MartPerm2VariableOD(mart_perm, dag_tasks);
  Schedule schedule_actual = VariableMart2Schedule(tasks_info, variable_od);
  EXPECT_EQ(JobCEC(0, -1), GetLastReadJob(JobCEC(1, 0), task0, 20, tasks_info,
                                          schedule_actual));
}
TEST_F(PermutationTest18_n3, GetLastReadJobv3) {
  std::vector<int> chain = {0, 1, 2};
  Martinez18Perm mart_perm;
  mart_perm.push_back(0);
  mart_perm.push_back(1);
  mart_perm.push_back(3);
  VariableOD variable_od = MartPerm2VariableOD(mart_perm, dag_tasks);
  Schedule schedule_actual = VariableMart2Schedule(tasks_info, variable_od);
  EXPECT_EQ(JobCEC(0, -1), GetLastReadJob(JobCEC(1, 0), task0, 20, tasks_info,
                                          schedule_actual));
  EXPECT_EQ(JobCEC(0, -3), GetLastReadJob(JobCEC(1, -1), task0, 20, tasks_info,
                                          schedule_actual));
  EXPECT_EQ(JobCEC(1, -2), GetLastReadJob(JobCEC(2, -1), task1, 20, tasks_info,
                                          schedule_actual));
}
TEST_F(PermutationTest18_n3, GetLastReadJobv4) {
  std::vector<int> chain = {0, 1, 2};
  Martinez18Perm mart_perm;
  mart_perm.push_back(0);
  mart_perm.push_back(10);
  mart_perm.push_back(3);
  VariableOD variable_od = MartPerm2VariableOD(mart_perm, dag_tasks);
  Schedule schedule_actual = VariableMart2Schedule(tasks_info, variable_od);
  EXPECT_EQ(JobCEC(1, -2), GetLastReadJob(JobCEC(2, 0), task1, 20, tasks_info,
                                          schedule_actual));
}

TEST_F(PermutationTest18_n3, GetVariable) {
  std::vector<int> chain = {0, 1, 2};
  Martinez18Perm mart_perm;
  mart_perm.push_back(0);
  mart_perm.push_back(1);
  mart_perm.push_back(3);
  auto variable_od = MartPerm2VariableOD(mart_perm, dag_tasks);
  EXPECT_EQ(3, variable_od[2].offset);
  EXPECT_EQ(23, variable_od[2].deadline);
}

TEST_F(PermutationTest18_n3, Obj_v1) {
  VariableOD variable_od(tasks);
  variable_od[1].offset = 1;
  variable_od[2].offset = 3;
  std::vector<int> chain = {0, 1, 2};
  Martinez18Perm mart_perm;
  mart_perm.push_back(0);
  mart_perm.push_back(1);
  mart_perm.push_back(3);
  EXPECT_EQ(23 + 30,
            ObjDataAgeFromVariable(mart_perm, dag_tasks, tasks_info, chain));
}

TEST_F(PermutationTest18_n3, Obj_v2) {
  VariableOD variable_od(tasks);
  variable_od[1].offset = 10;
  variable_od[2].offset = 3;
  std::vector<int> chain = {0, 1, 2};
  Martinez18Perm mart_perm;
  mart_perm.push_back(0);
  mart_perm.push_back(10);
  mart_perm.push_back(3);
  EXPECT_EQ(23 + 40,
            ObjDataAgeFromVariable(mart_perm, dag_tasks, tasks_info, chain));
}
TEST_F(PermutationTest18_n3, Obj_v3) {
  std::vector<int> chain = {0, 1, 2};
  Martinez18Perm mart_perm;
  mart_perm.push_back(1);
  mart_perm.push_back(10);
  mart_perm.push_back(10);
  EXPECT_EQ(50 - (-9),
            ObjDataAgeFromVariable(mart_perm, dag_tasks, tasks_info, chain));
}
TEST_F(PermutationTest18_n3, EvaluateMartSchedulability) {
  Martinez18Perm mart_perm(3);
  mart_perm[0] = 0;
  mart_perm[1] = 10;
  mart_perm[2] = 10;
  Martinez18TaskSetPerms mart_task_perms(dag_tasks, dag_tasks.chains_[0]);
  EXPECT_TRUE(mart_task_perms.EvaluateMartSchedulability(mart_perm));
  mart_perm[1] = 18;
  EXPECT_TRUE(mart_task_perms.EvaluateMartSchedulability(mart_perm));
}

TEST_F(PermutationTest18_n3, Iterate) {
  Martinez18TaskSetPerms mart_task_perms(dag_tasks, dag_tasks.chains_[0]);
  EXPECT_EQ(50, mart_task_perms.PerformOptimization());
  EXPECT_EQ(50, PerformOPT_Martinez18_DA(dag_tasks).obj_);
  EXPECT_EQ(11 * 21, mart_task_perms.iteration_count_);
}

class PermutationTest51_n5 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n5_v51");
    type_trait = "DataAge";
  }

 public:
  std::string type_trait;
};

TEST_F(PermutationTest51_n5, GetLastReadJob) {
  VariableOD variable_od(tasks);
  variable_od[4].offset = 100;
  variable_od[2].offset = 0;
  Schedule schedule_actual = Variable2Schedule(tasks_info, variable_od);
  EXPECT_EQ(JobCEC(2, 9), GetLastReadJob(JobCEC(4, 0), dag_tasks.GetTask(2),
                                         200, tasks_info, schedule_actual));
}

class PermutationTest29_n3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v29");
    type_trait = "DataAge";
  }

 public:
  std::string type_trait;
};
TEST_F(PermutationTest29_n3, GetLastReadJob) {
  std::vector<int> chain = {0, 1, 2};
  Martinez18Perm mart_perm;
  mart_perm.push_back(0);
  mart_perm.push_back(3);
  mart_perm.push_back(0);
  VariableOD variable_od = MartPerm2VariableOD(mart_perm, dag_tasks);
  Schedule schedule_actual = VariableMart2Schedule(tasks_info, variable_od);
  EXPECT_EQ(JobStartFinish(3, 8), schedule_actual[JobCEC(1, 0)]);
  EXPECT_EQ(JobCEC(1, -2), GetLastReadJob(JobCEC(2, 0), task1, 10, tasks_info,
                                          schedule_actual));
  EXPECT_EQ(JobCEC(1, -2), GetLastReadJob(JobCEC(2, 1), task1, 10, tasks_info,
                                          schedule_actual));
}

TEST_F(PermutationTest29_n3, base) {
  dag_tasks.chains_[0] = {0, 1, 2};
  auto res = PerformStandardLETAnalysis<ObjDataAge>(dag_tasks);
  EXPECT_EQ(26, res.obj_);
}
TEST_F(PermutationTest29_n3, Iterate) {
  dag_tasks.chains_[0] = {0, 1, 2};
  Martinez18TaskSetPerms mart_task_perms(dag_tasks, dag_tasks.chains_[0]);
  EXPECT_EQ(25, mart_task_perms.PerformOptimization());
  EXPECT_EQ(25, PerformOPT_Martinez18_DA(dag_tasks).obj_);
}

class PermutationTest52_n5 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n5_v52"); }
};
TEST_F(PermutationTest52_n5, GetLastReadJob) {
  Martinez18Perm mart_perm;
  mart_perm.push_back(0);
  mart_perm.push_back(0);
  mart_perm.push_back(0);
  VariableOD variable_od = MartPerm2VariableOD(mart_perm, dag_tasks);
  Schedule schedule_actual = VariableMart2Schedule(tasks_info, variable_od);
  EXPECT_EQ(JobCEC(1, -1), GetLastReadJob(JobCEC(2, 0), dag_tasks.GetTask(1),
                                          200, tasks_info, schedule_actual));
}
TEST_F(PermutationTest52_n5, Iterate) {  // test if dead
  Martinez18TaskSetPerms mart_task_perms(dag_tasks, dag_tasks.chains_[0]);
  mart_task_perms.PerformOptimization();
  // EXPECT_EQ(25, PerformOPT_Martinez18_DA(dag_tasks).obj_);
}

// class PermutationTest9_n10 : public PermutationTestBase {
//   void SetUp() override {
//     SetUpBase("test_n10_v9");
//     type_trait = "DataAge";
//   }

//  public:
//   std::string type_trait;
// };
// TEST_F(PermutationTest9_n10, TimeOut) {
//   Martinez18TaskSetPerms mart_task_perms(dag_tasks, dag_tasks.chains_[0]);
//   EXPECT_THAT(mart_task_perms.PerformOptimization(),
//               ::testing::Le(INFEASIBLE_OBJ - 1));
//   EXPECT_EQ(201 * 201 * 21, mart_task_perms.iteration_count_);
// }
class PermutationTest4_n3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v4");
    dag_tasks.chains_ = {{0, 1, 2}};
    type_trait = "DataAge";
  }

 public:
  std::string type_trait;
};

TEST_F(PermutationTest4_n3, Obj_v1) {
  std::vector<int> chain = {0, 1, 2};
  Martinez18Perm mart_perm;
  mart_perm.push_back(0);
  mart_perm.push_back(5);
  mart_perm.push_back(9);
  EXPECT_EQ(24 - (-30),
            ObjDataAgeFromVariable(mart_perm, dag_tasks, tasks_info, chain));
}
TEST_F(PermutationTest4_n3, Iterate) {
  Martinez18TaskSetPerms mart_task_perms(dag_tasks, dag_tasks.chains_[0]);
  EXPECT_EQ(45, mart_task_perms.PerformOptimization());
  EXPECT_EQ(45, PerformOPT_Martinez18_DA(dag_tasks).obj_);
  EXPECT_EQ((5 + 1) * (15 + 1), mart_task_perms.iteration_count_);
}
int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}