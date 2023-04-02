
#pragma once

#include <memory>

#include "sources/Optimization/Variable.h"
#include "sources/Permutations/FeasibleChainManager.h"
#include "sources/Permutations/TwoTaskPermutations.h"
#include "sources/Utils/profilier.h"
namespace DAG_SPACE {

bool IfChainsContainBetterPerm(const ChainsPermutation& chains_perm_partial,
                               const FeasibleChainManager& feasible_chain_man);

bool IfFutureEdgesContainBetterPerm(
    const std::vector<Edge>& unvisited_edges,
    const FeasibleChainManager& feasible_chain_man);

// this struct saves the possible range for a valid perm_ineq;
struct PermIneqBound_Range {
  // the upper bound of the member 'lower_bound_' in class PermutationInequality
  int lower_bound_s_upper_bound;
  // the lower bound of the member 'lower_bound_' in class PermutationInequality
  int upper_bound_s_lower_bound;
};

inline PermIneqBound_Range GetEdgeIneqRange(
    const Edge& edge, const VariableRange& variable_range) {
  int low = variable_range.upper_bound.at(edge.from_id).deadline -
            variable_range.lower_bound.at(edge.to_id).offset;
  int upp = variable_range.lower_bound.at(edge.from_id).deadline -
            variable_range.upper_bound.at(edge.to_id).offset;
  return PermIneqBound_Range{low, upp};
}

class TwoTaskPermutationsIterator : public TwoTaskPermutations {
 public:
  TwoTaskPermutationsIterator() {}
  // TODO: there is a copy consturctor call, consider adjust it
  TwoTaskPermutationsIterator(const TwoTaskPermutations& two_task_perms)
      : TwoTaskPermutations(two_task_perms) {
    for (const auto& ptr : single_permutations_)
      single_perms_ite_record_.push_back(ptr);
  }

  TwoTaskPermutationsIterator(const TwoTaskPermutations& two_task_perms,
                              const PermIneqBound_Range& perm_range)
      : TwoTaskPermutations(two_task_perms) {
    for (const auto& ptr : single_permutations_) {
      if (ptr->inequality_.lower_bound_ <=
              perm_range.lower_bound_s_upper_bound &&
          ptr->inequality_.upper_bound_ >= perm_range.upper_bound_s_lower_bound)
        single_perms_ite_record_.push_back(ptr);
    }
  }

  // unvisited_future_edges don't include the edge to iterate in the current
  // loop
  void RemoveCandidate(const ChainsPermutation& chains_perm_partial,
                       const FeasibleChainManager& feasible_chain_man,
                       const std::vector<Edge>& unvisited_future_edges);

  // unvisited_future_edges don't include the edge to iterate in the current
  // loop
  void RemoveCandidates(
      const ChainsPermutation& chains_perm_partial,
      const std::vector<FeasibleChainManager>& feasible_chain_man_vec,
      const std::vector<Edge>& unvisited_future_edges) {
    for (const auto& feasible_chain_man : feasible_chain_man_vec) {
      // #ifdef PROFILE_CODE
      //       BeginTimer("RemoveCandidates_innerloop");
      // #endif
      if (feasible_chain_man_vec.size() > 5 &&
          double(rand()) / RAND_MAX <
              GlobalVariablesDAGOpt::SAMPLE_FEASIBLE_CHAINS)
        continue;
      RemoveCandidate(chains_perm_partial, feasible_chain_man,
                      unvisited_future_edges);

      // #ifdef PROFILE_CODE
      //       EndTimer("RemoveCandidates_innerloop");
      // #endif
    }
  }

  void TakeCommonElements(const PermRefSet& per_ptr_set);

  inline const std::shared_ptr<const SinglePairPermutation> pop_front() {
    if (single_perms_ite_record_.size() == 0)
      CoutError("No elements left in TwoTaskPermutationsIterator!");
    auto itr = single_perms_ite_record_.front();
    single_perms_ite_record_.pop_front();
    return itr;
  }

  inline bool empty() const { return single_perms_ite_record_.empty(); }

  inline size_t size() const { return single_perms_ite_record_.size(); }

  inline bool ifHarmonic() const {
    int period_prev = tasks_info_.GetTask(task_prev_id_).period;
    int period_next = tasks_info_.GetTask(task_next_id_).period;
    return period_prev % period_next == 0 || period_next % period_prev == 0;
  }

  // data members
  std::list<std::shared_ptr<const SinglePairPermutation>>
      single_perms_ite_record_;
  bool if_harmonic;
};
}  // namespace DAG_SPACE