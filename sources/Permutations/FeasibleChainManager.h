#pragma once
#include "sources/Permutations/ChainsPermutation.h"

namespace DAG_SPACE {
class FeasibleChainManager {
 public:
  // FeasibleChainManager() {}

  // chains_perm needs to have the same order as adjacent_two_task_permutations
  FeasibleChainManager(
      const ChainsPermutation& chains_perm,
      const std::vector<TwoTaskPermutations>& adjacent_two_task_permutations,
      const std::string& obj_trait);

  void FindBetterPermsPerEdge(const std::string& obj_trait);

  // data members
  ChainsPermutation feasible_chain_;
  // for each edge of each feasible_chain, it stores the set of
  // SinglePermutations that could possible improve
  std::unordered_map<Edge, PermPtrSet> better_perm_per_chain_per_edge_;
  const std::vector<TwoTaskPermutations>& adjacent_two_task_permutations_;
};

typedef std::vector<FeasibleChainManager> FeasiblieChainsManagerVec;
}  // namespace DAG_SPACE
