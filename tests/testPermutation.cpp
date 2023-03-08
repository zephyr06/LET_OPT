#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/TwoTaskSinglePermutation.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Interval.h"
#include "sources/Utils/JobCEC.h"
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

inline int GetSuperPeriod(const Task& task1, const Task& task2) {
    return std::lcm(task1.period, task2.period);
}

std::vector<JobCEC> GetPossibleReactingJobs(const JobCEC& jobCurr,
                                            const Task& task_next) {
    std::vector<JobCEC> reactingJobs;
    return reactingJobs;
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