#include "sources/OptimizeMain.h"

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

ScheduleResult PerformOPT_Martinez18_DA(const DAG_Model& dag_tasks) {
  auto start = std::chrono::high_resolution_clock::now();
  ScheduleResult res;
  Martinez18TaskSetPerms task_sets_perms(dag_tasks, dag_tasks.chains_[0]);
  res.obj_ = task_sets_perms.PerformOptimization();
  if (res.obj_ >= 1e8) {
    res.obj_ = PerformStandardLETAnalysis<ObjDataAge>(dag_tasks).obj_;
  }
  res.schedulable_ = task_sets_perms.ExamSchedulabilityOptSol();
  auto stop = std::chrono::high_resolution_clock::now();
  res.timeTaken_ = GetTimeTaken(start, stop);

  return res;
}

}  // namespace DAG_SPACE
