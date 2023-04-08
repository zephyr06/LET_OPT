#include "sources/Baseline/StandardLET.h"

namespace DAG_SPACE {
PermutationInequality GetPermIneq(const DAG_Model& dag_tasks,
                                  const TaskSetInfoDerived& tasks_info,
                                  int prev_task_id, int next_task_id,
                                  const std::string& type_trait) {
  return PermutationInequality(prev_task_id, next_task_id, type_trait);
}

// int FindFirstReactJob(const Task& task_prev, const Task& task_next,
//                       int prev_task_index) {
//     return std::ceil(
//         float(prev_task_index * task_prev.period + task_prev.deadline) /
//         task_next.period);
// }

JobCEC GetPossibleReactingJobsLET(
    const JobCEC& job_curr, const Task& task_next, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info) {
  int job_finish_curr = GetDeadline(job_curr, tasks_info);
  int period_next = tasks_info.GetTask(task_next.id).period;
  return JobCEC(task_next.id, std::ceil(float(job_finish_curr) / period_next));
}

// TODO: add DA support
std::unordered_map<JobCEC, JobCEC> GetJobMatch(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    int prev_task_id, int next_task_id, const std::string& type_trait) {
  const TaskSet& tasks = dag_tasks.GetTaskSet();

  Task task_prev = tasks[dag_tasks.GetTaskIndex(prev_task_id)];
  Task task_next = tasks[dag_tasks.GetTaskIndex(next_task_id)];
  int super_period = GetSuperPeriod(task_prev, task_next);

  std::unordered_map<JobCEC, JobCEC> job_matches;
  for (int i = 0; i < super_period / task_prev.period; i++) {
    JobCEC job_curr(prev_task_id, i);
    JobCEC jobs_possible_match = GetPossibleReactingJobsLET(
        job_curr, task_next, super_period, tasks_info);
    job_matches[job_curr] = jobs_possible_match;
  }
  return job_matches;
}

SinglePairPermutation GetSinglePermutationStanLET(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    int prev_task_id, int next_task_id, const std::string& type_trait) {
  return SinglePairPermutation(GetPermIneq(dag_tasks, tasks_info, prev_task_id,
                                           next_task_id, type_trait),
                               GetJobMatch(dag_tasks, tasks_info, prev_task_id,
                                           next_task_id, type_trait),
                               type_trait);
}

ChainsPermutation GetStandardLETChain(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    const std::vector<std::vector<int>>& task_chains,
    const std::string& type_trait) {
  ChainsPermutation chains_perm(1e2);
  for (const auto& chain : task_chains) {
    for (uint i = 0; i < chain.size() - 1; i++) {
      chains_perm.push_back(std::make_shared<const SinglePairPermutation>(
          GetSinglePermutationStanLET(dag_tasks, tasks_info, chain[i],
                                      chain[i + 1], type_trait)));
    }
  }

  return chains_perm;
}

}  // namespace DAG_SPACE