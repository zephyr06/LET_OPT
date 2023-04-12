#include "sources/Baseline/JobCommunications.h"

namespace DAG_SPACE {

JobCEC GetPossibleReactingJobs(
    const JobCEC& job_curr, const Task& task_next, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
    const VariableOD& variable_od) {
  int job_finish_curr = GetActivationTime(job_curr, tasks_info) +
                        variable_od.at(job_curr.taskId).deadline;
  int period_next = tasks_info.GetTask(task_next.id).period;
  return JobCEC(task_next.id, std::ceil(float(job_finish_curr) / period_next));
}

JobCEC GetPossibleReadingJobs(
    const JobCEC& job_curr, const Task& task_prev, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
    const VariableOD& variable_od) {
  int job_start_curr = GetActivationTime(job_curr, tasks_info) +
                       variable_od.at(job_curr.taskId).offset;
  int period_prev = tasks_info.GetTask(task_prev.id).period;
  return JobCEC(task_prev.id,
                std::floor(float(job_start_curr) / period_prev) - 1);
}

std::unordered_map<JobCEC, JobCEC> GetJobMatch(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    int prev_task_id, int next_task_id, const std::string& type_trait,
    const VariableOD& variable_od) {
  const TaskSet& tasks = dag_tasks.GetTaskSet();

  Task task_prev = tasks[dag_tasks.GetTaskIndex(prev_task_id)];
  Task task_next = tasks[dag_tasks.GetTaskIndex(next_task_id)];
  int super_period = GetSuperPeriod(task_prev, task_next);

  std::unordered_map<JobCEC, JobCEC> job_matches;
  if (type_trait == "ReactionTimeApprox" || type_trait == "ReactionTime") {
    for (int i = 0; i < super_period / task_prev.period; i++) {
      JobCEC job_curr(prev_task_id, i);
      JobCEC jobs_possible_match = GetPossibleReactingJobs(
          job_curr, task_next, super_period, tasks_info, variable_od);
      job_matches[job_curr] = jobs_possible_match;
    }
  } else if (type_trait == "DataAge" || type_trait == "DataAgeApprox") {
    for (int i = 0; i < super_period / task_next.period; i++) {
      JobCEC job_curr(next_task_id, i);
      JobCEC jobs_possible_match = GetPossibleReadingJobs(
          job_curr, task_prev, super_period, tasks_info, variable_od);
      job_matches[job_curr] = jobs_possible_match;
    }
  } else
    CoutError("Unknown type trait in BatchOptimize!");

  return job_matches;
}
}  // namespace DAG_SPACE