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
                           dag_tasks, dag_tasks.chains_, 6));

  EXPECT_EQ(1 + 2 + 3, GetApproximatedObjBound<ObjReactionTime>(
                           dag_tasks, dag_tasks.chains_, 20));

  EXPECT_EQ(100 - 9 - 14, GetApproximatedObjBound<ObjReactionTime>(
                              dag_tasks, dag_tasks.chains_, 100));
}

TEST_F(PermutationTest18_n3, PerformTOM_OPT_SortBoundImproved) {
  VariableOD variable(dag_tasks.GetTaskSet());
  variable[0].offset = 3;
  variable[0].deadline = 4;
  variable[1].offset = 0;
  variable[1].deadline = 20;
  variable[2].offset = 14;
  variable[2].deadline = 20;
  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  VariableRange variable_range =
      FindVariableRangeImproved(dag_tasks, rta, variable);
  variable_range.lower_bound.print();
  variable_range.upper_bound.print();

  EXPECT_EQ(100 - 6 - 14, GetApproximatedObjBoundImproved<ObjReactionTime>(
                              dag_tasks, dag_tasks.chains_, 100, variable));
}

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}