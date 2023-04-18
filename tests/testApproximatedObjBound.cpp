#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Baseline/OptSortedBound.h"
#include "sources/Optimization/Variable.h"
#include "sources/TaskModel/DAG_Model.h"
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

TEST_F(PermutationTest18_n3, GetObjLowerBound) {
  EXPECT_EQ(1 + 2 + 3,
            GetObjLowerBound<ObjReactionTime>(dag_tasks, dag_tasks.chains_));
}

TEST_F(PermutationTest18_n3, GetApproximatedObjBound) {
  EXPECT_EQ(1 + 2 + 3, GetApproximatedObjBound<ObjReactionTime>(
                           dag_tasks, dag_tasks.chains_, {6}));

  EXPECT_EQ(1 + 2 + 3, GetApproximatedObjBound<ObjReactionTime>(
                           dag_tasks, dag_tasks.chains_, {20}));

  EXPECT_EQ(100 - 9 - 14, GetApproximatedObjBound<ObjReactionTime>(
                              dag_tasks, dag_tasks.chains_, {100}));
}
class PermutationTest18_n3_2chain : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v18");
    dag_tasks.chains_ = {{0, 1, 2}};
    dag_tasks.chains_.push_back({0, 1});
    type_trait = "DataAge";
  }

 public:
  std::string type_trait;
};
TEST_F(PermutationTest18_n3_2chain, GetApproximatedObjBound) {
  EXPECT_EQ(1 + 2 + 3 + 3, GetApproximatedObjBound<ObjReactionTime>(
                               dag_tasks, dag_tasks.chains_, {6, 3}));

  EXPECT_EQ(1 + 2 + 3 + 3, GetApproximatedObjBound<ObjReactionTime>(
                               dag_tasks, dag_tasks.chains_, {20, 3}));

  EXPECT_EQ(100 - 9 - 14 + 100 - 9 - 17,
            GetApproximatedObjBound<ObjReactionTime>(
                dag_tasks, dag_tasks.chains_, {100, 100}));

  EXPECT_EQ(100 - 9 - 14 + 3, GetApproximatedObjBound<ObjReactionTime>(
                                  dag_tasks, dag_tasks.chains_, {100, 5}));
}

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}