
#pragma once
#include "sources/Optimization/TaskSetOptSorted.h"
// This class only optimizes offset. The system model is the same as
// Martinez18TCAD
namespace DAG_SPACE {

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
    FindAllPermutations();
  }

  void FindAllPermutations();

  // data members
  VariableRange variable_range_od_;
};

// ************************************************************************************
class TaskSetOptSorted_Offset : public TaskSetOptSorted {
 public:
  TaskSetOptSorted_Offset(const DAG_Model& dag_tasks,
                          const std::vector<int>& chain)
      : TaskSetOptSorted(dag_tasks, chain) {
    optimize_offset_only_ = true;
    FindPairPermutations();
  }

  void FindPairPermutations();
};

}  // namespace DAG_SPACE