#include "sources/Baseline/StandardLET.h"

namespace DAG_SPACE {
PermutationInequality GetPermIneq(const DAG_Model& dag_tasks,
                                  const TaskSetInfoDerived& tasks_info,
                                  int prev_task_id, int next_task_id) {
    return PermutationInequality(prev_task_id, next_task_id);
}

// int FindFirstReactJob(const Task& task_prev, const Task& task_next,
//                       int prev_task_index) {
//     return std::ceil(
//         float(prev_task_index * task_prev.period + task_prev.deadline) /
//         task_next.period);
// }

std::vector<JobCEC> GetPossibleReactingJobsLET(
    const JobCEC& job_curr, const Task& task_next, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info) {
    int job_finish_curr = GetDeadline(job_curr, tasks_info);
    int period_next = tasks_info.GetTask(task_next.id).period;
    return {
        JobCEC(task_next.id, std::ceil(float(job_finish_curr) / period_next))};
}

std::unordered_map<JobCEC, std::vector<JobCEC>> GetJobMatch(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    int prev_task_id, int next_task_id) {
    const TaskSet& tasks = dag_tasks.GetTaskSet();

    Task task_prev = tasks[dag_tasks.GetTaskIndex(prev_task_id)];
    Task task_next = tasks[dag_tasks.GetTaskIndex(next_task_id)];
    int super_period = GetSuperPeriod(task_prev, task_next);

    std::unordered_map<JobCEC, std::vector<JobCEC>> job_matches;
    for (int i = 0; i < super_period / task_prev.period; i++) {
        JobCEC job_curr(prev_task_id, i);
        std::vector<JobCEC> jobs_possible_match = GetPossibleReactingJobsLET(
            job_curr, task_next, super_period, tasks_info);
        job_matches[job_curr] = jobs_possible_match;
    }
    return job_matches;
}

SinglePairPermutation GetSinglePermutationStanLET(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    int prev_task_id, int next_task_id) {
    return SinglePairPermutation(
        GetPermIneq(dag_tasks, tasks_info, prev_task_id, next_task_id),
        GetJobMatch(dag_tasks, tasks_info, prev_task_id, next_task_id));
}

ChainPermutation GetStandardLETChain(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    const std::vector<std::vector<int>>& task_chains) {
    ChainPermutation chain_perm(1e2);
    for (const auto& chain : task_chains) {
        for (uint i = 0; i < chain.size() - 1; i++) {
            chain_perm.push_back(GetSinglePermutationStanLET(
                dag_tasks, tasks_info, chain[i], chain[i + 1]));
        }
    }

    return chain_perm;
}

}  // namespace DAG_SPACE