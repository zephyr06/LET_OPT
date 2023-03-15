

#include "sources/Optimization/ChainPermutation.h"

namespace DAG_SPACE {

// TODO: not only check the last two adjacent chains
bool ChainPermutation::IsValid(const VariableRange& variable_od_range) const {
    // return true;
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

        int offset_next_max =
            variable_od_range.upper_bound.at(perm_curr.GetNextTaskId()).offset;
        int deadline_curr_max_from_ineq =
            offset_next_max + perm_curr.inequality_.upper_bound_;

        int deadline_curr_max =
            variable_od_range.upper_bound.at(perm_curr.GetPrevTaskId())
                .deadline;
        deadline_curr_max =
            std::min(deadline_curr_max, deadline_curr_max_from_ineq);

        int rta_curr =
            variable_od_range.lower_bound.at(perm_curr.GetPrevTaskId())
                .deadline;
        int offset_curr_max = deadline_curr_max - rta_curr;
        if (offset_curr_max < offset_curr_min) {
            return false;
        }
    }

    return true;
}

bool IsValidTwoSerialSinglePerm(const VariableRange& variable_od_range,
                                const SinglePairPermutation& perm_prev,
                                const SinglePairPermutation& perm_curr) {
    // return true;
    // int perm_single_chain_size = permutation_chain_.size();
    // if (perm_single_chain_size > 1) {
    // const SinglePairPermutation& perm_prev =
    //     permutation_chain_[perm_single_chain_size - 2];
    int deadline_prev_min =
        variable_od_range.lower_bound.at(perm_prev.GetPrevTaskId())
            .deadline;  // this is rta
    int offset_curr_min =
        deadline_prev_min - perm_prev.inequality_.upper_bound_;

    // const SinglePairPermutation& perm_curr =
    //     permutation_chain_[perm_single_chain_size - 1];

    int offset_next_max =
        variable_od_range.upper_bound.at(perm_curr.GetNextTaskId()).offset;
    int deadline_curr_max_from_ineq =
        offset_next_max + perm_curr.inequality_.upper_bound_;

    int deadline_curr_max =
        variable_od_range.upper_bound.at(perm_curr.GetPrevTaskId()).deadline;
    deadline_curr_max =
        std::min(deadline_curr_max, deadline_curr_max_from_ineq);

    int rta_curr =
        variable_od_range.lower_bound.at(perm_curr.GetPrevTaskId()).deadline;
    int offset_curr_max = deadline_curr_max - rta_curr;
    if (offset_curr_max < offset_curr_min) {
        return false;
    }
    // }

    return true;
}

// TODO: add other check types
bool ChainPermutation::IsValid(const VariableRange& variable_od_range,
                               const SinglePairPermutation& perm_curr) const {
    int perm_single_chain_size = permutation_chain_.size();
    if (perm_single_chain_size > 0) {
        const SinglePairPermutation& perm_prev =
            permutation_chain_[perm_single_chain_size - 1];
        if (!IsValidTwoSerialSinglePerm(variable_od_range, perm_prev,
                                        perm_curr))
            return false;
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