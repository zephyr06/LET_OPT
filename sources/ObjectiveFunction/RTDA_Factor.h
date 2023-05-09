#pragma once

#include "sources/Baseline/JobScheduleInfo.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/JobCEC.h"

namespace DAG_SPACE {
struct RTDA {
  double reactionTime;
  double dataAge;
  RTDA() : reactionTime(-1), dataAge(-1) {}
  RTDA(double r, double d) : reactionTime(r), dataAge(d) {}
  void print() {
    std::cout << "Reaction time is " << reactionTime << ", data age is "
              << dataAge << std::endl;
  }
};

RTDA GetMaxRTDA(const std::vector<RTDA> &resVec);

std::vector<RTDA> GetRTDAFromSingleJob(const TaskSetInfoDerived &tasksInfo,
                                       const std::vector<int> &causeEffectChain,
                                       const Schedule &shcedule_jobs,
                                       double precision = 0);

RTDA GetMaxRTDA(const TaskSetInfoDerived &tasksInfo,
                const std::vector<int> &causeEffectChain,
                Schedule &shcedule_jobs);

double ObjRTDA(const RTDA &rtda);
double ObjRTDA(const std::vector<RTDA> &rtdaVec);

std::vector<RTDA> GetMaxRTDAs(const std::vector<std::vector<RTDA>> &rtdaVec);

std::vector<std::vector<RTDA>> GetRTDAFromAllChains(
    const DAG_Model &dagTasks, const TaskSetInfoDerived &tasksInfo,
    const Schedule &shcedule_jobs);

double ObjDataAgeFromSChedule(const DAG_Model &dagTasks,
                              const TaskSetInfoDerived &tasksInfo,
                              const Schedule &shcedule_jobs);

double ObjReactionTimeFromSChedule(const DAG_Model &dagTasks,
                                   const TaskSetInfoDerived &tasksInfo,
                                   const Schedule &shcedule_jobs);

}  // namespace DAG_SPACE