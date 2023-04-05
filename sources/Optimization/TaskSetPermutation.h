#pragma once
#include <list>

#include "sources/Baseline/StandardLET.h"
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
#include "sources/Optimization/LPSolver_Cplex.h"
#include "sources/Optimization/TaskSetPermutationHelper.h"
#include "sources/Permutations/ChainsPermutation.h"
#include "sources/Permutations/GraphOfChains.h"
#include "sources/Permutations/TwoTaskPermutationsIterator.h"
#include "sources/Utils/BatchUtils.h"
#include "sources/Utils/profilier.h"
namespace DAG_SPACE {

// currently, as asusme there is only one chain
// TODO: what's the usage of chains in arguments
class TaskSetPermutation {
 public:
  // TaskSetPermutation() {}
  TaskSetPermutation(const DAG_Model& dag_tasks,
                     const std::vector<std::vector<int>>& chains);

  void FindPairPermutations();
  bool ExamSchedulabilityOptSol() const;
  std::vector<Edge> GetAllEdges() const;
  // print optimization trajectory
  void PrintFeasibleChainsRecord() const;
  void PrintSinglePermIndex(const ChainsPermutation& chains_perm) const;
  void PrintSinglePermIndex(const ChainsPermutation& chains_perm,
                            const std::vector<Edge>& edges) const;

  template <typename ObjectiveFunction>
  int PerformOptimizationEnumerate() {
    ChainsPermutation chains_perm;
    IterateAllChainsPermutations<ObjectiveFunction>(0, chains_perm);
    lp_optimizer_.ClearCplexMemory();  // TODO: consider trying to optimize
    // performance by directly set coefficient
    // rather than remove/add constraints
    return best_yet_obj_;
  }

  // chains_perm already pushed the new perm_single
  template <typename ObjectiveFunction>
  bool WhetherSkipToNextPerm(const ChainsPermutation& chains_perm) {
#ifdef PROFILE_CODE
    BeginTimer(__FUNCTION__);
#endif

    std::vector<std::vector<int>> sub_chains =
        GetSubChains(dag_tasks_.chains_, chains_perm);
    for (const auto& sub_chain : sub_chains) {
      if (sub_chain.size() == 0) continue;
      if (GlobalVariablesDAGOpt::SKIP_PERM >= 2 &&
          !FindODFromSingleChainPermutation(dag_tasks_, chains_perm,
                                            graph_of_all_ca_chains_, sub_chain,
                                            rta_)
               .valid_) {
#ifdef PROFILE_CODE
        EndTimer(__FUNCTION__);
#endif
        if (GlobalVariablesDAGOpt::debugMode) {
          std::cout << "Early break at level " << chains_perm.size() << ": ";
          PrintSinglePermIndex(chains_perm);
          std::cout
              << " due to being conflicted permutations from sub-chains while "
                 "exploring the "
              // << adjacent_two_task_permutations_[position].size() -
              //        iterator.size()
              << " permutations\n";
          std::cout << "\n";
        }
        return true;
      }
    }

    VariableOD best_possible_variable_od =
        FindBestPossibleVariableOD(dag_tasks_, tasks_info_, rta_, chains_perm);
    double obj_curr =
        ObjectiveFunction::Obj(dag_tasks_, tasks_info_, chains_perm,
                               best_possible_variable_od, sub_chains);
    if (obj_curr > best_yet_obj_) {
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

  // optimize with Dynamic Programming
  // *********************************************

  template <typename ObjectiveFunction>
  int PerformOptimizationSort() {
    ChainsPermutation chains_perm;
    IterateSortedPerms<ObjectiveFunction>(0, chains_perm);
    lp_optimizer_.ClearCplexMemory();
    std::cout << "The number of feasibile chains found: "
              << feasible_chains_.size() << "\n";
    std::cout << "Decrease succes: " << decrease_success
              << ", Decrease Fail: " << decrease_fail << std::endl;
    PrintFeasibleChainsRecord();
    return best_yet_obj_;
  }

  std::vector<Edge> GetUnvisitedFutureEdges(uint position) const {
    std::vector<Edge> edges;
    edges.reserve(adjacent_two_task_permutations_.size());
    for (uint i = position + 1; i < adjacent_two_task_permutations_.size();
         i++) {
      edges.push_back(adjacent_two_task_permutations_.at(i).GetEdge());
    }
    return edges;
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

    BeginTimer("TwoTaskPermutationsIterator_constructor");
    PermIneqBound_Range perm_ineq_bound_range = GetEdgeIneqRange(
        adjacent_two_task_permutations_[position].GetEdge(),
        FindPossibleVariableOD(dag_tasks_, tasks_info_, rta_, chains_perm));
    TwoTaskPermutationsIterator iterator(
        adjacent_two_task_permutations_[position], perm_ineq_bound_range);
    EndTimer("TwoTaskPermutationsIterator_constructor");
    bool feasible_prev_chain = false;

    int count = iterator.size();
    while (!iterator.empty()) {
      if (ifTimeout(start_time_)) break;
      BeginTimer("RemoveCandidates_related");
      // TODO: predict decrease_fail?
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
      // TODO: consider where to put the code carefully

      count--;
      if (count < -10) {
        CoutWarning("deadlock found during IterateSortedPerms");
      }
    }
    return feasible_prev_chain;
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
  TimerType start_time_;
  DAG_Model dag_tasks_;
  RegularTaskSystem::TaskSetInfoDerived tasks_info_;
  GraphOfChains graph_of_all_ca_chains_;
  std::vector<TwoTaskPermutations> adjacent_two_task_permutations_;
  ChainsPermutation best_yet_chain_;
  int best_yet_obj_;
  int iteration_count_;
  VariableOD best_yet_variable_od_;
  std::vector<int> rta_;
  VariableOD best_possible_variable_od_;
  VariableRange variable_range_od_;
  int infeasible_iteration_ = 0;
  LPOptimizer lp_optimizer_;
  FeasiblieChainsManagerVec feasible_chains_;
  int decrease_success = 0;
  int decrease_fail = 0;
};

template <typename ObjectiveFunction>
ScheduleResult PerformTOM_OPT(const DAG_Model& dag_tasks,
                              std::string method = "Enumerate") {
  auto start = std::chrono::high_resolution_clock::now();
  ScheduleResult res;
  TaskSetPermutation task_sets_perms =
      TaskSetPermutation(dag_tasks, dag_tasks.chains_);
  if (method == "Enumerate")
    res.obj_ =
        task_sets_perms.PerformOptimizationEnumerate<ObjectiveFunction>();
  else if (method == "Sort")
    res.obj_ = task_sets_perms.PerformOptimizationSort<ObjectiveFunction>();
  else
    CoutError("Unrecognized method in PerformTOM_OPT");
  if (res.obj_ >= 1e8) {
    res.obj_ = PerformStandardLETAnalysis<ObjectiveFunction>(dag_tasks).obj_;
  }
  res.schedulable_ = task_sets_perms.ExamSchedulabilityOptSol();

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  res.timeTaken_ = double(duration.count()) / 1e6;
  std::cout << "The total number of permutation iterations is: "
            << task_sets_perms.iteration_count_ << "\n";
  if (GlobalVariablesDAGOpt::debugMode == 1) {
    std::cout << "The best permutation is: \n";
    task_sets_perms.best_yet_chain_.print();
  }
  if (!res.schedulable_ &&
      res.timeTaken_ < GlobalVariablesDAGOpt::TIME_LIMIT - 5)
    CoutError("Find an unschedulable case!");
  return res;
}

}  // namespace DAG_SPACE
