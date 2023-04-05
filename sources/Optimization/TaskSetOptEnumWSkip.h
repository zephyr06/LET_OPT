#pragma once
#include "sources/Optimization/TaskSetPermutation.h"
namespace DAG_SPACE {

class TaskSetOptEnumWSkip : public TaskSetPermutation {
 public:
  TaskSetOptEnumWSkip(const DAG_Model& dag_tasks,
                      const std::vector<std::vector<int>>& chains)
      : TaskSetPermutation(dag_tasks, chains) {}

  template <typename ObjectiveFunction>
  int PerformOptimizationEnumerate() {
    ChainsPermutation chains_perm;
    IterateAllChainsPermutations<ObjectiveFunction>(0, chains_perm);
    return best_yet_obj_;
  }

  // chains_perm already pushed the new perm_single
  template <typename ObjectiveFunction>
  bool WhetherSkipToNextPerm(const ChainsPermutation& chains_perm) {
    std::vector<std::vector<int>> sub_chains =
        GetSubChains(dag_tasks_.chains_, chains_perm);
    if (WhetherContainInfeasibleSubChains(chains_perm, sub_chains)) return true;

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
  void IterateAllChainsPermutations(uint position,
                                    ChainsPermutation& chains_perm) {
    if (position == graph_of_all_ca_chains_.edge_records_
                        .size()) {  // finish iterate all the pair permutations
      iteration_count_++;
      EvaluateChainsPermutation<ObjectiveFunction>(chains_perm);
      return;
    }

    for (uint i = 0; i < adjacent_two_task_permutations_[position].size();
         i++) {
      if (ifTimeout(start_time_)) break;
      if (chains_perm.IsValid(variable_range_od_,
                              *adjacent_two_task_permutations_[position][i],
                              graph_of_all_ca_chains_)) {
        chains_perm.push_back(adjacent_two_task_permutations_[position][i]);

        // try to skip some permutations
        if (!WhetherSkipToNextPerm<ObjectiveFunction>(chains_perm)) {
          IterateAllChainsPermutations<ObjectiveFunction>(position + 1,
                                                          chains_perm);
        }
        chains_perm.pop(*adjacent_two_task_permutations_[position][i]);
      }
    }
  }

  template <typename ObjectiveFunction>
  double EvaluateChainsPermutation(const ChainsPermutation& chains_perm) {
    std::pair<VariableOD, int> res = FindODWithLP(
        dag_tasks_, tasks_info_, chains_perm, graph_of_all_ca_chains_,
        ObjectiveFunction::type_trait, rta_);

    if (res.first.valid_)  // if valid, we'll exam obj; otherwise, we'll
                           // just move forward
    {
      if (res.second < best_yet_obj_) {
        best_yet_obj_ = res.second;
        best_yet_chain_ = chains_perm;
        best_yet_variable_od_ = res.first;
      }
    } else {
      infeasible_iteration_++;
    }
    return res.second;
  }
};

}  // namespace DAG_SPACE