
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
class PermutationTest53_n5 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n5_v53"); }
};

TEST_F(PermutationTest53_n5, ReadSF_Fork) {
  EXPECT_EQ(2, dag_tasks.sf_forks_.size());
  EXPECT_EQ(0, dag_tasks.sf_forks_[0].source[0]);
  EXPECT_EQ(1, dag_tasks.sf_forks_[0].source[1]);
  EXPECT_EQ(2, dag_tasks.sf_forks_[0].sink);

  EXPECT_EQ(0, dag_tasks.sf_forks_[1].source[0]);
  EXPECT_EQ(1, dag_tasks.sf_forks_[1].source[1]);
  EXPECT_EQ(3, dag_tasks.sf_forks_[1].sink);
}
class PermutationTest54_n5 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n5_v54"); }
};

TEST_F(PermutationTest54_n5, ReadSF_Fork) {
  EXPECT_EQ(2, dag_tasks.sf_forks_.size());
  EXPECT_EQ(1, dag_tasks.sf_forks_[0].source[0]);
  EXPECT_EQ(2, dag_tasks.sf_forks_[0].source[1]);
  EXPECT_EQ(3, dag_tasks.sf_forks_[0].sink);

  EXPECT_EQ(0, dag_tasks.sf_forks_[1].source[0]);
  EXPECT_EQ(3, dag_tasks.sf_forks_[1].source[1]);
  EXPECT_EQ(4, dag_tasks.sf_forks_[1].sink);
}

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}