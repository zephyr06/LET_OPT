
#include "sources/Optimization/TwoTaskSinglePermutation.h"

namespace DAG_SPACE {

bool ExamConfliction(const TwoTaskSinlgePermutation& perm1,
                     const TwoTaskSinlgePermutation& perm2) {
    Interval interval1 = perm1.GetInterval();
    Interval interval2 = perm2.GetInterval();
    if (Overlap(interval1, interval2) > 0 ||
        WhetherAdjacent(interval1, interval2) == true)
        return false;
    else
        return true;
}

int MergeSmallerThanValue(const TwoTaskSinlgePermutation& perm1,
                          const TwoTaskSinlgePermutation& perm2) {
    if (perm1.lower_bound_valid_ && perm2.lower_bound_valid_)
        return std::max(perm1.lower_bound_, perm2.lower_bound_);
    else if (perm1.lower_bound_valid_)
        return perm1.lower_bound_;
    else if (perm2.lower_bound_valid_)
        return perm2.lower_bound_;
    else
        return -1e9;
}

int MergeLargerThanValue(const TwoTaskSinlgePermutation& perm1,
                         const TwoTaskSinlgePermutation& perm2) {
    if (perm1.upper_bound_valid_ && perm2.upper_bound_valid_)
        return std::min(perm1.upper_bound_, perm2.upper_bound_);
    else if (perm1.upper_bound_valid_)
        return perm1.upper_bound_;
    else if (perm2.upper_bound_valid_)
        return perm2.upper_bound_;
    else
        return 1e9;
}

TwoTaskSinlgePermutation MergeTwoSinglePermutations(
    const TwoTaskSinlgePermutation& perm1,
    const TwoTaskSinlgePermutation& perm2) {
    TwoTaskSinlgePermutation merged_perm;

    if (ExamConfliction(perm1, perm2) ||
        perm1.task_prev_id_ != perm2.task_prev_id_ ||
        perm1.task_next_id_ != perm2.task_next_id_) {
        merged_perm.setInvalid();
    } else {
        int smaller_value_merged = MergeSmallerThanValue(perm1, perm2);
        int larger_value_merged = MergeLargerThanValue(perm1, perm2);

        merged_perm = TwoTaskSinlgePermutation(
            perm1.task_prev_id_, perm1.task_next_id_, smaller_value_merged,
            perm1.lower_bound_valid_ || perm2.lower_bound_valid_,
            larger_value_merged,
            perm1.upper_bound_valid_ || perm2.upper_bound_valid_);
    }
    return merged_perm;
}

}  // namespace DAG_SPACE