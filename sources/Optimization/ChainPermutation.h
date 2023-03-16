#pragma once
#include "sources/Optimization/Edge.h"
#include "sources/Optimization/GraphOfChains.h"
#include "sources/Optimization/TwoTaskPermutations.h"
#include "sources/Utils/profilier.h"
namespace DAG_SPACE {

Interval GetDeadlineRange(const VariableRange &variable_od_range,
                          const SinglePairPermutation &perm_prev);

Interval GetOffsetRange(const VariableRange &variable_od_range,
                        const SinglePairPermutation &perm_prev);

// this class actually stores graphs rather than a single chain
class ChainPermutation {
   public:
    ChainPermutation() {}

    ChainPermutation(uint n) { permutation_chain_map_.reserve(n); }

    inline size_t size() const { return permutation_chain_map_.size(); }

    inline void push_back(const SinglePairPermutation &perm) {
        // permutation_chain_.push_back(perm);
        Edge edge_curr(perm.GetPrevTaskId(), perm.GetNextTaskId());
        if (permutation_chain_map_.find(edge_curr) !=
                permutation_chain_map_.end() &&
            permutation_chain_map_[edge_curr] != perm)
            CoutError(
                "Insert conflicted SinglePairPermutation into "
                "ChainPermutation!");
        permutation_chain_map_[edge_curr] = perm;
    }

    void clear() { permutation_chain_map_.clear(); }

    inline void pop(const SinglePairPermutation &perm) {
        Edge edge_curr(perm.GetPrevTaskId(), perm.GetNextTaskId());
        permutation_chain_map_.erase(edge_curr);
    }

    inline void reserve(size_t n) { permutation_chain_map_.reserve(n); }

    // SinglePairPermutation operator[](size_t i) const {
    //     if (i >= size()) CoutError("Out-of-range error!");
    //     return permutation_chain_[i];
    // }

    SinglePairPermutation operator[](const Edge &edge) const {
        auto itr = permutation_chain_map_.find(edge);
        if (itr == permutation_chain_map_.end())
            CoutError("Didn't find the given edge in SinglePairPermutation!");
        return permutation_chain_map_.at(edge);
    }

    // int GetTaskId(size_t i) const;

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

    // data members
   private:
    std::unordered_map<Edge, SinglePairPermutation> permutation_chain_map_;
};

}  // namespace DAG_SPACE