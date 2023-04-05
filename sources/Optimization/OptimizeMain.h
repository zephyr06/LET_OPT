#pragma once
#include "sources/Optimization/TaskSetOptEnumWSkip.h"
#include "sources/Optimization/TaskSetOptSorted.h"

namespace DAG_SPACE {
template <typename ObjectiveFunction>
ScheduleResult PerformTOM_OPT(const DAG_Model& dag_tasks,
                              std::string method = "Enumerate") {
  auto start = std::chrono::high_resolution_clock::now();
  ScheduleResult res;
  //   TaskSetPermutation task_sets_perms;
  if (method == "Enumerate") {
    TaskSetOptEnumWSkip task_sets_perms =
        TaskSetOptEnumWSkip(dag_tasks, dag_tasks.chains_);
    res.obj_ =
        task_sets_perms.PerformOptimizationEnumerate<ObjectiveFunction>();
    if (res.obj_ >= 1e8) {
      res.obj_ = PerformStandardLETAnalysis<ObjectiveFunction>(dag_tasks).obj_;
    }
    res.schedulable_ = task_sets_perms.ExamSchedulabilityOptSol();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    res.timeTaken_ = double(duration.count()) / 1e6;
    std::cout << "The total number of permutation iterations is: "
              << task_sets_perms.iteration_count_ << "\n";
    if (GlobalVariablesDAGOpt::debugMode == 1) {
      std::cout << "The best permutation is: \n";
      task_sets_perms.best_yet_chain_.print();
    }
    if (!res.schedulable_ &&
        res.timeTaken_ < GlobalVariablesDAGOpt::TIME_LIMIT - 5)
      CoutError("Find an unschedulable case!");
  } else if (method == "Sort") {
    TaskSetOptSorted task_sets_perms =
        TaskSetOptSorted(dag_tasks, dag_tasks.chains_);
    res.obj_ = task_sets_perms.PerformOptimizationSort<ObjectiveFunction>();
    if (res.obj_ >= 1e8) {
      res.obj_ = PerformStandardLETAnalysis<ObjectiveFunction>(dag_tasks).obj_;
    }
    res.schedulable_ = task_sets_perms.ExamSchedulabilityOptSol();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    res.timeTaken_ = double(duration.count()) / 1e6;
    std::cout << "The total number of permutation iterations is: "
              << task_sets_perms.iteration_count_ << "\n";
    if (GlobalVariablesDAGOpt::debugMode == 1) {
      std::cout << "The best permutation is: \n";
      task_sets_perms.best_yet_chain_.print();
    }
    if (!res.schedulable_ &&
        res.timeTaken_ < GlobalVariablesDAGOpt::TIME_LIMIT - 5)
      CoutError("Find an unschedulable case!");
  } else
    CoutError("Unrecognized method in PerformTOM_OPT");

  return res;
}

}  // namespace DAG_SPACE
