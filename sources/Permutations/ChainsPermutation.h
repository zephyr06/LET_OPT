#pragma once
#include <memory>

#include "sources/Optimization/Edge.h"
#include "sources/Optimization/GraphOfChains.h"
#include "sources/Permutations/TwoTaskPermutations.h"
#include "sources/Utils/profilier.h"
namespace DAG_SPACE {

Interval GetDeadlineRange(const VariableRange &variable_od_range,
                          const SinglePairPermutation &perm_prev);

Interval GetOffsetRange(const VariableRange &variable_od_range,
                        const SinglePairPermutation &perm_prev);

// this class actually stores graphs rather than a single chain
class ChainsPermutation {
 public:
  ChainsPermutation() {}

  ChainsPermutation(uint n) { permutation_chain_map_.reserve(n); }

  inline size_t size() const { return permutation_chain_map_.size(); }

  void push_back(const std::shared_ptr<const SinglePairPermutation> perm) {
    // permutation_chain_.push_back(perm);
    Edge edge_curr(perm->GetPrevTaskId(), perm->GetNextTaskId());
    if (permutation_chain_map_.find(edge_curr) !=
            permutation_chain_map_.end() &&
        *permutation_chain_map_[edge_curr] != *perm)
      CoutError(
          "Insert conflicted SinglePairPermutation into "
          "ChainsPermutation!");
    permutation_chain_map_[edge_curr] = perm;
  }

  void clear() { permutation_chain_map_.clear(); }

  inline void pop(const SinglePairPermutation &perm) {
    Edge edge_curr(perm.GetPrevTaskId(), perm.GetNextTaskId());
    permutation_chain_map_.erase(edge_curr);
  }

  inline void reserve(size_t n) { permutation_chain_map_.reserve(n); }

  // return true if the edge can be found in records
  bool exist(const Edge &edge) const {
    return permutation_chain_map_.find(edge) != permutation_chain_map_.end();
  }
  // TODO: return pointer?
  const SinglePairPermutation &operator[](const Edge &edge) const {
    auto itr = permutation_chain_map_.find(edge);
    if (itr == permutation_chain_map_.end())
      CoutError("Didn't find the given edge in SinglePairPermutation!");
    return *permutation_chain_map_.at(edge);
  }

  // several IsValid()-related functions
  bool IsValid(const VariableRange &variable_od_range,
               const SinglePairPermutation &perm_curr,
               const GraphOfChains &graph_of_all_ca_chains) const;

  bool IsPermConflicted_CheckAllWithSameSource(
      const VariableRange &variable_od_range,
      const SinglePairPermutation &perm_curr,
      const GraphOfChains &graph_of_all_ca_chains) const;

  bool IsPermConflicted_CheckAllWithSameSink(
      const VariableRange &variable_od_range,
      const SinglePairPermutation &perm_curr,
      const GraphOfChains &graph_of_all_ca_chains) const;

  bool IsPermConflicted_CheckAllSerialConnect(
      const VariableRange &variable_od_range,
      const SinglePairPermutation &perm_curr,
      const GraphOfChains &graph_of_all_ca_chains) const;

  void print() const {
    for (auto itr = permutation_chain_map_.begin();
         itr != permutation_chain_map_.end(); itr++) {
      itr->second->print();
    }
  }

  std::vector<Edge> GetEdges() const {
    std::vector<Edge> edges;
    edges.reserve(size());
    for (auto itr = permutation_chain_map_.begin();
         itr != permutation_chain_map_.end(); itr++) {
      edges.push_back(itr->first);
    }
    return edges;
  }
  // data members
 private:
  std::unordered_map<Edge, std::shared_ptr<const SinglePairPermutation>>
      permutation_chain_map_;
};

VariableRange FindPossibleVariableOD(const DAG_Model &dag_tasks,
                                     const TaskSetInfoDerived &tasks_info,
                                     const std::vector<int> &rta,
                                     const ChainsPermutation &chains_perm);
VariableOD FindBestPossibleVariableOD(const DAG_Model &dag_tasks,
                                      const TaskSetInfoDerived &tasks_info,
                                      const std::vector<int> &rta,
                                      const ChainsPermutation &chains_perm);
}  // namespace DAG_SPACE