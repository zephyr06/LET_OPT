#pragma once
#include <deque>

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
    chain_man_vec_.reserve(n);
    // chain_man_vec_incomplete_.reserve(n);
  }

  inline size_t size() const { return chain_man_vec_.size(); }

  void push_back(const FeasibleChainManager& feasible_chain_man) {
    if (chain_man_vec_.size() >
        uint(GlobalVariablesDAGOpt::FEASIBLE_CHAINS_MAX)) {
      // CoutError("too many feasible_chains!");
      chain_man_vec_.pop_back();
    }
    chain_man_vec_.push_back(feasible_chain_man);
  }

  inline void pop_back() { chain_man_vec_.pop_back(); }

  void push_back_incomplete(const FeasibleChainManager& feasible_chain_man) {
    if (chain_man_vec_incomplete_.size() >=
        uint(GlobalVariablesDAGOpt::FEASIBLE_INCOMPLETE_CHAINS_MAX)) {
      pop_incomplete();
    }
    chain_man_vec_incomplete_.push_back(feasible_chain_man);
  }
  inline void pop_incomplete() { chain_man_vec_incomplete_.pop_front(); }

  // data member
  std::vector<FeasibleChainManager> chain_man_vec_;
  std::deque<FeasibleChainManager> chain_man_vec_incomplete_;
  uint length;
};
}  // namespace DAG_SPACE
