#pragma once
#include "sources/Optimization/TaskSetPermutation.h"
namespace DAG_SPACE {

class TaskSetOptEnumWSkip : public TaskSetPermutation {
 public:
  TaskSetOptEnumWSkip(const DAG_Model& dag_tasks,
                      const std::vector<std::vector<int>>& chains,
                      const std::string& type_trait)
      : TaskSetPermutation(dag_tasks, chains, type_trait) {}

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
          IterateAllChainsPermutations<ObjectiveFunction>(position + 1,
                                                          chains_perm);
        }
        chains_perm.pop(perm_sing_curr);
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