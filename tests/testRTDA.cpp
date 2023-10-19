
#include <algorithm>  // for std::gcd

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Baseline/JobCommunications.h"
#include "sources/Baseline/Martinez18.h"
#include "sources/Baseline/StandardLET.h"
#include "sources/ObjectiveFunction/RTDA_Factor.h"
#include "sources/Optimization/Variable.h"
#include "tests/testEnv.cpp"
using namespace DAG_SPACE;

class PermutationTest_n3_v35 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v35");
    type_trait = "SensorFusion";
    dag_tasks.chains_ = {{0, 1, 2}};
  }

 public:
  std::string type_trait;
};

TEST_F(PermutationTest_n3_v35, SF_Obj_Implicit_DA) {
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);

  EXPECT_EQ(36, ObjDataAgeFromSChedule(dag_tasks, tasks_info, dag_tasks.chains_,
                                       schedule));
}

TEST_F(PermutationTest_n3_v35, SF_Obj_Implicit_RT) {
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);

  EXPECT_EQ(56, ObjReactionTimeFromSChedule(dag_tasks, tasks_info,
                                            dag_tasks.chains_, schedule));
}

// TEST_F(PermutationTest_n3_v35, SF_Obj_Implicit) {
//   ScheduleResult res;
//   dag_tasks.chains_ = GetChainsForSF(dag_tasks);
//   res = PerformImplicitCommuAnalysis<DAG_SPACE::ObjSensorFusion>(dag_tasks);
//   EXPECT_EQ(9, res.obj_);
// }

int main(int argc, char **argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}