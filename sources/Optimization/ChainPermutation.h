#pragma once
#include "sources/Optimization/Edge.h"
#include "sources/Optimization/GraphOfChains.h"
#include "sources/Optimization/TwoTaskPermutations.h"
#include "sources/Utils/profilier.h"
namespace DAG_SPACE {

class ChainPermutation {
   public:
    ChainPermutation() {}

    ChainPermutation(uint n) { permutation_chain_.reserve(n); }

    inline size_t size() const { return permutation_chain_.size(); }
    inline void push_back(const SinglePairPermutation &perm) {
        permutation_chain_.push_back(perm);
        Edge edge_curr(perm.GetPrevTaskId(), perm.GetNextTaskId());
        permutation_map_[edge_curr] = permutation_chain_.size() - 1;
    }
    void clear() {
        permutation_map_.clear();
        permutation_chain_.clear();
    }

    inline void pop_back() { permutation_chain_.pop_back(); }
    inline void reserve(size_t n) { permutation_chain_.reserve(n); }

    SinglePairPermutation operator[](size_t i) const {
        if (i >= size()) CoutError("Out-of-range error!");
        return permutation_chain_[i];
    }

    SinglePairPermutation operator[](const Edge &edge) const {
        auto itr = permutation_map_.find(edge);
        if (itr == permutation_map_.end())
            CoutError("Didn't find the given edge in SinglePairPermutation!");
        return permutation_chain_[permutation_map_.at(edge)];
    }

    int GetTaskId(size_t i) const;

    // several IsValid()-related functions
    bool IsValid(const VariableRange &variable_od_range,
                 const SinglePairPermutation &perm_curr,
                 const GraphOfChains &graph_of_all_ca_chains) const;

    bool IsValidSameSourceCheck(
        int source_task_id, int sink_task_id,
        const VariableRange &variable_od_range,
        const SinglePairPermutation &perm_curr,
        const GraphOfChains &graph_of_all_ca_chains) const;

    bool IsValidSameSinkCheck(
        int source_task_id, int sink_task_id,
        const VariableRange &variable_od_range,
        const SinglePairPermutation &perm_curr,
        const GraphOfChains &graph_of_all_ca_chains) const;

    // data members

   private:
    std::unordered_map<Edge, int>
        permutation_map_;  // edge to the index of its SinglePairPermutation
    // TODO: merge these two members
    std::vector<SinglePairPermutation> permutation_chain_;
};

}  // namespace DAG_SPACE