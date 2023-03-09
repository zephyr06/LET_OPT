#pragma once

#include "sources/Optimization/TwoTaskPermutations.h"

namespace DAG_SPACE {

class ChainPermutation {
   public:
    ChainPermutation() {}

    inline size_t size() const { return permutation_chain_.size(); }
    inline void push_back(const SinglePairPermutation &perm) {
        permutation_chain_.push_back(perm);
    }
    inline void reserve(size_t n) { permutation_chain_.reserve(n); }
    bool IsValid() const { return true; }
    SinglePairPermutation operator[](size_t i) const {
        if (i >= size()) CoutError("Out-of-range error!");
        return permutation_chain_[i];
    }

    int GetTaskId(size_t i) const {
        if (i < size())
            return permutation_chain_[i].inequality_.task_prev_id_;
        else if (i == size()) {
            return permutation_chain_[size() - 1].inequality_.task_next_id_;
        } else
            CoutError("Out-or-range error in ChainPermutation!");
        return -1;
    }

    // data members
    std::vector<SinglePairPermutation> permutation_chain_;
};

}  // namespace DAG_SPACE