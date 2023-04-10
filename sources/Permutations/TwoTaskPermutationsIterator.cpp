

#include "sources/Permutations/TwoTaskPermutationsIterator.h"
namespace DAG_SPACE {

PermIneqBound_Range GetEdgeIneqRange(const Edge& edge,
                                     const VariableRange& variable_range,
                                     const std::string& type_trait) {
  if (type_trait == "ReactionTimeApprox" || type_trait == "ReactionTime") {
    return GetEdgeIneqRangeRT(edge, variable_range);
  } else if (type_trait == "DataAgeApprox" || type_trait == "DataAge") {
    return GetEdgeIneqRangeDA(edge, variable_range);
  } else
    CoutError("Unrecognized type_trait in GetEdgeIneqRange!");
  return PermIneqBound_Range{-1, -1};
}

bool IfChainsContainBetterPerm(const ChainsPermutation& chains_perm_partial,
                               const FeasibleChainManager& feasible_chain_man) {
  const std::vector<Edge>& edges_inserted = chains_perm_partial.GetEdges();
  for (const auto& edge : edges_inserted) {
    const auto& ptr_curr = chains_perm_partial[edge];
    const PermRefSet& perms_better_set =
        feasible_chain_man.better_perm_per_chain_per_edge_.at(edge);

    if (perms_better_set.find(*ptr_curr) != perms_better_set.end()) {
      return true;
    }
  }
  return false;
}

bool IfFutureEdgesContainBetterPerm(
    const std::vector<Edge>& unvisited_edges,
    const FeasibleChainManager& feasible_chain_man) {
  for (const auto& edge_curr : unvisited_edges) {
    if (feasible_chain_man.better_perm_per_chain_per_edge_.at(edge_curr)
            .size() > 0)
      return true;
  }
  return false;
}

TwoTaskPermutationsIterator::TwoTaskPermutationsIterator(
    const TwoTaskPermutations& two_task_perms,
    const PermIneqBound_Range& perm_range)
    : TwoTaskPermutations(two_task_perms) {
  for (const auto& ptr : single_permutations_) {
    if (ptr->inequality_.lower_bound_ <= perm_range.lower_bound_s_upper_bound &&
        ptr->inequality_.upper_bound_ >= perm_range.upper_bound_s_lower_bound)
      single_perms_ite_record_.push_back(ptr);
  }
}

void TwoTaskPermutationsIterator::TakeCommonElements(
    const PermRefSet& per_ptr_set) {
  // #ifdef PROFILE_CODE
  //   BeginTimer(__FUNCTION__);
  // #endif
  for (auto itr = single_perms_ite_record_.begin();
       itr != single_perms_ite_record_.end();) {
    if (per_ptr_set.find(**itr) == per_ptr_set.end())
      itr = single_perms_ite_record_.erase(itr);
    else
      ++itr;
  }
  // #ifdef PROFILE_CODE
  //   EndTimer(__FUNCTION__);
  // #endif
}

// first generate candidates to select at this level, then take intersection
// with current available candidates;
void TwoTaskPermutationsIterator::RemoveCandidate(
    const ChainsPermutation& chains_perm_partial,
    const FeasibleChainManager& feasible_chain_man,
    const std::vector<Edge>& unvisited_future_edges) {
  // #ifdef PROFILE_CODE
  //   BeginTimer(__FUNCTION__);
  // #endif
  if (IfChainsContainBetterPerm(chains_perm_partial, feasible_chain_man) ||
      IfFutureEdgesContainBetterPerm(unvisited_future_edges,
                                     feasible_chain_man)) {
    // #ifdef PROFILE_CODE
    //     EndTimer(__FUNCTION__);
    // #endif
    return;
  }

  // in this case, the possible elements to iterate must include only those
  // possibly better perms
  TakeCommonElements(
      feasible_chain_man.better_perm_per_chain_per_edge_.at(GetEdge()));
  // #ifdef PROFILE_CODE
  //   EndTimer(__FUNCTION__);
  // #endif
}

void TwoTaskPermutationsIterator::RemoveCandidates(
    const ChainsPermutation& chains_perm_partial,
    const std::vector<FeasibleChainManager>& feasible_chain_man_vec,
    const std::vector<Edge>& unvisited_future_edges) {
  for (const auto& feasible_chain_man : feasible_chain_man_vec) {
    if (feasible_chain_man_vec.size() > 5 &&
        double(rand()) / RAND_MAX <
            GlobalVariablesDAGOpt::SAMPLE_FEASIBLE_CHAINS)
      continue;
    RemoveCandidate(chains_perm_partial, feasible_chain_man,
                    unvisited_future_edges);
  }
}
void TwoTaskPermutationsIterator::RemoveCandidates(
    const ChainsPermutation& chains_perm_partial,
    const std::deque<FeasibleChainManager>& feasible_chain_man_vec,
    const std::vector<Edge>& unvisited_future_edges) {
  // for (const auto& feasible_chain_man : feasible_chain_man_vec) {
  for (auto feasible_chain_man = feasible_chain_man_vec.rbegin();
       feasible_chain_man != feasible_chain_man_vec.rend();
       feasible_chain_man++) {
    RemoveCandidate(chains_perm_partial, *feasible_chain_man,
                    unvisited_future_edges);
    if (empty()) break;
  }
}

}  // namespace DAG_SPACE