#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/TwoTaskSinglePermutation.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Interval.h"
#include "sources/Utils/JobCEC.h"
using namespace DAG_SPACE;

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

std::vector<JobCEC> GetPossibleReactingJobs(
    const JobCEC& job_curr, const Task& task_next, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasksInfo) {
    int job_min_finish = GetActivationTime(job_curr, tasksInfo) +
                         GetExecutionTime(job_curr, tasksInfo);
    int job_max_finish = GetDeadline(job_curr, tasksInfo);
    int period_next = tasksInfo.tasks[task_next.id].period;
    std::vector<JobCEC> reactingJobs;
    reactingJobs.reserve(superperiod / tasksInfo.tasks[job_curr.taskId].period);
    for (int i = std::floor(float(job_min_finish) / period_next);
         i <= std::ceil(float(job_max_finish) / period_next); i++)
        reactingJobs.push_back(JobCEC(task_next.id, i));

    return reactingJobs;
}

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
        task0 = tasks[0];
        task1 = tasks[1];
        task2 = tasks[2];
    };

    DAG_Model dagTasks;
    TaskSet tasks;
    TaskSetInfoDerived tasksInfo;
    std::vector<int> chain1;
    Task task0;
    Task task1;
    Task task2;
};

TEST_F(PermutationTest1, GetPossibleReactingJobs_same_period) {
    int superperiod = GetSuperPeriod(task1, task2);
    auto reacting_jobs =
        GetPossibleReactingJobs(JobCEC(1, 0), task2, superperiod, tasksInfo);
    EXPECT_EQ(2, reacting_jobs.size());
    EXPECT_EQ(0, reacting_jobs[0].jobId);
    EXPECT_EQ(1, reacting_jobs[1].jobId);
}
TEST_F(PermutationTest1, GetPossibleReactingJobs_harmonic_period) {
    int superperiod = GetSuperPeriod(task0, task1);
    auto reacting_jobs =
        GetPossibleReactingJobs(JobCEC(0, 0), task1, superperiod, tasksInfo);
    EXPECT_EQ(2, reacting_jobs.size());
    EXPECT_EQ(0, reacting_jobs[0].jobId);
    EXPECT_EQ(1, reacting_jobs[1].jobId);

    reacting_jobs =
        GetPossibleReactingJobs(JobCEC(1, 0), task0, superperiod, tasksInfo);
    EXPECT_EQ(3, reacting_jobs.size());
    EXPECT_EQ(0, reacting_jobs[0].jobId);
    EXPECT_EQ(2, reacting_jobs[reacting_jobs.size() - 1].jobId);

    reacting_jobs =
        GetPossibleReactingJobs(JobCEC(0, 1), task1, superperiod, tasksInfo);
    EXPECT_EQ(2, reacting_jobs.size());
    EXPECT_EQ(0, reacting_jobs[0].jobId);
    EXPECT_EQ(1, reacting_jobs[1].jobId);
}

class PermutationTest3 : public ::testing::Test {
   protected:
    void SetUp() override {
        dagTasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v2.csv",
            "orig", 1);
        tasks = dagTasks.tasks;
        tasksInfo = TaskSetInfoDerived(tasks);
        task0 = tasks[0];
        task1 = tasks[1];
        task2 = tasks[2];
    };

    DAG_Model dagTasks;
    TaskSet tasks;
    TaskSetInfoDerived tasksInfo;
    Task task0;
    Task task1;
    Task task2;
};

TEST_F(PermutationTest3, GetPossibleReactingJobs_non_harmonic_period) {
    int superperiod = GetSuperPeriod(task0, task1);
    auto reacting_jobs =
        GetPossibleReactingJobs(JobCEC(0, 0), task1, superperiod, tasksInfo);
    EXPECT_EQ(2, reacting_jobs.size());
    EXPECT_EQ(0, reacting_jobs[0].jobId);
    EXPECT_EQ(1, reacting_jobs[1].jobId);

    reacting_jobs =
        GetPossibleReactingJobs(JobCEC(0, 1), task1, superperiod, tasksInfo);
    EXPECT_EQ(3, reacting_jobs.size());
    EXPECT_EQ(0, reacting_jobs[0].jobId);
    EXPECT_EQ(2, reacting_jobs.back().jobId);

    reacting_jobs =
        GetPossibleReactingJobs(JobCEC(0, 2), task1, superperiod, tasksInfo);
    EXPECT_EQ(2, reacting_jobs.size());
    EXPECT_EQ(1, reacting_jobs[0].jobId);
    EXPECT_EQ(2, reacting_jobs[1].jobId);
}

// class TwoTaskPermutation {
//    public:
//     TwoTaskPermutation(const Task& task_prev, const Task& task_next)
//         : task_prev_(task_prev), task_next_(task_next) {}

//     inline size_t size() const { return singlePermutations_.size(); }

//     // data members
//     Task task_prev_;
//     Task task_next_;
//     std::vector<TwoTaskSinlgePermutation> singlePermutations_;
// };

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