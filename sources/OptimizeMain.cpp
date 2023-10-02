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
  if (GlobalVariablesDAGOpt::debugMode) {
    std::cout << "Best yet variable of Martinez18: \n";
    task_sets_perms.best_yet_variable_od_.print();
  }
  return res;
}

ScheduleResult PerformTOM_OPTOffset_Sort(const DAG_Model& dag_tasks) {
  auto start = std::chrono::high_resolution_clock::now();
  ScheduleResult res;
  TaskSetOptSorted_Offset task_sets_perms(dag_tasks, dag_tasks.chains_);
  res.obj_ = task_sets_perms.PerformOptimizationSort<ObjDataAge>();
  if (res.obj_ >= 1e8) {
    res.obj_ = PerformStandardLETAnalysis<ObjDataAge>(dag_tasks).obj_;
  }
  res.schedulable_ = CheckSchedulability(dag_tasks);
  auto stop = std::chrono::high_resolution_clock::now();
  res.timeTaken_ = GetTimeTaken(start, stop);

  if (GlobalVariablesDAGOpt::debugMode) {
    std::cout << "Best yet variable of Martinez18: \n";
    task_sets_perms.best_yet_variable_od_.print();
  }
  PrintResultAnalysis(task_sets_perms, res);
  return res;
}

ScheduleResult PerformTOM_OPT_BF_SF(const DAG_Model& dag_tasks) {
  auto dags = ExtractDAGsWithIndependentForks(dag_tasks);
  ScheduleResult res;
  res.obj_ = 0;
  for (const auto& dag : dags) {
    TaskSetOptEnumerate task_sets_perms =
        TaskSetOptEnumerate(dag, GetChainsForSF(dag), "SensorFusion");
    res.obj_ += task_sets_perms.PerformOptimizationBF<ObjSensorFusion>().second;
    res.schedulable_ =
        res.schedulable_ && task_sets_perms.ExamSchedulabilityOptSol();
    std::cout << "\n";
  }
  return res;
}

ScheduleResult PerformTOM_OPT_EnumW_Skip_SF(const DAG_Model& dag_tasks) {
  auto dags = ExtractDAGsWithIndependentForks(dag_tasks);
  ScheduleResult res;
  res.obj_ = 0;
  for (const auto& dag : dags) {
    TaskSetOptEnumWSkip task_sets_perms =
        TaskSetOptEnumWSkip(dag, GetChainsForSF(dag), "SensorFusion");
    res.obj_ +=
        task_sets_perms.PerformOptimizationSkipInfeasible<ObjSensorFusion>();
    res.schedulable_ =
        res.schedulable_ && task_sets_perms.ExamSchedulabilityOptSol();
    std::cout << "\n";
  }
  return res;
}
}  // namespace DAG_SPACE
