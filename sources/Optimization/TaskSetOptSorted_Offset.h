
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

  template <typename ObjectiveFunction>
  int PerformOptimizationSort() {
    ChainsPermutation chains_perm;
    IterateSortedPerms<ObjectiveFunction>(0, chains_perm);
    std::cout << "The number of feasibile chains found: "
              << feasible_chains_.size() << "\n";
    std::cout << "Decrease succes: " << decrease_success
              << ", Decrease Fail: " << decrease_fail << std::endl;
    PrintFeasibleChainsRecord();
    return best_yet_obj_;
  }

  template <typename ObjectiveFunction>
  void IterateSortedPerms(uint position, ChainsPermutation& chains_perm) {
    if (position == graph_of_all_ca_chains_.edge_records_
                        .size()) {  // finish iterate all the pair permutations
      iteration_count_++;
      EvaluateChainsPermutation<ObjectiveFunction>(chains_perm);
      return;
    }

    VariableRange variable_range_w_chains = FindPossibleVariableOD(
        dag_tasks_, tasks_info_, rta_, chains_perm, true);
#ifdef PROFILE_CODE
    BeginTimer("iterator_constructor");
#endif
    TwoTaskPermutationsIterator iterator(
        adjacent_two_task_permutations_[position]);
#ifdef PROFILE_CODE
    EndTimer("iterator_constructor");
#endif

    int count = iterator.size();
    std::vector<Edge> unvisited_future_edges =
        GetUnvisitedFutureEdges(position);
    while (!iterator.empty()) {
      if (ifTimeout(start_time_)) break;
#ifdef PROFILE_CODE
      BeginTimer("RemoveCandidates_related");
#endif
      iterator.RemoveCandidates(chains_perm, feasible_chains_.chain_man_vec_,
                                unvisited_future_edges);
      iterator.RemoveCandidates(chains_perm,
                                feasible_chains_.chain_man_vec_incomplete_,
                                unvisited_future_edges);
#ifdef PROFILE_CODE
      EndTimer("RemoveCandidates_related");
#endif
      if (iterator.empty()) break;

      const auto& perm_sing_curr = iterator.pop_front();
      // if (chains_perm.IsValid(variable_range_w_chains, *perm_sing_curr,
      //                         graph_of_all_ca_chains_, rta_)) {
      if (true) {
        chains_perm.push_back(perm_sing_curr);

        if (!WhetherSkipToNextPerm<ObjectiveFunction>(chains_perm)) {
          IterateSortedPerms<ObjectiveFunction>(position + 1, chains_perm);
        }
        chains_perm.pop(perm_sing_curr);
      } else {
      }

      count--;
      if (count < -10) {
        CoutWarning("deadlock found during IterateSortedPerms");
      }
    }
  }

  // chains_perm already pushed the new perm_single
  template <typename ObjectiveFunction>
  bool WhetherSkipToNextPerm(const ChainsPermutation& chains_perm) {
#ifdef PROFILE_CODE
    BeginTimer(__FUNCTION__);
#endif
    std::vector<std::vector<int>> sub_chains =
        GetSubChains(dag_tasks_.chains_, chains_perm);

    int best_possible_obj_incomplete =
        GetBestPossibleObj_UpperBound<ObjectiveFunction>(chains_perm,
                                                         sub_chains);
    if (best_possible_obj_incomplete > best_yet_obj_) {
      if (GlobalVariablesDAGOpt::debugMode) {
        std::cout << "Early break at level " << chains_perm.size() << ": ";
        PrintSinglePermIndex(chains_perm);
        std::cout << " due to guarantee to perform worse at the "
                     "per-chain test\n";
      }

      feasible_chains_.push_back_incomplete(
          FeasibleChainManager(chains_perm, adjacent_two_task_permutations_,
                               ObjectiveFunction::type_trait));
#ifdef PROFILE_CODE
      EndTimer(__FUNCTION__);
#endif
      return true;
    }
#ifdef PROFILE_CODE
    EndTimer(__FUNCTION__);
#endif
    return false;
  }
};

}  // namespace DAG_SPACE