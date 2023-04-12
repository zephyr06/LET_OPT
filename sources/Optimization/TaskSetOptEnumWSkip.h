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
  int PerformOptimizationSkipInfeasible() {
    ChainsPermutation chains_perm;
    IterateAllPermsWSkip<ObjectiveFunction>(0, chains_perm);
    return best_yet_obj_;
  }

  // chains_perm already pushed the new perm_single
  template <typename ObjectiveFunction>
  bool WhetherSkipToNextPerm(const ChainsPermutation& chains_perm) {
    std::vector<std::vector<int>> sub_chains =
        GetSubChains(dag_tasks_.chains_, chains_perm);

    if (GetBestPossibleObj<ObjectiveFunction>(chains_perm, sub_chains) >
        best_yet_obj_) {
      if (GlobalVariablesDAGOpt::debugMode) {
        std::cout << "Early break at level " << chains_perm.size() << ": ";
        PrintSinglePermIndex(chains_perm);
        std::cout << " due to guarantee to perform worse at the "
                     "per-chain test\n";
      }
      return true;
    }
    return false;
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

    VariableRange variable_range_w_chains =
        FindPossibleVariableOD(dag_tasks_, tasks_info_, rta_, chains_perm);
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

        // try to skip some permutations
        if (!WhetherSkipToNextPerm<ObjectiveFunction>(chains_perm)) {
          IterateAllPermsWSkip<ObjectiveFunction>(position + 1, chains_perm);
        }
        chains_perm.pop(perm_sing_curr);
      }
    }
  }
};

}  // namespace DAG_SPACE