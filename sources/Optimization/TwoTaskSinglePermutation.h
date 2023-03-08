#pragma once
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Interval.h"
namespace DAG_SPACE {

// permutation is described by
//      o_{task_next_id} + smaller_than_value <= d_{task_prev_id} ;
//      d_{task_prev_id} <= o_{task_next_id} + larger_than_value ;
// This is the same as the following in the paper:
// o_{i+1} + x < d_i < o_{i+1} + y
// Currently, this struct only saves the first-reaction relationship, that
// means, if J_{11} triggers J_{22}, then J_{11} cannot trigger J_{21}
class TwoTaskSinlgePermutation {
   public:
    TwoTaskSinlgePermutation() {}
    TwoTaskSinlgePermutation(int task_prev_id, int task_next_id,
                             int smaller_than_value, bool prev_const_valid,
                             int larger_than_value, bool next_const_valid)
        : task_prev_id_(task_prev_id),
          task_next_id_(task_next_id),
          smaller_than_value_(smaller_than_value),
          prev_const_valid_(prev_const_valid),
          larger_than_value_(larger_than_value),
          next_const_valid_(next_const_valid) {
        if (larger_than_value < smaller_than_value && prev_const_valid &&
            next_const_valid)
            CoutError(
                "Invalid arguments in TwoTaskSinlgePermutation's constructor!");
    }

    inline bool operator==(const TwoTaskSinlgePermutation& other) const {
        return prev_const_valid_ == other.prev_const_valid_ &&
               next_const_valid_ == other.next_const_valid_ &&
               task_prev_id_ == other.task_prev_id_ &&
               task_next_id_ == other.task_next_id_ &&
               smaller_than_value_ == other.smaller_than_value_ &&
               larger_than_value_ == other.larger_than_value_;
    }
    bool operator!=(const TwoTaskSinlgePermutation& other) const {
        return !((*this) == other);
    }

    inline void setInvalid() {
        prev_const_valid_ = false;
        next_const_valid_ = false;
    }

    inline bool IsValid() const {
        return next_const_valid_ == true || prev_const_valid_ == true;
    }

    inline Interval GetInterval() const {
        int lower_bound = -1e9, upper_bound = 1e9;
        if (prev_const_valid_) lower_bound = smaller_than_value_;
        if (next_const_valid_) upper_bound = larger_than_value_;
        return Interval(lower_bound, upper_bound - lower_bound);
    }

    // data member
    int task_prev_id_;
    int task_next_id_;
    int smaller_than_value_;
    bool prev_const_valid_;
    int larger_than_value_;
    bool next_const_valid_;
};

/**
 * @brief
 *
 * @param perm1
 * @param perm2
 * @return true: perm1 and perm2 have confliction
 * @return false: no confliction
 */
bool ExamConfliction(const TwoTaskSinlgePermutation& perm1,
                     const TwoTaskSinlgePermutation& perm2);

int MergeSmallerThanValue(const TwoTaskSinlgePermutation& perm1,
                          const TwoTaskSinlgePermutation& perm2);

int MergeLargerThanValue(const TwoTaskSinlgePermutation& perm1,
                         const TwoTaskSinlgePermutation& perm2);

TwoTaskSinlgePermutation MergeSinglePermutation(
    const TwoTaskSinlgePermutation& perm1,
    const TwoTaskSinlgePermutation& perm2);

}  // namespace DAG_SPACE