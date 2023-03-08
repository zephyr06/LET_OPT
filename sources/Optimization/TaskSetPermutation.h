#pragma once

#include "sources/Optimization/PermutationInequality.h"
#include "sources/Utils/JobCEC.h"
namespace DAG_SPACE {

inline int GetSuperPeriod(const Task& task1, const Task& task2) {
    return std::lcm(task1.period, task2.period);
}

std::vector<JobCEC> GetPossibleReactingJobs(
    const JobCEC& job_curr, const Task& task_next, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasksInfo);

}  // namespace DAG_SPACE