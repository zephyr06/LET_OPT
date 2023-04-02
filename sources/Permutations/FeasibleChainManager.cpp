
#include "sources/Permutations/FeasibleChainManager.h"

namespace DAG_SPACE {

// chains_perm needs to have the same order as adjacent_two_task_permutations
FeasibleChainManager::FeasibleChainManager(
    const ChainsPermutation& chains_perm,
    const std::vector<TwoTaskPermutations>& adjacent_two_task_permutations,
    const std::string& obj_trait)
    : feasible_chain_(chains_perm),
      adjacent_two_task_permutations_(adjacent_two_task_permutations) {
  FindBetterPermsPerEdge(obj_trait);
  // std::cout << "Size of FeasibleChainManager: " << sizeof(*this) << "\n";
}

void FeasibleChainManager::FindBetterPermsPerEdge(
    const std::string& obj_trait) {
  for (uint i = 0; i < adjacent_two_task_permutations_.size(); i++) {
    const TwoTaskPermutations& perms_all = adjacent_two_task_permutations_[i];
    const Edge& edge_curr = perms_all.GetEdge();
    PermRefSet perm_ref_set;
    perm_ref_set.reserve(perms_all.size());
    if (feasible_chain_.exist(edge_curr)) {
      const SinglePairPermutation& perm_curr = *feasible_chain_[edge_curr];
      for (uint j = 0; j < perms_all.size(); j++) {
        const auto& perm_ptr = perms_all[j];
        if (!IfSkipAnotherPerm(perm_curr, *perm_ptr, obj_trait)) {
          perm_ref_set.insert(*perm_ptr);
        }
      }
    }
    better_perm_per_chain_per_edge_[edge_curr] = perm_ref_set;
  }
}

}  // namespace DAG_SPACE
