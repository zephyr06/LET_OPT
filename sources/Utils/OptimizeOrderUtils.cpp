#include "OptimizeOrderUtils.h"

namespace OrderOptDAG_SPACE {

void PrintResultAnalyzation(const ScheduleResult &scheduleResult, const DAG_Model &dagTasks) {
  const TaskSet &tasks = dagTasks.tasks;
  RegularTaskSystem::TaskSetInfoDerived tasksInfo(tasks);
  std::cout << Color::blue;
  std::cout << "Schedulable after optimization? " << scheduleResult.schedulable_ << std::endl;
  std::cout << "Cause effect chains:" << std::endl;
  PrintChains(dagTasks.chains_);
  RTDA resAfterOpt = GetMaxRTDA(tasksInfo, dagTasks.chains_[0], scheduleResult.startTimeVector_);
  resAfterOpt.print();

  VectorDynamic sfError =
      OrderOptDAG_SPACE::ObtainSensorFusionError(dagTasks, tasksInfo, scheduleResult.startTimeVector_);
  std::cout << "SF Bound from schedule: " << sfError.maxCoeff() << std::endl;
  std::cout << "Max allowed SF Bound: " << GlobalVariablesDAGOpt::sensorFusionTolerance << std::endl;

  std::cout << Color::def << std::endl;
  std::cout << "Schedule: " << std::endl;
  if (GlobalVariablesDAGOpt::printSchedule) {
    std::cout << scheduleResult.startTimeVector_ << std::endl;
    PrintSchedule(tasksInfo, scheduleResult.startTimeVector_);
  }
}

bool CheckDDLConstraint(const DAG_Model &dagTasks, const RegularTaskSystem::TaskSetInfoDerived &tasksInfo,
                        const VectorDynamic &startTimeVector) {
  for (int i = 0; i < tasksInfo.N; i++) {
    for (int j = 0; j < int(tasksInfo.sizeOfVariables[i]); j++) {
      JobCEC job(i, j);
      double start = GetStartTime(job, startTimeVector, tasksInfo);
      if (start < GetActivationTime(job, tasksInfo) ||
          start + GetExecutionTime(job, tasksInfo) > GetDeadline(job, tasksInfo))
        return false;
    }
  }
  return true;
}

std::vector<std::vector<Interval>>
ExtractJobsPerProcessor(const DAG_Model &dagTasks, const RegularTaskSystem::TaskSetInfoDerived &tasksInfo,
                        const VectorDynamic &startTimeVector, const std::vector<uint> &processorJobVec,
                        int processorNum) {

  std::vector<std::vector<Interval>> jobsPerProcessor(processorNum);
  if (processorNum <= 0)
    return jobsPerProcessor;
  int index = 0;
  for (uint i = 0; i < dagTasks.tasks.size(); i++) {
    for (uint j = 0; j < tasksInfo.sizeOfVariables[i]; j++) {
      JobCEC job(i, j);
      Interval v(GetStartTime(job, startTimeVector, tasksInfo), tasksInfo.tasks[i].executionTime);
      if (processorJobVec[index] >= jobsPerProcessor.size()) {
        if (GlobalVariablesDAGOpt::debugMode)
          CoutWarning("Wrong processorNum in ExtractJobsPerProcessor!");
        // jobsPerProcessor.resize(processorJobVec[index] + 1);
        while (jobsPerProcessor.size() < processorJobVec[index] + 1) {
          std::vector<Interval> ttt;
          jobsPerProcessor.push_back(ttt);
        }
        jobsPerProcessor[processorJobVec[index]].push_back(v);
      } else
        jobsPerProcessor[processorJobVec[index]].push_back(v);
      index++;
    }
  }
  return jobsPerProcessor;
}

double DBF_Error(const DAG_Model &dagTasks, const RegularTaskSystem::TaskSetInfoDerived &tasksInfo,
                 const VectorDynamic &startTimeVector, const std::vector<uint> &processorJobVec,
                 int processorNum) {
  if (processorNum <= 0)
    return 0;
  if (processorJobVec.size() == 0)
    CoutError("Empty processorJobVecin DBF_Error!");
  std::vector<std::vector<Interval>> jobsPerProcessor =
      ExtractJobsPerProcessor(dagTasks, tasksInfo, startTimeVector, processorJobVec, processorNum);
  double overallError = 0;
  for (int i = 0; i < processorNum; i++) {
    overallError += IntervalOverlapError(jobsPerProcessor[i]);
  }
  return overallError;
}

// Exams DBF constraints
bool ExamDBF_Feasibility(const DAG_Model &dagTasks, const RegularTaskSystem::TaskSetInfoDerived &tasksInfo,
                         const VectorDynamic &startTimeVector, const std::vector<uint> &processorJobVec,
                         int processorNum) {
  if (processorNum <= 0)
    return false;
  std::vector<std::vector<Interval>> jobsPerProcessor =
      ExtractJobsPerProcessor(dagTasks, tasksInfo, startTimeVector, processorJobVec, processorNum);
  if (static_cast<int>(jobsPerProcessor.size()) > processorNum)
    return false;
  for (int i = 0; i < processorNum; i++) {
    // to avoid rounding errors, use 1e-6 as threshold
    if (IntervalOverlapError(jobsPerProcessor[i]) > 1e-6)
      return false;
  }
  return true;
}

bool ExamBasic_Feasibility(const DAG_Model &dagTasks, const RegularTaskSystem::TaskSetInfoDerived &tasksInfo,
                           const VectorDynamic &startTimeVector, const std::vector<uint> &processorJobVec,
                           int processorNum) {
  // BeginTimer("ExamBasic_Feasibility");
  if (processorJobVec.empty())
    CoutError("Empty processorJobVec in ExamBasicFeasibility!");
  bool schedulable = ExamDDL_Feasibility(dagTasks, tasksInfo, startTimeVector) &&
                     ExamDBF_Feasibility(dagTasks, tasksInfo, startTimeVector, processorJobVec, processorNum);
  // EndTimer("ExamBasic_Feasibility");
  return schedulable;
}

bool ExamAll_Feasibility(const DAG_Model &dagTasks, const RegularTaskSystem::TaskSetInfoDerived &tasksInfo,
                         const VectorDynamic &startTimeVector, const std::vector<uint> &processorJobVec,
                         int processorNum, double sfBound, double freshnessBound) {
  if (!ExamBasic_Feasibility(dagTasks, tasksInfo, startTimeVector, processorJobVec, processorNum))
    return false;

  //   if (GlobalVariablesDAGOpt::considerSensorFusion) {
  // Exam RTDA
  for (auto chain : dagTasks.chains_) {
    std::vector<OrderOptDAG_SPACE::RTDA> rtdaVec =
        OrderOptDAG_SPACE::GetRTDAFromSingleJob(tasksInfo, chain, startTimeVector);
    OrderOptDAG_SPACE::RTDA rtda = GetMaxRTDA(rtdaVec);
    // if (GlobalVariablesDAGOpt::debugMode == 1) {
    //   std::cout << "ExamAll_Feasibility: " << std::endl;
    //   rtda.print();
    // }
    if (rtda.dataAge > freshnessBound || rtda.reactionTime > freshnessBound)
      return false;
  }

  // Exam SF
  VectorDynamic sfError = OrderOptDAG_SPACE::ObtainSensorFusionError(dagTasks, tasksInfo, startTimeVector);
  // if (GlobalVariablesDAGOpt::debugMode == 1) {
  //   std::cout << "SF Bound from schedule: " << sfError.maxCoeff() << std::endl;
  //   std::cout << "Max allowed SF Bound: " << sfBound << std::endl;
  // }
  if (sfError.maxCoeff() > sfBound)
    return false;
  //   }
  return true;
}

// template <typename ObjectiveFunctionBase>
// ScheduleResult ScheduleDAGLS_LFT(const DAG_Model &dagTasks,
//                                  const OptimizeSF::ScheduleOptions &scheduleOptions, double sfBound,
//                                  double freshnessBound) {
//   TaskSet &tasks = dagTasks.tasks;
//   RegularTaskSystem::TaskSetInfoDerived tasksInfo(tasks);
//   std::vector<uint> processorJobVec;
//   VectorDynamic initialSTV =
//       ListSchedulingLFTPA(dagTasks, tasksInfo, scheduleOptions.processorNum_, processorJobVec);

//   ScheduleResult res;
//   res.startTimeVector_ = initialSTV;
//   if (ObjectiveFunctionBase::type_trait == "RTDAExperimentObj") {
//     res.schedulable_ = ExamBasic_Feasibility(dagTasks, tasksInfo, initialSTV, processorJobVec,
//                                              scheduleOptions.processorNum_);
//   } else if (ObjectiveFunctionBase::type_trait == "RTSS21ICObj") {
//     res.schedulable_ = ExamAll_Feasibility(dagTasks, tasksInfo, initialSTV, processorJobVec,
//                                            scheduleOptions.processorNum_, sfBound, freshnessBound);
//   }

//   res.obj_ = ObjectiveFunctionBase::TrueObj(dagTasks, tasksInfo, initialSTV, scheduleOptions);
//   return res;
// }
} // namespace OrderOptDAG_SPACE