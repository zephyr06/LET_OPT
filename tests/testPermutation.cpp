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
          larger_than_value_(larger_than_value) {
        if (larger_than_value < smaller_than_value)
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
    inline bool valid() const {
        return next_const_valid_ == false && prev_const_valid_ == false;
    }

    inline Interval GetInterval() const {
        int lower_bound = -1e8, upper_bound = 1e8;
        if (prev_const_valid_) lower_bound = smaller_than_value_;
        if (upper_bound) upper_bound = larger_than_value_;
        return Interval(lower_bound, upper_bound - lower_bound);
    }

    // data member
    int task_prev_id_;
    int task_next_id_;
    int smaller_than_value_;
    int larger_than_value_;
    bool prev_const_valid_ = true;
    bool next_const_valid_ = true;
};

class PermutationTest2 : public ::testing::Test {
   protected:
    void SetUp() override {
        perm1 = TwoTaskSinlgePermutation(0, 1, 0, true, 10, true);
        perm2 = TwoTaskSinlgePermutation(0, 1, 10, true, 30, true);
        perm3 = TwoTaskSinlgePermutation(0, 1, 0, true, 5, true);
        perm4 = TwoTaskSinlgePermutation(0, 1, 11, true, 20, true);
        perm5 = TwoTaskSinlgePermutation(0, 1, 5, true, 20, true);
    };

    TwoTaskSinlgePermutation perm1;
    TwoTaskSinlgePermutation perm2;
    TwoTaskSinlgePermutation perm3;
    TwoTaskSinlgePermutation perm4;
    TwoTaskSinlgePermutation perm5;
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
}

TwoTaskSinlgePermutation MergeSinglePermutation(
    const TwoTaskSinlgePermutation& perm1,
    const TwoTaskSinlgePermutation& perm2) {
    TwoTaskSinlgePermutation merged_perm;

    if (ExamConfliction(perm1, perm2)) {
        merged_perm.setInvalid();
    } else {
    }
    return merged_perm;
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

TEST_F(PermutationTest1, simple_contructor_harmonic) {
    TwoTaskPermutation two_task_permutation(tasks[1], tasks[2]);
    EXPECT_EQ(2, two_task_permutation.size());
    TwoTaskSinlgePermutation perm_expected0(1, 2, 0, false, 0, true);
    TwoTaskSinlgePermutation perm_expected1(1, 2, 0, false, 20, true);
}

int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}