
#include "sources/Optimization/PermutationInequality.h"

namespace DAG_SPACE {

bool ExamConfliction(const PermutationInequality& perm1,
                     const PermutationInequality& perm2) {
    Interval interval1 = perm1.GetInterval();
    Interval interval2 = perm2.GetInterval();
    if (Overlap(interval1, interval2) > 0)
        return false;
    else
        return true;
}

int MergeSmallerThanValue(const PermutationInequality& perm1,
                          const PermutationInequality& perm2) {
    if (perm1.lower_bound_valid_ && perm2.lower_bound_valid_)
        return std::max(perm1.lower_bound_, perm2.lower_bound_);
    else if (perm1.lower_bound_valid_)
        return perm1.lower_bound_;
    else if (perm2.lower_bound_valid_)
        return perm2.lower_bound_;
    else
        return -1e9;
}

int MergeLargerThanValue(const PermutationInequality& perm1,
                         const PermutationInequality& perm2) {
    if (perm1.upper_bound_valid_ && perm2.upper_bound_valid_)
        return std::min(perm1.upper_bound_, perm2.upper_bound_);
    else if (perm1.upper_bound_valid_)
        return perm1.upper_bound_;
    else if (perm2.upper_bound_valid_)
        return perm2.upper_bound_;
    else
        return 1e9;
}

PermutationInequality MergeTwoSinglePermutations(
    const PermutationInequality& perm1, const PermutationInequality& perm2) {
    PermutationInequality merged_perm;

    if (ExamConfliction(perm1, perm2) ||
        perm1.task_prev_id_ != perm2.task_prev_id_ ||
        perm1.task_next_id_ != perm2.task_next_id_) {
        merged_perm.setInvalid();
    } else {
        int smaller_value_merged = MergeSmallerThanValue(perm1, perm2);
        int larger_value_merged = MergeLargerThanValue(perm1, perm2);

        merged_perm = PermutationInequality(
            perm1.task_prev_id_, perm1.task_next_id_, smaller_value_merged,
            perm1.lower_bound_valid_ || perm2.lower_bound_valid_,
            larger_value_merged,
            perm1.upper_bound_valid_ || perm2.upper_bound_valid_);
    }
    return merged_perm;
}

}  // namespace DAG_SPACE