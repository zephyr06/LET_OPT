#pragma once
#include "sources/Baseline/Bradatsch16.h"
#include "sources/Baseline/ImplicitCommunication/ScheduleSimulation.h"
#include "sources/Baseline/Martinez18.h"
#include "sources/Baseline/OptSortedBound.h"
#include "sources/Optimization/TaskSetOptEnumWSkip.h"
#include "sources/Optimization/TaskSetOptEnumerate.h"
#include "sources/Optimization/TaskSetOptSorted.h"
#include "sources/Optimization/TaskSetOptSorted_Offset.h"

namespace DAG_SPACE {

void PrintResultAnalysis(const TaskSetPermutation& task_sets_perms,
                         const ScheduleResult& res);

ScheduleResult PerformOPT_Martinez18_DA(const DAG_Model& dag_tasks);

ScheduleResult PerformTOM_OPTOffset_Sort(const DAG_Model& dag_tasks);

template <typename ObjectiveFunctionBase>
ScheduleResult PerformImplicitCommuAnalysis(const DAG_Model& dag_tasks) {
  auto start = std::chrono::high_resolution_clock::now();

  ScheduleResult res;
  const TaskSet& tasks = dag_tasks.GetTaskSet();
  TaskSetInfoDerived tasks_info(tasks);
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
  ChainsPermutation chains_perm =
      GetChainsPermFromVariable(dag_tasks, tasks_info, dag_tasks.chains_,
                                ObjectiveFunctionBase::type_trait, schedule);
  //   chains_perm.print();
  res.obj_ = ObjectiveFunctionBase::Obj(dag_tasks, tasks_info, chains_perm,
                                        schedule, dag_tasks.chains_);
  res.schedulable_ = CheckSchedulability(dag_tasks);

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  res.timeTaken_ = double(duration.count()) / 1e6;
  return res;
}

template <typename ObjectiveFunction>
ScheduleResult PerformTOM_OPT_BF(const DAG_Model& dag_tasks) {
  auto start = std::chrono::high_resolution_clock::now();
  ScheduleResult res;
  TaskSetOptEnumerate task_sets_perms(dag_tasks, dag_tasks.chains_,
                                      ObjectiveFunction::type_trait);
  res.obj_ = task_sets_perms.PerformOptimizationBF<ObjectiveFunction>();
  res.schedulable_ = task_sets_perms.ExamSchedulabilityOptSol();
  if (res.obj_ >= 1e8) {
    res.obj_ = PerformStandardLETAnalysis<ObjectiveFunction>(dag_tasks).obj_;
  }
  auto stop = std::chrono::high_resolution_clock::now();
  res.timeTaken_ = GetTimeTaken(start, stop);

  PrintResultAnalysis(task_sets_perms, res);
  return res;
}

template <typename ObjectiveFunction>
ScheduleResult PerformTOM_OPT_EnumW_Skip(const DAG_Model& dag_tasks) {
  auto start = std::chrono::high_resolution_clock::now();
  ScheduleResult res;
  TaskSetOptEnumWSkip task_sets_perms = TaskSetOptEnumWSkip(
      dag_tasks, dag_tasks.chains_, ObjectiveFunction::type_trait);
  res.obj_ =
      task_sets_perms.PerformOptimizationSkipInfeasible<ObjectiveFunction>();
  res.schedulable_ = task_sets_perms.ExamSchedulabilityOptSol();
  if (res.obj_ >= 1e8) {
    res.obj_ = PerformStandardLETAnalysis<ObjectiveFunction>(dag_tasks).obj_;
  }
  auto stop = std::chrono::high_resolution_clock::now();
  res.timeTaken_ = GetTimeTaken(start, stop);

  PrintResultAnalysis(task_sets_perms, res);
  return res;
}

template <typename ObjectiveFunction>
ScheduleResult PerformTOM_OPT_Sort(const DAG_Model& dag_tasks) {
  auto start = std::chrono::high_resolution_clock::now();
  ScheduleResult res;
  TaskSetOptSorted task_sets_perms = TaskSetOptSorted(
      dag_tasks, dag_tasks.chains_, ObjectiveFunction::type_trait);
  res.obj_ = task_sets_perms.PerformOptimizationSort<ObjectiveFunction>();
  if (res.obj_ >= 1e8) {
    res.obj_ = PerformStandardLETAnalysis<ObjectiveFunction>(dag_tasks).obj_;
  }
  res.schedulable_ = task_sets_perms.ExamSchedulabilityOptSol();
  auto stop = std::chrono::high_resolution_clock::now();
  res.timeTaken_ = GetTimeTaken(start, stop);
  res.variable_opt_ = task_sets_perms.best_yet_variable_od_;
  res.obj_per_chain_ = task_sets_perms.GetOptObjPerChain<ObjectiveFunction>();
  PrintResultAnalysis(task_sets_perms, res);
  return res;
}

template <typename ObjectiveFunction>
ScheduleResult PerformTOM_OPT_SortBound(const DAG_Model& dag_tasks) {
  auto start = std::chrono::high_resolution_clock::now();
  ScheduleResult res_of_sort;

  if (ObjectiveFunction::type_trait == "ReactionTime")
    res_of_sort = PerformTOM_OPT_Sort<ObjReactionTimeApprox>(dag_tasks);
  else if (ObjectiveFunction::type_trait == "DataAge")
    res_of_sort = PerformTOM_OPT_Sort<ObjDataAgeApprox>(dag_tasks);
  else
    CoutError("TypeTrait not recognized in PerformTOM_OPT_SortBound");

  res_of_sort.obj_ = GetApproximatedObjBound<ObjectiveFunction>(
      dag_tasks, dag_tasks.chains_, res_of_sort.obj_per_chain_);
  auto stop = std::chrono::high_resolution_clock::now();
  res_of_sort.timeTaken_ = GetTimeTaken(start, stop);

  return res_of_sort;
}

}  // namespace DAG_SPACE
