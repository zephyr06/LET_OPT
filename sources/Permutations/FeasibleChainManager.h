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
  std::unordered_map<Edge, PermRefSet> better_perm_per_chain_per_edge_;
  const std::vector<TwoTaskPermutations>& adjacent_two_task_permutations_;
};

// typedef std::vector<FeasibleChainManager> FeasiblieChainsManagerVec;

class FeasiblieChainsManagerVec {
 public:
  FeasiblieChainsManagerVec() {}

  FeasiblieChainsManagerVec(uint n) : length(n) {
    feasible_chains_modify_record.reserve(n);
    for (uint i = 0; i < n; i++) feasible_chains_modify_record.push_back(false);
  }

  inline size_t size() const { return chain_man_vec_.size(); }

  void SetModify() {
    for (uint i = 0; i < length; i++) feasible_chains_modify_record[i] = true;
  }

  void UnSetModify(uint level) {
    if (level >= length)
      CoutError("invalid level in FeasiblieChainsManagerVec");
    feasible_chains_modify_record[level] = false;
  }

  bool IfModified(uint level) {
    if (level >= length)
      CoutError("invalid level in FeasiblieChainsManagerVec");
    return feasible_chains_modify_record[level];
  }

  void push_back(const FeasibleChainManager& feasible_chain_man) {
    chain_man_vec_.push_back(feasible_chain_man);
    SetModify();
  }

  void pop_back() {
    chain_man_vec_.pop_back();
    SetModify();
  }

  // data member
  std::vector<FeasibleChainManager> chain_man_vec_;
  std::vector<bool> feasible_chains_modify_record;
  uint length;
};
}  // namespace DAG_SPACE
