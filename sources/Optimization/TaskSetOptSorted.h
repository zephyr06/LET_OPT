#pragma once
#include "sources/Optimization/TaskSetPermutation.h"
namespace DAG_SPACE {

class TaskSetOptSorted : public TaskSetPermutation {
 public:
  TaskSetOptSorted(const DAG_Model& dag_tasks,
                   const std::vector<std::vector<int>>& chains)
      : TaskSetPermutation(dag_tasks, chains) {
    feasible_chains_ =
        FeasiblieChainsManagerVec(adjacent_two_task_permutations_.size());
  }

  void PrintFeasibleChainsRecord() const;

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
  bool IterateSortedPerms(uint position, ChainsPermutation& chains_perm) {
    if (position == graph_of_all_ca_chains_.edge_records_
                        .size()) {  // finish iterate all the pair permutations
      iteration_count_++;
      return EvaluateChainsPermutation<ObjectiveFunction>(chains_perm) !=
             INFEASIBLE_OBJ;
    }

    std::vector<Edge> unvisited_future_edges =
        GetUnvisitedFutureEdges(position);
    PermIneqBound_Range perm_ineq_bound_range = GetEdgeIneqRange(
        adjacent_two_task_permutations_[position].GetEdge(),
        FindPossibleVariableOD(dag_tasks_, tasks_info_, rta_, chains_perm));
    TwoTaskPermutationsIterator iterator(
        adjacent_two_task_permutations_[position], perm_ineq_bound_range);
    bool feasible_prev_chain = false;

    int count = iterator.size();
    while (!iterator.empty()) {
      if (ifTimeout(start_time_)) break;
      BeginTimer("RemoveCandidates_related");
      uint size_before = iterator.size();
      iterator.RemoveCandidates(chains_perm, feasible_chains_.chain_man_vec_,
                                unvisited_future_edges);
      iterator.RemoveCandidates(chains_perm,
                                feasible_chains_.chain_man_vec_incomplete_,
                                unvisited_future_edges);
      uint size_after = iterator.size();
      if (size_after < size_before)
        decrease_success++;
      else
        decrease_fail++;
      EndTimer("RemoveCandidates_related");
      if (iterator.empty()) break;

      const auto& perm_sing_curr = iterator.pop_front();
      if (chains_perm.IsValid(variable_range_od_, *perm_sing_curr,
                              graph_of_all_ca_chains_)) {
        chains_perm.push_back(perm_sing_curr);

        if (!WhetherSkipToNextPerm<ObjectiveFunction>(chains_perm)) {
          IterateSortedPerms<ObjectiveFunction>(position + 1, chains_perm);
        }
        chains_perm.pop(*perm_sing_curr);
      } else {
        if (GlobalVariablesDAGOpt::debugMode) {
          std::cout << "Early break at level " << position << ": ";
          PrintSinglePermIndex(chains_perm);
          std::cout << " due to being conflicted permutations while "
                       "exploring the "
                    << adjacent_two_task_permutations_[position].size() -
                           iterator.size()
                    << " permutations\n";
          std::cout << "\n";
        }
      }

      count--;
      if (count < -10) {
        CoutWarning("deadlock found during IterateSortedPerms");
      }
    }
    return feasible_prev_chain;
  }

  // chains_perm already pushed the new perm_single
  template <typename ObjectiveFunction>
  bool WhetherSkipToNextPerm(const ChainsPermutation& chains_perm) {
#ifdef PROFILE_CODE
    BeginTimer(__FUNCTION__);
#endif
    std::vector<std::vector<int>> sub_chains =
        GetSubChains(dag_tasks_.chains_, chains_perm);

    if (WhetherContainInfeasibleSubChains(chains_perm, sub_chains)) {
#ifdef PROFILE_CODE
      EndTimer(__FUNCTION__);
#endif
      return true;
    }

    if (GetBestPossibleObj<ObjectiveFunction>(chains_perm, sub_chains) >
        best_yet_obj_) {
      if (GlobalVariablesDAGOpt::debugMode) {
        std::cout << "Early break at level " << chains_perm.size() << ": ";
        PrintSinglePermIndex(chains_perm);
        std::cout << " due to guarantee to perform worse at the "
                     "per-chain test\n";
      }
      // TODO: push to feasible_chains_vec, probbaly use a separate queue
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

  template <typename ObjectiveFunction>
  double EvaluateChainsPermutation(const ChainsPermutation& chains_perm) {
#ifdef PROFILE_CODE
    BeginTimer(__FUNCTION__);
#endif

    std::pair<VariableOD, int> res = FindODWithLP(
        dag_tasks_, tasks_info_, chains_perm, graph_of_all_ca_chains_,
        ObjectiveFunction::type_trait, rta_);

    if (res.first.valid_)  // if valid, we'll exam obj; otherwise, we'll
                           // just move forward
    {
      feasible_chains_.push_back(
          FeasibleChainManager(chains_perm, adjacent_two_task_permutations_,
                               ObjectiveFunction::type_trait));

      if (res.second < best_yet_obj_) {
        best_yet_obj_ = res.second;
        best_yet_chain_ = chains_perm;
        best_yet_variable_od_ = res.first;
      }
    } else {
      infeasible_iteration_++;
    }

#ifdef PROFILE_CODE
    EndTimer(__FUNCTION__);
#endif
    return res.second;
  }

  // data members
  FeasiblieChainsManagerVec feasible_chains_;
  int decrease_success = 0;
  int decrease_fail = 0;
};

}  // namespace DAG_SPACE