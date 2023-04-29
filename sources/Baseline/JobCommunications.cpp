#include "sources/Baseline/JobCommunications.h"

#include "sources/ObjectiveFunction/ObjectiveFunction.h"
namespace DAG_SPACE {

JobCEC GetReactingJob(const JobCEC& job_curr, const Task& task_next,
                      int superperiod,
                      const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
                      const Schedule& schedule) {
  int job_finish_curr = GetFinishTime(job_curr, schedule, tasks_info);
  int period_next = tasks_info.GetTask(task_next.id).period;
  JobCEC react_job_prev(task_next.id,
                        std::floor(float(job_finish_curr) / period_next));
  if (GetStartTime(react_job_prev, schedule, tasks_info) >= job_finish_curr)
    return react_job_prev;
  else
    return JobCEC(task_next.id, react_job_prev.jobId + 1);
}

JobCEC GetReadingJob(const JobCEC& job_curr, const Task& task_prev,
                     int superperiod,
                     const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
                     const Schedule& schedule) {
  int job_start_curr = GetStartTime(job_curr, schedule, tasks_info);
  int period_prev = tasks_info.GetTask(task_prev.id).period;
  JobCEC possible_read_job(
      task_prev.id, std::floor(float(job_start_curr) / period_prev) - 1 - 1);
  int job_id_try_upper_bound = possible_read_job.jobId + 10;
  for (int job_id = possible_read_job.jobId; job_id < job_id_try_upper_bound;
       job_id++) {
    JobCEC job_curr_try(task_prev.id, job_id);
    int prev_job_finish = GetFinishTime(job_curr_try, schedule, tasks_info);
    if (prev_job_finish <= job_start_curr) {
      possible_read_job = job_curr_try;
    } else {
      if (GetFinishTime(possible_read_job, schedule, tasks_info) <=
          job_start_curr)
        return possible_read_job;
      else
        CoutError("Didn't find reading job in GetPossibleReadingJobs!");
    }
  }
  CoutError("didn't find reading job!");
  return possible_read_job;
}

std::unordered_map<JobCEC, JobCEC> GetJobMatch(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    int prev_task_id, int next_task_id, const std::string& type_trait,
    const Schedule& schedule) {
  const TaskSet& tasks = dag_tasks.GetTaskSet();

  Task task_prev = tasks[dag_tasks.GetTaskIndex(prev_task_id)];
  Task task_next = tasks[dag_tasks.GetTaskIndex(next_task_id)];
  int super_period = GetSuperPeriod(task_prev, task_next);

  std::unordered_map<JobCEC, JobCEC> job_matches;
  if (IfRT_Trait(type_trait)) {
    for (int i = 0; i < super_period / task_prev.period; i++) {
      JobCEC job_curr(prev_task_id, i);
      JobCEC jobs_possible_match = GetReactingJob(
          job_curr, task_next, super_period, tasks_info, schedule);
      job_matches[job_curr] = jobs_possible_match;
    }
  } else if (IfDA_Trait(type_trait)) {
    for (int i = 0; i < super_period / task_next.period; i++) {
      JobCEC job_curr(next_task_id, i);
      JobCEC jobs_possible_match = GetReadingJob(
          job_curr, task_prev, super_period, tasks_info, schedule);
      job_matches[job_curr] = jobs_possible_match;
    }
  } else
    CoutError("Unknown type trait in BatchOptimize!");

  return job_matches;
}

ChainsPermutation GetChainsPermFromVariable(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    const std::vector<std::vector<int>>& task_chains,
    const std::string& type_trait, const Schedule& schedule) {
  ChainsPermutation chains_perm(1e2);
  for (const auto& chain : task_chains) {
    for (uint i = 0; i < chain.size() - 1; i++) {
      chains_perm.push_back(std::make_shared<const SinglePairPermutation>(
          GetSinglePermFromVariable(dag_tasks, tasks_info, chain[i],
                                    chain[i + 1], type_trait, schedule)));
    }
  }

  return chains_perm;
}
}  // namespace DAG_SPACE