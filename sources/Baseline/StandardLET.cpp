#include "sources/Baseline/StandardLET.h"

namespace StandardLET {
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
    int period_next = tasks_info.tasks[task_next.id].period;
    return {
        JobCEC(task_next.id, std::ceil(float(job_finish_curr) / period_next))};
}

std::unordered_map<JobCEC, std::vector<JobCEC>> GetJobMatch(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    int prev_task_id, int next_task_id) {
    const TaskSet& tasks = dag_tasks.tasks;

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

SinglePairPermutation GetSinglePermutation(const DAG_Model& dag_tasks,
                                           const TaskSetInfoDerived& tasks_info,
                                           int prev_task_id, int next_task_id) {
    return SinglePairPermutation(
        GetPermIneq(dag_tasks, tasks_info, prev_task_id, next_task_id),
        GetJobMatch(dag_tasks, tasks_info, prev_task_id, next_task_id));
}

ChainPermutation GetStandardLETChain(const DAG_Model& dag_tasks,
                                     const TaskSetInfoDerived& tasks_info,
                                     std::vector<int> task_chain) {
    ChainPermutation chain_perm(task_chain.size() - 1);
    for (uint i = 0; i < task_chain.size() - 1; i++) {
        chain_perm.push_back(GetSinglePermutation(
            dag_tasks, tasks_info, task_chain[i], task_chain[i + 1]));
    }
    return chain_perm;
}

}  // namespace StandardLET