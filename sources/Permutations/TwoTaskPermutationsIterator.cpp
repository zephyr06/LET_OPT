

#include "sources/Permutations/TwoTaskPermutationsIterator.h"
namespace DAG_SPACE {

bool IfChainsContainBetterPerm(const ChainsPermutation& chains_perm_partial,
                               const FeasibleChainManager& feasible_chain_man) {
  const std::vector<Edge>& edges_inserted = chains_perm_partial.GetEdges();
  for (const auto& edge : edges_inserted) {
    const auto& ptr_curr = chains_perm_partial[edge];
    const PermRefSet& perms_better_set =
        feasible_chain_man.better_perm_per_chain_per_edge_.at(edge);

    if (perms_better_set.find(*ptr_curr) != perms_better_set.end()) return true;
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

void TwoTaskPermutationsIterator::TakeCommonElements(
    const PermRefSet& per_ptr_set) {
  for (auto itr = single_perms_ite_record_.begin();
       itr != single_perms_ite_record_.end();) {
    if (per_ptr_set.find(**itr) == per_ptr_set.end())
      itr = single_perms_ite_record_.erase(itr);
    else
      ++itr;
  }
}

// first generate candidates to select at this level, then take intersection
// with current available candidates;
void TwoTaskPermutationsIterator::RemoveCandidate(
    const ChainsPermutation& chains_perm_partial,
    const FeasibleChainManager& feasible_chain_man,
    const std::vector<Edge>& unvisited_future_edges) {
  if (IfChainsContainBetterPerm(chains_perm_partial, feasible_chain_man) ||
      IfFutureEdgesContainBetterPerm(unvisited_future_edges,
                                     feasible_chain_man))
    return;

  // in this case, the possible elements to iterate must include only those
  // possibly better perms
  TakeCommonElements(
      feasible_chain_man.better_perm_per_chain_per_edge_.at(GetEdge()));
}

}  // namespace DAG_SPACE