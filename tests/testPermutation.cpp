#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Interval.h"
using namespace DAG_SPACE;

class PermutationTest1 : public ::testing::Test {
   protected:
    void SetUp() override {
        dagTasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v18.csv",
            "orig", 1);
        tasks = dagTasks.tasks;
        tasksInfo = TaskSetInfoDerived(tasks);
        chain1 = {0, 2};
        dagTasks.chains_[0] = chain1;
    };

    DAG_Model dagTasks;
    TaskSet tasks;
    TaskSetInfoDerived tasksInfo;
    std::vector<int> chain1;
};

class TaskSetPermutation {};

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

    bool operator==(const TwoTaskSinlgePermutation& other) const {
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

class PermutationTest2 : public ::testing::Test {
   protected:
    void SetUp() override {
        perm1 = TwoTaskSinlgePermutation(0, 1, 0, true, 10, true);
        perm2 = TwoTaskSinlgePermutation(0, 1, 10, true, 30, true);
        perm3 = TwoTaskSinlgePermutation(0, 1, 0, true, 5, true);
        perm4 = TwoTaskSinlgePermutation(0, 1, 11, true, 20, true);
        perm5 = TwoTaskSinlgePermutation(0, 1, 5, true, 20, true);

        perm6 = TwoTaskSinlgePermutation(0, 1, 11, false, 20, true);
        perm7 = TwoTaskSinlgePermutation(0, 1, 5, false, 20, true);
        perm8 = TwoTaskSinlgePermutation(0, 1, 30, true, 20, false);
        perm9 = TwoTaskSinlgePermutation(0, 1, 5, true, 15, false);
    };

    TwoTaskSinlgePermutation perm1;
    TwoTaskSinlgePermutation perm2;
    TwoTaskSinlgePermutation perm3;
    TwoTaskSinlgePermutation perm4;
    TwoTaskSinlgePermutation perm5;
    TwoTaskSinlgePermutation perm6;
    TwoTaskSinlgePermutation perm7;
    TwoTaskSinlgePermutation perm8;
    TwoTaskSinlgePermutation perm9;
};

;

/**
 * @brief
 *
 * @param perm1
 * @param perm2
 * @return true: perm1 and perm2 have confliction
 * @return false: no confliction
 */
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

TEST_F(PermutationTest2, ExamConfliction_and_WhetherAdjacent) {
    EXPECT_FALSE(ExamConfliction(perm1, perm2));
    EXPECT_FALSE(ExamConfliction(perm2, perm1));
    EXPECT_FALSE(ExamConfliction(perm4, perm2));
    EXPECT_TRUE(ExamConfliction(perm4, perm3));
    EXPECT_TRUE(ExamConfliction(perm1, perm4));

    EXPECT_FALSE(ExamConfliction(perm6, perm7));
    EXPECT_FALSE(ExamConfliction(perm6, perm7));
    EXPECT_TRUE(ExamConfliction(perm8, perm7));
}

int MergeSmallerThanValue(const TwoTaskSinlgePermutation& perm1,
                          const TwoTaskSinlgePermutation& perm2) {
    if (perm1.prev_const_valid_ && perm2.prev_const_valid_)
        return std::max(perm1.smaller_than_value_, perm2.smaller_than_value_);
    else if (perm1.prev_const_valid_)
        return perm1.smaller_than_value_;
    else if (perm2.prev_const_valid_)
        return perm2.smaller_than_value_;
    else
        return -1e9;
}

int MergeLargerThanValue(const TwoTaskSinlgePermutation& perm1,
                         const TwoTaskSinlgePermutation& perm2) {
    if (perm1.next_const_valid_ && perm2.next_const_valid_)
        return std::min(perm1.larger_than_value_, perm2.larger_than_value_);
    else if (perm1.next_const_valid_)
        return perm1.larger_than_value_;
    else if (perm2.next_const_valid_)
        return perm2.larger_than_value_;
    else
        return 1e9;
}

TwoTaskSinlgePermutation MergeSinglePermutation(
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
            perm1.prev_const_valid_ || perm2.prev_const_valid_,
            larger_value_merged,
            perm1.next_const_valid_ || perm2.next_const_valid_);
    }
    return merged_perm;
}

TEST_F(PermutationTest2, MergeSingleValue) {
    EXPECT_EQ(10, MergeSmallerThanValue(perm1, perm2));
    EXPECT_EQ(5, MergeSmallerThanValue(perm6, perm9));
    EXPECT_EQ(-1e9, MergeSmallerThanValue(perm6, perm7));

    EXPECT_EQ(10, MergeLargerThanValue(perm1, perm2));
    EXPECT_EQ(20, MergeLargerThanValue(perm6, perm9));
    EXPECT_EQ(1e9, MergeLargerThanValue(perm8, perm9));
}

TEST_F(PermutationTest2, MergeSinglePermutation) {
    TwoTaskSinlgePermutation merged_perm = MergeSinglePermutation(perm1, perm2);
    EXPECT_TRUE(merged_perm.IsValid());
    EXPECT_EQ(perm1.task_prev_id_, merged_perm.task_prev_id_);
    EXPECT_EQ(perm1.task_next_id_, merged_perm.task_next_id_);
    EXPECT_EQ(10, merged_perm.smaller_than_value_);
    EXPECT_EQ(10, merged_perm.larger_than_value_);

    merged_perm = MergeSinglePermutation(perm1, perm3);
    EXPECT_TRUE(merged_perm.IsValid());
    EXPECT_EQ(0, merged_perm.smaller_than_value_);
    EXPECT_EQ(5, merged_perm.larger_than_value_);

    merged_perm = MergeSinglePermutation(perm1, perm4);
    EXPECT_FALSE(merged_perm.IsValid());

    merged_perm = MergeSinglePermutation(perm1, perm5);
    EXPECT_TRUE(merged_perm.IsValid());
    EXPECT_EQ(5, merged_perm.smaller_than_value_);
    EXPECT_EQ(10, merged_perm.larger_than_value_);

    merged_perm = MergeSinglePermutation(perm6, perm7);
    EXPECT_TRUE(merged_perm.IsValid());
    EXPECT_FALSE(merged_perm.prev_const_valid_);
    // EXPECT_EQ(-1e9, merged_perm.smaller_than_value_);
    EXPECT_EQ(20, merged_perm.larger_than_value_);

    merged_perm = MergeSinglePermutation(perm6, perm8);
    EXPECT_FALSE(merged_perm.IsValid());

    merged_perm = MergeSinglePermutation(perm6, perm9);
    EXPECT_TRUE(merged_perm.IsValid());
    EXPECT_EQ(5, merged_perm.smaller_than_value_);
    EXPECT_EQ(20, merged_perm.larger_than_value_);
}

class TwoTaskPermutation {
   public:
    TwoTaskPermutation(const Task& task_prev, const Task& task_next)
        : task_prev_(task_prev), task_next_(task_next) {}

    inline size_t size() const { return singlePermutations_.size(); }

    // data members
    Task task_prev_;
    Task task_next_;
    std::vector<TwoTaskSinlgePermutation> singlePermutations_;
};

// TEST_F(PermutationTest1, simple_contructor_harmonic) {
//     TwoTaskPermutation two_task_permutation(tasks[1], tasks[2]);
//     EXPECT_EQ(2, two_task_permutation.size());
//     TwoTaskSinlgePermutation perm_expected0(1, 2, 0, false, 0, true);
//     TwoTaskSinlgePermutation perm_expected1(1, 2, 0, false, 20, true);
// }

int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}