#pragma once

#include "sources/Optimization/PermutationInequality.h"
#include "sources/Optimization/Variable.h"
#include "sources/Utils/profilier.h"

namespace DAG_SPACE {

inline int GetSuperPeriod(const Task& task1, const Task& task2) {
    return std::lcm(task1.period, task2.period);
}

std::vector<JobCEC> GetPossibleReactingJobs(
    const JobCEC& job_curr, const Task& task_next, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasksInfo);

PermutationInequality GenerateBoxPermutationConstraints(
    int task_prev_id, int task_next_id, const VariableRange& variable_range);

bool ifTimeout(TimerType start_time);

struct SinglePairPermutation {
    SinglePairPermutation() {}

    SinglePairPermutation(
        PermutationInequality inequality,
        const RegularTaskSystem::TaskSetInfoDerived& tasks_info)
        : inequality_(inequality) {
        int superperiod =
            GetSuperPeriod(tasks_info.GetTask(inequality.task_prev_id_),
                           tasks_info.GetTask(inequality.task_next_id_));
        job_first_react_matches_.reserve(
            superperiod / tasks_info.GetTask(inequality.task_prev_id_).period);
    }
    SinglePairPermutation(
        PermutationInequality inequality,
        std::unordered_map<JobCEC, std::vector<JobCEC>> job_first_react_matches)
        : inequality_(inequality),
          job_first_react_matches_(job_first_react_matches) {}

    bool AddMatchJobPair(const JobCEC& job_curr, const JobCEC& job_match);

    void print() const;

    inline int GetPrevTaskId() const { return inequality_.task_prev_id_; }
    inline int GetNextTaskId() const { return inequality_.task_next_id_; }

    // data members
    PermutationInequality inequality_;
    std::unordered_map<JobCEC, std::vector<JobCEC>>
        job_first_react_matches_;  // TODO: there should be only one job in the
                                   // vector, make it happen
};

class TwoTaskPermutations {
   public:
    TwoTaskPermutations() {}
    TwoTaskPermutations(int task_prev_id, int task_next_id,
                        const DAG_Model& dag_tasks,
                        const RegularTaskSystem::TaskSetInfoDerived& tasks_info)
        : start_time_((std::chrono::high_resolution_clock::now())),
          task_prev_id_(task_prev_id),
          task_next_id_(task_next_id),
          tasks_info_(tasks_info) {
        superperiod_ = GetSuperPeriod(tasks_info.GetTask(task_prev_id),
                                      tasks_info.GetTask(task_next_id));
        variable_od_range_ = FindVariableRange(dag_tasks);
        single_permutations_.reserve(1e4);
        FindAllPermutations();
    }

    inline size_t size() const { return single_permutations_.size(); }

    inline SinglePairPermutation operator[](size_t i) {
        if (i >= size()) CoutError("Out-of-range error in TwoTaskPermutations");
        return single_permutations_[i];
    }

    // helper function for AppendAllPermutations during iterations
    bool AppendJobs(const JobCEC& job_curr, const JobCEC& job_match,
                    SinglePairPermutation& permutation_current);

    void AppendAllPermutations(const JobCEC& job_curr,
                               SinglePairPermutation& permutation_current);

    void FindAllPermutations();

    // data members
    TimerType start_time_;
    int task_prev_id_;
    int task_next_id_;
    RegularTaskSystem::TaskSetInfoDerived tasks_info_;
    int superperiod_;
    VariableRange variable_od_range_;
    std::vector<SinglePairPermutation> single_permutations_;
};
}  // namespace DAG_SPACE