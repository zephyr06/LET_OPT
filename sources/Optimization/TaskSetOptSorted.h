#pragma once
#include "sources/Optimization/TaskSetPermutation.h"
namespace DAG_SPACE {
bool AllChainsNotEmpty(const std::vector<std::vector<int>>& chains);
class TaskSetOptSorted : public TaskSetPermutation {
 public:
  TaskSetOptSorted(const DAG_Model& dag_tasks,
                   const std::vector<std::vector<int>>& chains,
                   const std::string& type_trait)
      : TaskSetPermutation(dag_tasks, chains, type_trait) {
    feasible_chains_ =
        FeasiblieChainsManagerVec(adjacent_two_task_permutations_.size());
  }

  // ONLY for TaskSetOptSorted_Offset's constructor;
  // the only difference between this constructor and the base is that this
  // one doesn't call FindPairPermutations()
  TaskSetOptSorted(const DAG_Model& dag_tasks,
                   const std::vector<std::vector<int>>& chains)
      : TaskSetPermutation(dag_tasks, chains) {
    feasible_chains_ =
        FeasiblieChainsManagerVec(adjacent_two_task_permutations_.size());
    type_trait_ = "DataAge";
  }

  void PrintFeasibleChainsRecord() const;

  template <typename ObjectiveFunction>
  ScheduleResult PerformOptimizationSort() {
    InitializeSolutions<ObjectiveFunction>(DefaultLET);
    InitializeSolutions<ObjectiveFunction>(Maia23);
    ChainsPermutation chains_perm;
    IterateSortedPerms<ObjectiveFunction>(0, chains_perm);
    std::cout << "The number of feasibile chains found: "
              << feasible_chains_.size() << "\n";
    std::cout << "Decrease succes: " << decrease_success
              << ", Decrease Fail: " << decrease_fail << std::endl;
    PrintFeasibleChainsRecord();
    // return best_yet_obj_;
    return GetScheduleResult<ObjectiveFunction>();
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
        dag_tasks_, tasks_info_, rta_, chains_perm, optimize_offset_only_);
    PermIneqBound_Range perm_ineq_bound_range = GetEdgeIneqRange(
        adjacent_two_task_permutations_[position].GetEdge(),
        variable_range_w_chains, ObjectiveFunction::type_trait);
#ifdef PROFILE_CODE
    BeginTimer("iterator_constructor");
#endif
    TwoTaskPermutationsIterator iterator(
        adjacent_two_task_permutations_[position], perm_ineq_bound_range);
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
      if (chains_perm.IsValid(variable_range_w_chains, *perm_sing_curr,
                              graph_of_all_ca_chains_, rta_)) {
        chains_perm.push_back(perm_sing_curr);

        if (!WhetherSkipToNextPerm<ObjectiveFunction>(chains_perm)) {
          IterateSortedPerms<ObjectiveFunction>(position + 1, chains_perm);
        }
        chains_perm.pop(perm_sing_curr);
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
  }

  // chains_perm already pushed the new perm_single
  template <typename ObjectiveFunction>
  bool WhetherSkipToNextPerm(const ChainsPermutation& chains_perm) {
#ifdef PROFILE_CODE
    BeginTimer(__FUNCTION__);
#endif
    std::vector<std::vector<int>> sub_chains =
        GetSubChains(dag_tasks_.chains_, chains_perm);

    // int best_possible_obj_incomplete =
    //     GetBestPossibleObj_UpperBound<ObjectiveFunction>(chains_perm,
    //                                                      sub_chains);
    // TODO: this part can be improved when there are multiple short chains
    int best_possible_obj_incomplete = 0;
    if (AllChainsNotEmpty(sub_chains))
      best_possible_obj_incomplete =
          FindODWithLP(dag_tasks_, tasks_info_, chains_perm, sub_chains,
                       ObjectiveFunction::type_trait, rta_,
                       optimize_offset_only_)
              .second;

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

  template <typename ObjectiveFunction>
  double GetBestPossibleObj_UpperBound(
      const ChainsPermutation& chains_perm,
      const std::vector<std::vector<int>>& sub_chains) {
    VariableOD best_possible_variable_od = FindBestPossibleVariableOD(
        dag_tasks_, tasks_info_, rta_, chains_perm, optimize_offset_only_);
    if (IfRT_Trait(ObjectiveFunction::type_trait))
      return ObjReactionTimeApprox::Obj(dag_tasks_, tasks_info_, chains_perm,
                                        best_possible_variable_od, sub_chains);
    else if (IfDA_Trait(ObjectiveFunction::type_trait))
      return ObjDataAgeApprox::Obj(dag_tasks_, tasks_info_, chains_perm,
                                   best_possible_variable_od, sub_chains);
    else
      CoutError("unrecognized obj in GetBestPossibleObj_UpperBound");
    return 0;
  }

  template <typename ObjectiveFunction>
  double EvaluateChainsPermutation(const ChainsPermutation& chains_perm) {
#ifdef PROFILE_CODE
    BeginTimer(__FUNCTION__);
#endif
    // chains_perm.print();
    std::pair<VariableOD, int> res = FindODWithLP(
        dag_tasks_, tasks_info_, chains_perm, graph_of_all_ca_chains_,
        ObjectiveFunction::type_trait, rta_, optimize_offset_only_);

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
  bool optimize_offset_only_ = false;
};

}  // namespace DAG_SPACE