
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

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}