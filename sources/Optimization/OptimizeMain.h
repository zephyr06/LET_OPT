#pragma once
#include "sources/Optimization/TaskSetOptEnumWSkip.h"
#include "sources/Optimization/TaskSetOptEnumerate.h"
#include "sources/Optimization/TaskSetOptSorted.h"

namespace DAG_SPACE {

void PrintResultAnalysis(const TaskSetPermutation& task_sets_perms,
                         const ScheduleResult& res) {
  std::cout << "The total number of permutation iterations is: "
            << task_sets_perms.iteration_count_ << "\n";
  if (GlobalVariablesDAGOpt::debugMode == 1) {
    std::cout << "The best permutation is: \n";
    task_sets_perms.best_yet_chain_.print();
  }
  if (!res.schedulable_ &&
      res.timeTaken_ < GlobalVariablesDAGOpt::TIME_LIMIT - 5)
    CoutError("Find an unschedulable case!");
}

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

  PrintResultAnalysis(task_sets_perms, res);
  return res;
}

}  // namespace DAG_SPACE
