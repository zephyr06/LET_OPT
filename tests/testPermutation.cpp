#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/PermutationInequality.h"
#include "sources/Optimization/TaskSetPermutation.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Interval.h"
#include "sources/Utils/JobCEC.h"
using namespace DAG_SPACE;

class TaskSetPermutation {};

class PermutationTest2 : public ::testing::Test {
   protected:
    void SetUp() override {
        perm1 = PermutationInequality(0, 1, 0, true, 10, true);
        perm2 = PermutationInequality(0, 1, 10, true, 30, true);
        perm3 = PermutationInequality(0, 1, 0, true, 5, true);
        perm4 = PermutationInequality(0, 1, 11, true, 20, true);
        perm5 = PermutationInequality(0, 1, 5, true, 20, true);

        perm6 = PermutationInequality(0, 1, 11, false, 20, true);
        perm7 = PermutationInequality(0, 1, 5, false, 20, true);
        perm8 = PermutationInequality(0, 1, 30, true, 20, false);
        perm9 = PermutationInequality(0, 1, 5, true, 15, false);
    };

    PermutationInequality perm1;
    PermutationInequality perm2;
    PermutationInequality perm3;
    PermutationInequality perm4;
    PermutationInequality perm5;
    PermutationInequality perm6;
    PermutationInequality perm7;
    PermutationInequality perm8;
    PermutationInequality perm9;
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

TEST_F(PermutationTest2, MergeTwoSinglePermutations) {
    PermutationInequality merged_perm =
        MergeTwoSinglePermutations(perm1, perm2);
    EXPECT_TRUE(merged_perm.IsValid());
    EXPECT_EQ(perm1.task_prev_id_, merged_perm.task_prev_id_);
    EXPECT_EQ(perm1.task_next_id_, merged_perm.task_next_id_);
    EXPECT_EQ(10, merged_perm.lower_bound_);
    EXPECT_EQ(10, merged_perm.upper_bound_);

    merged_perm = MergeTwoSinglePermutations(perm1, perm3);
    EXPECT_TRUE(merged_perm.IsValid());
    EXPECT_EQ(0, merged_perm.lower_bound_);
    EXPECT_EQ(5, merged_perm.upper_bound_);

    merged_perm = MergeTwoSinglePermutations(perm1, perm4);
    EXPECT_FALSE(merged_perm.IsValid());

    merged_perm = MergeTwoSinglePermutations(perm1, perm5);
    EXPECT_TRUE(merged_perm.IsValid());
    EXPECT_EQ(5, merged_perm.lower_bound_);
    EXPECT_EQ(10, merged_perm.upper_bound_);

    merged_perm = MergeTwoSinglePermutations(perm6, perm7);
    EXPECT_TRUE(merged_perm.IsValid());
    EXPECT_FALSE(merged_perm.lower_bound_valid_);
    // EXPECT_EQ(-1e9, merged_perm.lower_bound_);
    EXPECT_EQ(20, merged_perm.upper_bound_);

    merged_perm = MergeTwoSinglePermutations(perm6, perm8);
    EXPECT_FALSE(merged_perm.IsValid());

    merged_perm = MergeTwoSinglePermutations(perm6, perm9);
    EXPECT_TRUE(merged_perm.IsValid());
    EXPECT_EQ(5, merged_perm.lower_bound_);
    EXPECT_EQ(20, merged_perm.upper_bound_);
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
        job00 = JobCEC(0, 0);
        job01 = JobCEC(0, 1);
        job10 = JobCEC(1, 0);
        job20 = JobCEC(2, 0);
    };

    DAG_Model dagTasks;
    TaskSet tasks;
    TaskSetInfoDerived tasksInfo;
    std::vector<int> chain1;
    Task task0;
    Task task1;
    Task task2;
    JobCEC job00;
    JobCEC job01;
    JobCEC job10;
    JobCEC job20;
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

TEST_F(PermutationTest1, PermutationInequality_constructor) {
    PermutationInequality perm1(job00, job10, tasksInfo);
    EXPECT_EQ(0, perm1.upper_bound_);
    EXPECT_TRUE(perm1.upper_bound_valid_);
    EXPECT_EQ(-20, perm1.lower_bound_);
    EXPECT_TRUE(perm1.lower_bound_valid_);

    perm1 = PermutationInequality(job01, job10, tasksInfo);
    EXPECT_EQ(-10, perm1.upper_bound_);
    EXPECT_TRUE(perm1.upper_bound_valid_);
    EXPECT_EQ(-30, perm1.lower_bound_);
    EXPECT_TRUE(perm1.lower_bound_valid_);
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

struct PermutationTwoTask {
    PermutationTwoTask() {}

    PermutationTwoTask(PermutationInequality inequality,
                       const RegularTaskSystem::TaskSetInfoDerived& tasks_info)
        : inequality_(inequality) {
        int superperiod =
            GetSuperPeriod(tasks_info.tasks[inequality.task_prev_id_],
                           tasks_info.tasks[inequality.task_next_id_]);
        job_matches_.reserve(superperiod /
                             tasks_info.tasks[inequality.task_prev_id_].period);
    }

    bool AddMatchJobPair(const JobCEC& job_curr, const JobCEC& job_match) {
        auto itr = job_matches_.find(job_curr);
        if (itr == job_matches_.end())
            job_matches_[job_curr] = {job_match};
        else {
            JobCEC last_matched_job = itr->second.back();
            if (job_match.jobId < last_matched_job.jobId) return false;
            job_matches_[job_curr].push_back(job_match);
        }

        return true;
    }

    // data members
    PermutationInequality inequality_;
    std::unordered_map<JobCEC, std::vector<JobCEC>> job_matches_;
};

class TwoTaskPermutation {
   public:
    TwoTaskPermutation(const Task& task_prev, const Task& task_next,
                       const RegularTaskSystem::TaskSetInfoDerived& tasks_info)
        : task_prev_(task_prev),
          task_next_(task_next),
          tasks_info_(tasks_info) {
        superperiod_ = GetSuperPeriod(task_prev, task_next);
        single_permutations_.reserve(1e4);
        FindAllPermutations();
    }

    inline size_t size() const { return single_permutations_.size(); }
    inline PermutationTwoTask operator[](size_t i) {
        if (i >= size()) CoutError("Out-of-range error in TwoTaskPermutation");
        return single_permutations_[i];
    }

    // TODO: more cut can be added by avoiding conflicting job matches
    bool AppendJobs(const JobCEC& job_curr, const JobCEC& job_match,
                    PermutationTwoTask& permutation_current) {
        PermutationInequality perm_new(job_curr, job_match, tasks_info_);
        PermutationInequality perm_merged = MergeTwoSinglePermutations(
            perm_new, permutation_current.inequality_);
        if (perm_merged.IsValid()) {
            permutation_current.inequality_ = perm_merged;
            if (!(permutation_current.AddMatchJobPair(job_curr, job_match)))
                return false;
            return true;
        } else
            return false;
    }

    void AppendAllPermutations(const JobCEC& job_curr,
                               PermutationTwoTask& permutation_current) {
        std::vector<JobCEC> jobs_possible_match = GetPossibleReactingJobs(
            job_curr, task_next_, superperiod_, tasks_info_);

        for (auto job_match : jobs_possible_match) {
            PermutationTwoTask permutation_current_copy = permutation_current;
            if (AppendJobs(job_curr, job_match, permutation_current_copy)) {
                if (job_curr.jobId ==
                    tasks_info_.sizeOfVariables[job_curr.taskId] -
                        1) {  // reach end, record the current permutations
                    single_permutations_.push_back(permutation_current_copy);

                    if (single_permutations_.size() > 1e5)
                        CoutError("Possibly too many permutations!");
                } else {
                    JobCEC job_next(job_curr.taskId, job_curr.jobId + 1);
                    AppendAllPermutations(job_next, permutation_current_copy);
                }
            } else  // skip this match because it's not feasible or useful
                ;
        }
    }

    void FindAllPermutations() {
        JobCEC job_curr(task_prev_.id, 0);
        PermutationInequality perm_ineq(task_prev_.id, task_next_.id);
        PermutationTwoTask single_permutation(perm_ineq, tasks_info_);
        AppendAllPermutations(job_curr, single_permutation);
    }

    // data members
    Task task_prev_;
    Task task_next_;
    RegularTaskSystem::TaskSetInfoDerived tasks_info_;
    int superperiod_;
    std::vector<PermutationTwoTask> single_permutations_;
};

TEST_F(PermutationTest1, simple_contructor_harmonic) {
    TwoTaskPermutation two_task_permutation(tasks[1], tasks[2], tasksInfo);
    EXPECT_EQ(2, two_task_permutation.size());
    // PermutationInequality perm_expected0(1, 2, 0, false, 0, true);
    EXPECT_EQ(0, two_task_permutation[0].inequality_.upper_bound_);
    // PermutationInequality perm_expected1(1, 2, 0, false, 20, true);
    EXPECT_EQ(20, two_task_permutation[1].inequality_.upper_bound_);

    two_task_permutation = TwoTaskPermutation(tasks[0], tasks[2], tasksInfo);

    EXPECT_TRUE(JobCEC(2, 0) ==
                two_task_permutation[0].job_matches_[JobCEC(0, 0)][0]);
    EXPECT_TRUE(JobCEC(2, 0) ==
                two_task_permutation[0].job_matches_[JobCEC(0, 1)][0]);
    EXPECT_EQ(3, two_task_permutation.size());
    EXPECT_EQ(-10, two_task_permutation[0].inequality_.upper_bound_);

    EXPECT_TRUE(JobCEC(2, 0) ==
                two_task_permutation[1].job_matches_[JobCEC(0, 0)][0]);
    EXPECT_TRUE(JobCEC(2, 1) ==
                two_task_permutation[1].job_matches_[JobCEC(0, 1)][0]);
    EXPECT_EQ(-10, two_task_permutation[1].inequality_.lower_bound_);
    EXPECT_EQ(0, two_task_permutation[1].inequality_.upper_bound_);

    EXPECT_TRUE(JobCEC(2, 1) ==
                two_task_permutation[2].job_matches_[JobCEC(0, 0)][0]);
    EXPECT_TRUE(JobCEC(2, 1) ==
                two_task_permutation[2].job_matches_[JobCEC(0, 1)][0]);
    EXPECT_EQ(0, two_task_permutation[2].inequality_.lower_bound_);
    EXPECT_EQ(10, two_task_permutation[2].inequality_.upper_bound_);
}

TEST_F(PermutationTest3, simple_contructor_non_harmonic) {
    TwoTaskPermutation two_task_permutation(tasks[0], tasks[1], tasksInfo);
    EXPECT_EQ(5, two_task_permutation.size());

    int permutation_index = 0;
    EXPECT_TRUE(JobCEC(1, 0) ==
                two_task_permutation[0].job_matches_[JobCEC(0, 0)][0]);
    EXPECT_TRUE(JobCEC(1, 0) ==
                two_task_permutation[0].job_matches_[JobCEC(0, 1)][0]);
    EXPECT_TRUE(JobCEC(1, 1) ==
                two_task_permutation[0].job_matches_[JobCEC(0, 2)][0]);
    EXPECT_EQ(-15, two_task_permutation[0].inequality_.lower_bound_);
    EXPECT_EQ(-10, two_task_permutation[0].inequality_.upper_bound_);

    permutation_index++;
    EXPECT_TRUE(
        JobCEC(1, 0) ==
        two_task_permutation[permutation_index].job_matches_[JobCEC(0, 0)][0]);
    EXPECT_TRUE(
        JobCEC(1, 1) ==
        two_task_permutation[permutation_index].job_matches_[JobCEC(0, 1)][0]);
    EXPECT_TRUE(
        JobCEC(1, 1) ==
        two_task_permutation[permutation_index].job_matches_[JobCEC(0, 2)][0]);
    EXPECT_EQ(-10,
              two_task_permutation[permutation_index].inequality_.lower_bound_);
    EXPECT_EQ(-5,
              two_task_permutation[permutation_index].inequality_.upper_bound_);

    permutation_index++;
    EXPECT_TRUE(
        JobCEC(1, 0) ==
        two_task_permutation[permutation_index].job_matches_[JobCEC(0, 0)][0]);
    EXPECT_TRUE(
        JobCEC(1, 1) ==
        two_task_permutation[permutation_index].job_matches_[JobCEC(0, 1)][0]);
    EXPECT_TRUE(
        JobCEC(1, 2) ==
        two_task_permutation[permutation_index].job_matches_[JobCEC(0, 2)][0]);
    EXPECT_EQ(-5,
              two_task_permutation[permutation_index].inequality_.lower_bound_);
    EXPECT_EQ(0,
              two_task_permutation[permutation_index].inequality_.upper_bound_);

    // **********************************************************
    permutation_index++;
    EXPECT_TRUE(
        JobCEC(1, 1) ==
        two_task_permutation[permutation_index].job_matches_[JobCEC(0, 0)][0]);
    EXPECT_TRUE(
        JobCEC(1, 1) ==
        two_task_permutation[permutation_index].job_matches_[JobCEC(0, 1)][0]);
    EXPECT_TRUE(
        JobCEC(1, 2) ==
        two_task_permutation[permutation_index].job_matches_[JobCEC(0, 2)][0]);
    EXPECT_EQ(0,
              two_task_permutation[permutation_index].inequality_.lower_bound_);
    EXPECT_EQ(5,
              two_task_permutation[permutation_index].inequality_.upper_bound_);

    permutation_index++;
    EXPECT_TRUE(
        JobCEC(1, 1) ==
        two_task_permutation[permutation_index].job_matches_[JobCEC(0, 0)][0]);
    EXPECT_TRUE(
        JobCEC(1, 2) ==
        two_task_permutation[permutation_index].job_matches_[JobCEC(0, 1)][0]);
    EXPECT_TRUE(
        JobCEC(1, 2) ==
        two_task_permutation[permutation_index].job_matches_[JobCEC(0, 2)][0]);
    EXPECT_EQ(5,
              two_task_permutation[permutation_index].inequality_.lower_bound_);
    EXPECT_EQ(10,
              two_task_permutation[permutation_index].inequality_.upper_bound_);
}

int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}