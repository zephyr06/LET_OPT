
#include "sources/Optimization/TaskSetPermutation.h"

namespace DAG_SPACE {

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

}  // namespace DAG_SPACE