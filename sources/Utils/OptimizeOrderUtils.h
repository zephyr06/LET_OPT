#pragma once
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/MatirxConvenient.h"
#include "sources/Utils/Parameters.h"
// #include "sources/Factors/DDL_ConstraintFactor.h"
#include "sources/Factors/Interval.h"
#include "sources/Factors/RTDA_Factor.h"
#include "sources/Factors/SensorFusionFactor.h"
#include "sources/Optimization/SFOrder.h"
#include "sources/Optimization/ScheduleOptions.h"
#include "sources/Optimization/ScheduleSimulation.h"

namespace OrderOptDAG_SPACE {

struct ScheduleResult {
  VectorDynamic startTimeVector_;
  bool schedulable_;
  double obj_;
  double timeTaken_;
  std::vector<uint> processorJobVec_;
  LLint countOutermostWhileLoop_;
  LLint countMakeProgress_;
  LLint countIterationStatus_;
  bool discardResult_ = false; // if true, we'll discard this analysis result when analyzing results later

  ScheduleResult() { obj_ = -1; }
  ScheduleResult(SFOrder sfOrder, VectorDynamic startTimeVector, bool schedulable, double obj)
      : startTimeVector_(startTimeVector), schedulable_(schedulable), obj_(obj) //, rtda_(rtda)
  {
    timeTaken_ = 0;
    countOutermostWhileLoop_ = 0;
    countMakeProgress_ = 0;
    countIterationStatus_ = 0;
  }
  ScheduleResult(SFOrder sfOrder, VectorDynamic startTimeVector, bool schedulable, double obj,
                 std::vector<uint> processorJobVec)
      : startTimeVector_(startTimeVector), schedulable_(schedulable), obj_(obj),
        processorJobVec_(processorJobVec) {
    timeTaken_ = 0;
    countOutermostWhileLoop_ = 0;
    countMakeProgress_ = 0;
    countIterationStatus_ = 0;
  }

  void print() { std::cout << "Objective is: " << obj_ << std::endl; }
};

void PrintResultAnalyzation(const ScheduleResult &scheduleResult, const DAG_Model &dagTasks);

bool CheckDDLConstraint(const DAG_Model &dagTasks, const RegularTaskSystem::TaskSetInfoDerived &tasksInfo,
                        const VectorDynamic &startTimeVector);

std::vector<std::vector<Interval>>
ExtractJobsPerProcessor(const DAG_Model &dagTasks, const RegularTaskSystem::TaskSetInfoDerived &tasksInfo,
                        const VectorDynamic &startTimeVector, const std::vector<uint> &processorJobVec,
                        int processorNum);

double DBF_Error(const DAG_Model &dagTasks, const RegularTaskSystem::TaskSetInfoDerived &tasksInfo,
                 const VectorDynamic &startTimeVector, const std::vector<uint> &processorJobVec,
                 int processorNum);

// Exams DBF constraints
bool ExamDBF_Feasibility(const DAG_Model &dagTasks, const RegularTaskSystem::TaskSetInfoDerived &tasksInfo,
                         const VectorDynamic &startTimeVector, const std::vector<uint> &processorJobVec,
                         int processorNum);

inline bool ExamDDL_Feasibility(const DAG_Model &dagTasks,
                                const RegularTaskSystem::TaskSetInfoDerived &tasksInfo,
                                const VectorDynamic &startTimeVector) {
  return CheckDDLConstraint(dagTasks, tasksInfo, startTimeVector);
}

bool ExamBasic_Feasibility(const DAG_Model &dagTasks, const RegularTaskSystem::TaskSetInfoDerived &tasksInfo,
                           const VectorDynamic &startTimeVector, const std::vector<uint> &processorJobVec,
                           int processorNum);

bool ExamAll_Feasibility(const DAG_Model &dagTasks, const RegularTaskSystem::TaskSetInfoDerived &tasksInfo,
                         const VectorDynamic &startTimeVector, const std::vector<uint> &processorJobVec,
                         int processorNum, double sfBound, double freshnessBound);

template <typename ObjectiveFunctionBase>
ScheduleResult ScheduleDAGLS_LFT(const DAG_Model &dagTasks,
                                 const OptimizeSF::ScheduleOptions &scheduleOptions, double sfBound,
                                 double freshnessBound) {
  const TaskSet &tasks = dagTasks.tasks;
  RegularTaskSystem::TaskSetInfoDerived tasksInfo(tasks);
  std::vector<uint> processorJobVec;
  VectorDynamic initialSTV =
      ListSchedulingLFTPA(dagTasks, tasksInfo, scheduleOptions.processorNum_, processorJobVec);

  ScheduleResult res;
  res.startTimeVector_ = initialSTV;
  if (ObjectiveFunctionBase::type_trait == "RTDAExperimentObj") {
    res.schedulable_ = ExamBasic_Feasibility(dagTasks, tasksInfo, initialSTV, processorJobVec,
                                             scheduleOptions.processorNum_);
  } else if (ObjectiveFunctionBase::type_trait == "RTSS21ICObj") {
    res.schedulable_ = ExamAll_Feasibility(dagTasks, tasksInfo, initialSTV, processorJobVec,
                                           scheduleOptions.processorNum_, sfBound, freshnessBound);
  }

  res.obj_ = ObjectiveFunctionBase::TrueObj(dagTasks, tasksInfo, initialSTV, scheduleOptions);
  return res;
}
} // namespace OrderOptDAG_SPACE