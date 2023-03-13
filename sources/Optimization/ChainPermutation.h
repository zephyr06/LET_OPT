#pragma once

#include "sources/Optimization/TwoTaskPermutations.h"

namespace DAG_SPACE {

class ChainPermutation {
   public:
    ChainPermutation() {}

    ChainPermutation(uint n) { permutation_chain_.reserve(n); }

    inline size_t size() const { return permutation_chain_.size(); }
    inline void push_back(const SinglePairPermutation &perm) {
        permutation_chain_.push_back(perm);
    }
    inline void pop_back() { permutation_chain_.pop_back(); }
    inline void reserve(size_t n) { permutation_chain_.reserve(n); }
    // exam the last two single-pair-permutation to see if there are
    // conflictions
    bool IsValid(const  VariableRange &variable_od_range) const;

    SinglePairPermutation operator[](size_t i) const {
        if (i >= size()) CoutError("Out-of-range error!");
        return permutation_chain_[i];
    }

    int GetTaskId(size_t i) const;

    // data members
    std::vector<SinglePairPermutation> permutation_chain_;
};

}  // namespace DAG_SPACE