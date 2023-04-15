
#include <algorithm>  // for std::gcd

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Baseline/JobCommunications.h"
#include "sources/Baseline/Martinez18.h"
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

TEST_F(PermutationTest18_n3, GetPossibleReadingJobs) {
  VariableOD variable_od(tasks);
  variable_od[1].offset = 10;
  variable_od[2].offset = 10;
  std::vector<int> chain = {0, 1, 2};
  Martinez18Perm mart_perm;
  mart_perm.push_back(0);
  mart_perm.push_back(1);
  mart_perm.push_back(3);
  EXPECT_EQ(JobCEC(0, 0), GetPossibleReadingJobs(JobCEC(1, 0), task0, 20,
                                                 tasks_info, variable_od));
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
  EXPECT_EQ(50,
            ObjDataAgeFromVariable(mart_perm, dag_tasks, tasks_info, chain));
}

TEST_F(PermutationTest18_n3, Obj_v2) {
  VariableOD variable_od(tasks);
  variable_od[1].offset = 10;
  variable_od[2].offset = 10;
  std::vector<int> chain = {0, 1, 2};
  Martinez18Perm mart_perm;
  mart_perm.push_back(0);
  mart_perm.push_back(10);
  mart_perm.push_back(3);
  EXPECT_EQ(40,
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
  EXPECT_FALSE(mart_task_perms.EvaluateMartSchedulability(mart_perm));
}

TEST_F(PermutationTest18_n3, Iterate) {
  Martinez18TaskSetPerms mart_task_perms(dag_tasks, dag_tasks.chains_[0]);
  EXPECT_EQ(40, mart_task_perms.PerformOptimization());
  EXPECT_EQ(40, PerformOPT_Martinez18_DA(dag_tasks).obj_);
  EXPECT_EQ(11 * 21, mart_task_perms.iteration_count_);
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

TEST_F(PermutationTest4_n3, Obj_v1) {
  std::vector<int> chain = {0, 1, 2};
  Martinez18Perm mart_perm;
  mart_perm.push_back(0);
  mart_perm.push_back(5);
  mart_perm.push_back(9);
  EXPECT_EQ(45,
            ObjDataAgeFromVariable(mart_perm, dag_tasks, tasks_info, chain));
}
TEST_F(PermutationTest4_n3, Iterate) {
  Martinez18TaskSetPerms mart_task_perms(dag_tasks, dag_tasks.chains_[0]);
  EXPECT_EQ(45, mart_task_perms.PerformOptimization());
  EXPECT_EQ(45, PerformOPT_Martinez18_DA(dag_tasks).obj_);
  EXPECT_EQ((5 + 1) * (15 + 1), mart_task_perms.iteration_count_);
}

class PermutationTest9_n10 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n10_v9");
    type_trait = "DataAge";
  }

 public:
  std::string type_trait;
};
// TEST_F(PermutationTest9_n10, TimeOut) {
//   Martinez18TaskSetPerms mart_task_perms(dag_tasks, dag_tasks.chains_[0]);
//   EXPECT_THAT(mart_task_perms.PerformOptimization(),
//               ::testing::Le(INFEASIBLE_OBJ - 1));
//   EXPECT_EQ(201 * 201 * 21, mart_task_perms.iteration_count_);
// }

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}