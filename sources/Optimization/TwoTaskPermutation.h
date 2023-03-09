#pragma once

#include "sources/Optimization/PermutationInequality.h"

namespace DAG_SPACE {

inline int GetSuperPeriod(const Task& task1, const Task& task2) {
    return std::lcm(task1.period, task2.period);
}

std::vector<JobCEC> GetPossibleReactingJobs(
    const JobCEC& job_curr, const Task& task_next, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasksInfo);

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

    bool AddMatchJobPair(const JobCEC& job_curr, const JobCEC& job_match);

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

    bool AppendJobs(const JobCEC& job_curr, const JobCEC& job_match,
                    PermutationTwoTask& permutation_current);

    void AppendAllPermutations(const JobCEC& job_curr,
                               PermutationTwoTask& permutation_current);

    void FindAllPermutations();
    // data members
    Task task_prev_;
    Task task_next_;
    RegularTaskSystem::TaskSetInfoDerived tasks_info_;
    int superperiod_;
    std::vector<PermutationTwoTask> single_permutations_;
};
}  // namespace DAG_SPACE