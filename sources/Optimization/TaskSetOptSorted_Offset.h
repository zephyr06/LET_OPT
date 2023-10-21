
#pragma once
#include "sources/Optimization/TaskSetOptSorted.h"
// This class only optimizes offset. The system model is the same as
// Martinez18TCAD
namespace DAG_SPACE {

inline VariableRange FindVariableRangeMartModel(const DAG_Model& dag_tasks) {
  VariableRange variable_range;
  variable_range.lower_bound = VariableOD(dag_tasks.GetTaskSet());
  variable_range.upper_bound = variable_range.lower_bound;
  for (uint i = 0; i < variable_range.lower_bound.size(); i++) {
    variable_range.upper_bound[i].offset += dag_tasks.GetTask(i).period - 1;
    variable_range.upper_bound[i].deadline += dag_tasks.GetTask(i).period - 1;
  }
  return variable_range;
}

PermutationInequality GeneratePermIneqOnlyOffset(
    int task_prev_id, int task_next_id, const std::string& type_trait,
    const VariableRange& variable_od_range,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info);

class TwoTaskPermutations_OnlyOffset : public TwoTaskPermutations {
 public:
  TwoTaskPermutations_OnlyOffset(
      int task_prev_id, int task_next_id, const DAG_Model& dag_tasks,
      const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
      const std::vector<int>& rta, const std::string& type_trait,
      int perm_count_global = 0)
      : TwoTaskPermutations(task_prev_id, task_next_id, dag_tasks, tasks_info,
                            rta, perm_count_global) {
    type_trait_ = "DataAge";
    variable_od_range_ = FindVariableRangeMartModel(dag_tasks);
    FindAllPermutations();
  }

  void AppendAllPermutations(const JobCEC& job_curr,
                             SinglePairPermutation& permutation_current);

  void FindAllPermutations();

  // data members
  // VariableRange variable_range_od_;
};

// ************************************************************************************

class TaskSetOptSorted_Offset : public TaskSetOptSorted {
 public:
  TaskSetOptSorted_Offset(const DAG_Model& dag_tasks,
                          const std::vector<std::vector<int>>& chains)
      // : TaskSetOptSorted(dag_tasks, {chain}, "DataAge") {
      : TaskSetOptSorted(dag_tasks, chains) {
    optimize_offset_only_ = true;
    variable_range_od_ = FindVariableRangeMartModel(dag_tasks_);
    FindPairPermutations();
  }

  void FindPairPermutations();

  // template <typename ObjectiveFunction>
  // ScheduleResult PerformOptimizationSort() {
  //   InitializeSolutions<ObjectiveFunction>(DefaultLET);
  //   ChainsPermutation chains_perm;
  //   IterateSortedPerms<ObjectiveFunction>(0, chains_perm);
  //   std::cout << "The number of feasibile chains found: "
  //             << feasible_chains_.size() << "\n";
  //   std::cout << "Decrease succes: " << decrease_success
  //             << ", Decrease Fail: " << decrease_fail << std::endl;
  //   PrintFeasibleChainsRecord();
  //   // return best_yet_obj_;
  //   return GetScheduleResult<ObjectiveFunction>();
  // }
};

}  // namespace DAG_SPACE