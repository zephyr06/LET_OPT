#include "JobCEC.h"

namespace DAG_SPACE {

std::vector<std::pair<std::pair<double, double>, JobCEC>> SortJobSchedule(
    std::vector<std::pair<std::pair<double, double>, JobCEC>> &timeJobVector) {
    std::sort(timeJobVector.begin(), timeJobVector.end(),
              [](auto a, auto b) { return a.first.first < b.first.first; });
    return timeJobVector;
}

// map the job to the first hyper period and return job's unique id
LLint GetJobUniqueId(const JobCEC &jobCEC,
                     const RegularTaskSystem::TaskSetInfoDerived &tasksInfo) {
    LLint id = jobCEC.jobId % tasksInfo.GetVariableSize(jobCEC.taskId);
    for (int i = 0; i < jobCEC.taskId; i++) {
        id += tasksInfo.GetVariableSize(i);
    }
    return id;
}

LLint GetJobUniqueIdWithinHyperPeriod(
    const JobCEC &jobCEC,
    const RegularTaskSystem::TaskSetInfoDerived &tasksInfo) {
    if (jobCEC.jobId >= tasksInfo.GetVariableSize(jobCEC.taskId)) {
        JobCEC jobWithinHyperPeriod(
            jobCEC.taskId,
            jobCEC.jobId % tasksInfo.GetVariableSize(jobCEC.taskId));
        return GetJobUniqueId(jobWithinHyperPeriod, tasksInfo);
    } else {
        return GetJobUniqueId(jobCEC, tasksInfo);
    }
}

}  // namespace DAG_SPACE
