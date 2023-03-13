

#include "sources/Optimization/ChainPermutation.h"

namespace DAG_SPACE {

bool ChainPermutation::IsValid(const VariableRange& variable_od_range) const {
    return true;
    int perm_single_chain_size = permutation_chain_.size();
    if (perm_single_chain_size > 1) {
        const SinglePairPermutation& perm_prev =
            permutation_chain_[perm_single_chain_size - 2];
        int deadline_prev_min =
            variable_od_range.lower_bound.at(perm_prev.GetPrevTaskId())
                .deadline;  // this is rta
        int offset_curr_min =
            deadline_prev_min - perm_prev.inequality_.upper_bound_;

        const SinglePairPermutation& perm_curr =
            permutation_chain_[perm_single_chain_size - 1];
        int deadline_curr_min = 0 + perm_curr.inequality_.lower_bound_;
        int rta_curr =
            variable_od_range.lower_bound.at(perm_curr.GetPrevTaskId())
                .deadline;
        int offset_curr_max = deadline_curr_min - rta_curr;
        if (offset_curr_max < offset_curr_min) return false;
    }
    return true;
}

int ChainPermutation::GetTaskId(size_t i) const {
    if (i < size())
        return permutation_chain_[i].inequality_.task_prev_id_;
    else if (i == size()) {
        return permutation_chain_[size() - 1].inequality_.task_next_id_;
    } else
        CoutError("Out-or-range error in ChainPermutation!");
    return -1;
}

}  // namespace DAG_SPACE