#pragma once
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

template <typename ObjectiveFunction>
ScheduleResult PerformTOM_OPT_BF(const DAG_Model& dag_tasks) {
  auto start = std::chrono::high_resolution_clock::now();
  ScheduleResult res;
  TaskSetOptEnumerate task_sets_perms(dag_tasks, dag_tasks.chains_,
                                      ObjectiveFunction::type_trait);
  res.obj_ = task_sets_perms.PerformOptimizationBF<ObjectiveFunction>();
  if (res.obj_ >= 1e8) {
    res.obj_ = PerformStandardLETAnalysis<ObjectiveFunction>(dag_tasks).obj_;
  }
  res.schedulable_ = task_sets_perms.ExamSchedulabilityOptSol();
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
  if (res.obj_ >= 1e8) {
    res.obj_ = PerformStandardLETAnalysis<ObjectiveFunction>(dag_tasks).obj_;
  }
  res.schedulable_ = task_sets_perms.ExamSchedulabilityOptSol();
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

  PrintResultAnalysis(task_sets_perms, res);
  return res;
}

template <typename ObjectiveFunction>
ScheduleResult PerformTOM_OPT_SortBound(const DAG_Model& dag_tasks,
                                        const ScheduleResult& res_of_sort) {
  auto start = std::chrono::high_resolution_clock::now();
  ScheduleResult res;
  res.obj_ = GetApproximatedObjBound<ObjectiveFunction>(
      dag_tasks, dag_tasks.chains_, res_of_sort.obj_);
  res.schedulable_ = res_of_sort.schedulable_;
  auto stop = std::chrono::high_resolution_clock::now();
  res.timeTaken_ = GetTimeTaken(start, stop) + res_of_sort.timeTaken_;

  return res;
}

template <typename ObjectiveFunction>
ScheduleResult PerformTOM_OPT_SortBoundImproved(
    const DAG_Model& dag_tasks, const ScheduleResult& res_of_sort) {
  auto start = std::chrono::high_resolution_clock::now();
  ScheduleResult res;
  res.obj_ = GetApproximatedObjBoundImproved<ObjectiveFunction>(
      dag_tasks, dag_tasks.chains_, res_of_sort.obj_,
      res_of_sort.variable_opt_);
  res.schedulable_ = res_of_sort.schedulable_;
  auto stop = std::chrono::high_resolution_clock::now();
  res.timeTaken_ = GetTimeTaken(start, stop) + res_of_sort.timeTaken_;

  return res;
}
}  // namespace DAG_SPACE
