#include "sources/ObjectiveFunction/RTDA_Factor.h"

namespace DAG_SPACE {

RTDA GetMaxRTDA(const std::vector<RTDA> &resVec) {
    RTDA maxRTDA;
    for (const RTDA &item : resVec) {
        maxRTDA.reactionTime =
            std::max(item.reactionTime, maxRTDA.reactionTime);
        maxRTDA.dataAge = std::max(item.dataAge, maxRTDA.dataAge);
    }
    return maxRTDA;
}

std::vector<RTDA> GetRTDAFromSingleJob(const TaskSetInfoDerived &tasksInfo,
                                       const std::vector<int> &causeEffectChain,
                                       const Schedule &shcedule_jobs,
                                       double precision) {
    if (causeEffectChain.size() == 0) {
        return {RTDA(0, 0)};
    }
    LLint hyperPeriod = tasksInfo.hyper_period;
    // const TaskSet &tasks = tasksInfo.GetTaskSet();
    LLint totalStartJobs =
        hyperPeriod / tasksInfo.GetTask(causeEffectChain[0]).period + 1;
    RTDA res;
    std::vector<RTDA> resVec;
    resVec.reserve(totalStartJobs);
    for (LLint i = 0; i < totalStartJobs; i++) {
        resVec.push_back(RTDA{-1, -1});
    }

    std::unordered_map<JobCEC, JobCEC> firstReactionMap;

    for (LLint startInstanceIndex = 0; startInstanceIndex <= totalStartJobs;
         startInstanceIndex++) {
        JobCEC firstJob = {causeEffectChain[0], (startInstanceIndex)};
        for (uint j = 1; j < causeEffectChain.size(); j++) {
            double currentJobFT =
                GetFinishTime(firstJob, shcedule_jobs, tasksInfo);
            LLint jobIndex = 0;
            while (GetStartTime({causeEffectChain[j], jobIndex}, shcedule_jobs,
                                tasksInfo) +
                       precision <
                   currentJobFT) {
                jobIndex++;
                if (jobIndex > 10000) {
                    CoutError("didn't find a match!");
                }
            }
            firstJob = {causeEffectChain[j], jobIndex};
        }

        JobCEC jj(causeEffectChain[0], startInstanceIndex);
        firstReactionMap[jj] = firstJob;
        resVec[startInstanceIndex].reactionTime =
            GetFinishTime(firstJob, shcedule_jobs, tasksInfo) -
            GetStartTime({causeEffectChain[0], startInstanceIndex},
                         shcedule_jobs, tasksInfo);

        // update data age
        JobCEC firstReactLastJob =
            JobCEC{causeEffectChain[0], (startInstanceIndex - 1)};
        if (startInstanceIndex > 0 &&
            firstReactionMap[firstReactLastJob] != firstJob &&
            firstJob.jobId > 0) {
            JobCEC lastReaction = firstJob;
            lastReaction.jobId--;
            resVec[startInstanceIndex - 1].dataAge =
                GetStartTime(lastReaction, shcedule_jobs, tasksInfo) +
                tasksInfo.GetTask(lastReaction.taskId).executionTime -
                GetStartTime({causeEffectChain[0], startInstanceIndex - 1},
                             shcedule_jobs, tasksInfo);
        }
    }
    return resVec;
}

RTDA GetMaxRTDA(const TaskSetInfoDerived &tasksInfo,
                const std::vector<int> &causeEffectChain,
                Schedule &shcedule_jobs) {
    std::vector<RTDA> rtdaVec =
        GetRTDAFromSingleJob(tasksInfo, causeEffectChain, shcedule_jobs);
    RTDA maxRTDA = GetMaxRTDA(rtdaVec);
    return maxRTDA;
}

double ObjRTDA(const RTDA &rtda) { return rtda.reactionTime + rtda.dataAge; }
double ObjRTDA(const std::vector<RTDA> &rtdaVec) {
    double res = 0;
    for (auto &r : rtdaVec) res += ObjRTDA(r);
    return res;
}

std::vector<RTDA> GetMaxRTDAs(const std::vector<std::vector<RTDA>> &rtdaVec) {
    std::vector<RTDA> maxRtda;
    for (uint i = 0; i < rtdaVec.size(); i++)
        maxRtda.push_back(GetMaxRTDA(rtdaVec.at(i)));

    return maxRtda;
}
std::vector<std::vector<RTDA>> GetRTDAFromAllChains(
    const DAG_Model &dagTasks, const TaskSetInfoDerived &tasksInfo,
    const std::vector<std::vector<int>> &chains_to_analyze,
    const Schedule &shcedule_jobs) {
    std::vector<std::vector<RTDA>> rtdaVec;
    for (uint i = 0; i < chains_to_analyze.size(); i++) {
        auto rtdaVecTemp = GetRTDAFromSingleJob(tasksInfo, chains_to_analyze[i],
                                                shcedule_jobs);
        rtdaVec.push_back(rtdaVecTemp);
    }
    return rtdaVec;
}

double ObjDataAgeFromSChedule(
    const DAG_Model &dagTasks, const TaskSetInfoDerived &tasksInfo,
    const std::vector<std::vector<int>> &chains_to_analyze,
    const Schedule &shcedule_jobs) {
    std::vector<std::vector<RTDA>> rtdaVec = GetRTDAFromAllChains(
        dagTasks, tasksInfo, chains_to_analyze, shcedule_jobs);
    std::vector<RTDA> maxRtda = GetMaxRTDAs(rtdaVec);

    double res = 0;
    for (const auto &rtda : maxRtda) {
        res += rtda.dataAge;
    }
    return res;
}
double ObjReactionTimeFromSChedule(
    const DAG_Model &dagTasks, const TaskSetInfoDerived &tasksInfo,
    const std::vector<std::vector<int>> &chains_to_analyze,
    const Schedule &shcedule_jobs) {
    std::vector<std::vector<RTDA>> rtdaVec = GetRTDAFromAllChains(
        dagTasks, tasksInfo, chains_to_analyze, shcedule_jobs);
    std::vector<RTDA> maxRtda = GetMaxRTDAs(rtdaVec);

    double res = 0;
    for (const auto &rtda : maxRtda) {
        res += rtda.reactionTime;
    }
    return res;
}

}  // namespace DAG_SPACE