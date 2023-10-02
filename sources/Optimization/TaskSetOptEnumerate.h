#pragma once
#include "sources/Optimization/TaskSetPermutation.h"
namespace DAG_SPACE {

class TaskSetOptEnumerate : public TaskSetPermutation {
 public:
  TaskSetOptEnumerate(const DAG_Model& dag_tasks,
                      const std::vector<std::vector<int>>& chains,
                      const std::string& type_trait)
      : TaskSetPermutation(dag_tasks, chains, type_trait) {}

  template <typename ObjectiveFunction>
  std::pair<VariableOD, int> PerformOptimizationBF() {
    ChainsPermutation chains_perm;
    IterateAllPermsBF<ObjectiveFunction>(0, chains_perm);
    return std::make_pair(best_yet_variable_od_, best_yet_obj_);
  }

  // depth equals the number of edge pais
  template <typename ObjectiveFunction>
  void IterateAllPermsBF(uint position, ChainsPermutation& chains_perm) {
    if (position == graph_of_all_ca_chains_.edge_records_
                        .size()) {  // finish iterate all the pair permutations
      iteration_count_++;
      EvaluateChainsPermutation<ObjectiveFunction>(chains_perm);
      return;
    }
    if (found_optimal_) return;

    TwoTaskPermutationsIterator iterator(
        adjacent_two_task_permutations_[position]);

    while (!iterator.empty()) {
      if (ifTimeout(start_time_)) break;
      const auto& perm_sing_curr = iterator.pop_front();
      chains_perm.push_back(perm_sing_curr);
      IterateAllPermsBF<ObjectiveFunction>(position + 1, chains_perm);
      chains_perm.pop(perm_sing_curr);
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
        if (best_yet_obj_ == 0) found_optimal_ = true;
      }
    } else {
      infeasible_iteration_++;
    }
    return res.second;
  }
};

}  // namespace DAG_SPACE