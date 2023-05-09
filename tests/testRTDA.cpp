
#include <algorithm>  // for std::gcd

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Baseline/JobCommunications.h"
#include "sources/Baseline/Martinez18.h"
#include "sources/Baseline/StandardLET.h"
#include "sources/ObjectiveFunction/RTDA_Factor.h"
#include "sources/Optimization/Variable.h"
#include "testEnv.cpp"
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

std::vector<RTDA> GetMaxRTDAs(const std::vector<std::vector<RTDA>> &rtdaVec) {
  std::vector<RTDA> maxRtda;
  for (uint i = 0; i < rtdaVec.size(); i++)
    maxRtda.push_back(GetMaxRTDA(rtdaVec.at(i)));

  return maxRtda;
}
std::vector<std::vector<RTDA>> GetRTDAFromAllChains(
    const DAG_Model &dagTasks, const TaskSetInfoDerived &tasksInfo,
    const Schedule &shcedule_jobs) {
  std::vector<std::vector<RTDA>> rtdaVec;
  for (uint i = 0; i < dagTasks.chains_.size(); i++) {
    auto rtdaVecTemp =
        GetRTDAFromSingleJob(tasksInfo, dagTasks.chains_[i], shcedule_jobs);
    rtdaVec.push_back(rtdaVecTemp);
  }
  return rtdaVec;
}

double ObjDataAgeFromSChedule(const DAG_Model &dagTasks,
                              const TaskSetInfoDerived &tasksInfo,
                              const Schedule &shcedule_jobs) {
  std::vector<std::vector<RTDA>> rtdaVec =
      GetRTDAFromAllChains(dagTasks, tasksInfo, shcedule_jobs);
  std::vector<RTDA> maxRtda = GetMaxRTDAs(rtdaVec);

  double res = 0;
  for (const auto &rtda : maxRtda) {
    res += rtda.dataAge;
  }
  return res;
}
double ObjReactionTimeFromSChedule(const DAG_Model &dagTasks,
                                   const TaskSetInfoDerived &tasksInfo,
                                   const Schedule &shcedule_jobs) {
  std::vector<std::vector<RTDA>> rtdaVec =
      GetRTDAFromAllChains(dagTasks, tasksInfo, shcedule_jobs);
  std::vector<RTDA> maxRtda = GetMaxRTDAs(rtdaVec);

  double res = 0;
  for (const auto &rtda : maxRtda) {
    res += rtda.reactionTime;
  }
  return res;
}

TEST_F(PermutationTest_n3_v35, SF_Obj_Implicit_DA) {
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);

  EXPECT_EQ(36, ObjDataAgeFromSChedule(dag_tasks, tasks_info, schedule));
}

TEST_F(PermutationTest_n3_v35, SF_Obj_Implicit_RT) {
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);

  EXPECT_EQ(56, ObjReactionTimeFromSChedule(dag_tasks, tasks_info, schedule));
}

TEST_F(PermutationTest_n3_v35, SF_Obj_Implicit) {
  ScheduleResult res;
  dag_tasks.chains_ = GetChainsForSF(dag_tasks);
  res = PerformImplicitCommuAnalysis<DAG_SPACE::ObjSensorFusion>(dag_tasks);
  EXPECT_EQ(9, res.obj_);
}

int main(int argc, char **argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}