
#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Baseline/Bradatsch16.h"
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
TEST_F(PermutationTest18_n3, GetVariableOD_Bradatsch16) {
  auto rta = GetResponseTimeTaskSet(dag_tasks);
  PrintRta(rta);
  VariableOD variable = GetVariableOD_Bradatsch16(dag_tasks);
  EXPECT_EQ(1, variable[0].deadline);
  EXPECT_EQ(3, variable[1].deadline);
  EXPECT_EQ(6, variable[2].deadline);
}
TEST_F(PermutationTest18_n3, PerformBradatsch16LETAnalysis) {
  EXPECT_EQ(10 + 20 + 6,
            PerformBradatsch16LETAnalysis<ObjDataAge>(dag_tasks).obj_);
}
class PermutationTest23_n3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v23");
    dag_tasks.chains_ = {{0, 1, 2}};
    type_trait = "DataAge";
  }

 public:
  std::string type_trait;
};
TEST_F(PermutationTest23_n3, PerformBradatsch16LETAnalysis) {
  EXPECT_EQ(3 + 150 + 150,
            PerformBradatsch16LETAnalysis<ObjDataAge>(dag_tasks).obj_);
}
int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}