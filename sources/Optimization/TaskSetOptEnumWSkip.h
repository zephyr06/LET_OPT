#pragma once
#include "sources/Optimization/TaskSetOptEnumerate.h"
namespace DAG_SPACE {

class TaskSetOptEnumWSkip : public TaskSetOptEnumerate {
 public:
  TaskSetOptEnumWSkip(const DAG_Model& dag_tasks,
                      const std::vector<std::vector<int>>& chains,
                      const std::string& type_trait)
      : TaskSetOptEnumerate(dag_tasks, chains, type_trait) {}

  template <typename ObjectiveFunction>
  ScheduleResult PerformOptimizationSkipInfeasible() {
    InitializeSolutions<ObjectiveFunction>(DefaultLET);
    InitializeSolutions<ObjectiveFunction>(Maia23);
    // if (IfSF_Trait(ObjectiveFunction::type_trait)) {
    //   ReOrderTwoTaskPermutations();
    // }
    ChainsPermutation chains_perm;
    IterateAllPermsWSkip<ObjectiveFunction>(0, chains_perm);
    return GetScheduleResult<ObjectiveFunction>();
  }

  // depth equals the number of edge pais
  template <typename ObjectiveFunction>
  void IterateAllPermsWSkip(uint position, ChainsPermutation& chains_perm) {
    if (position == graph_of_all_ca_chains_.edge_records_
                        .size()) {  // finish iterate all the pair permutations
      iteration_count_++;
      EvaluateChainsPermutation<ObjectiveFunction>(chains_perm);
      return;
    }
    if (found_optimal_) return;

    VariableRange variable_range_w_chains =
        FindPossibleVariableOD(dag_tasks_, tasks_info_, rta_, chains_perm,
                               false);  // optimize_offset_only_=false
    PermIneqBound_Range perm_ineq_bound_range = GetEdgeIneqRange(
        adjacent_two_task_permutations_[position].GetEdge(),
        variable_range_w_chains, ObjectiveFunction::type_trait);
    TwoTaskPermutationsIterator iterator(
        adjacent_two_task_permutations_[position], perm_ineq_bound_range);

    while (!iterator.empty()) {
      if (ifTimeout(start_time_)) break;
      const auto& perm_sing_curr = iterator.pop_front();

      if (chains_perm.IsValid(variable_range_w_chains, *perm_sing_curr,
                              graph_of_all_ca_chains_, rta_)) {
        chains_perm.push_back(perm_sing_curr);

        IterateAllPermsWSkip<ObjectiveFunction>(position + 1, chains_perm);

        chains_perm.pop(perm_sing_curr);
      }
    }
  }
};

}  // namespace DAG_SPACE