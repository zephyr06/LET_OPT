#include "sources/Baseline/JobCommunications.h"

#include "sources/ObjectiveFunction/ObjectiveFunction.h"
namespace DAG_SPACE {
// assume no classical offset
JobCEC GetFirstReactJob(const JobCEC& job_curr, const Task& task_next,
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
// this function looks complicated because of the special situation in
// Martinez18TCAD
JobCEC GetLastReadJob(const JobCEC& job_curr, const Task& task_prev,
                      int superperiod,
                      const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
                      const Schedule& schedule) {
  int offset_prev = schedule.at(JobCEC(task_prev.id, 0)).start;
  int offset_curr = schedule.at(JobCEC(job_curr.taskId, 0)).start;

  int job_start_curr = GetStartTime(job_curr, schedule, tasks_info);
  int job_start_with_prev = job_start_curr - offset_prev;

  int period_prev = tasks_info.GetTask(task_prev.id).period;
  JobCEC possible_read(task_prev.id,
                       std::floor(float(job_start_with_prev) / period_prev));
  if (GetFinishTime(possible_read, schedule, tasks_info) <= job_start_curr)
    return possible_read;
  else {
    possible_read.jobId--;
    if (GetFinishTime(possible_read, schedule, tasks_info) <= job_start_curr)
      return possible_read;
    else
      CoutError("didn't find reading job!");
  }

  // int min_possible_read_job_index =
  //     std::floor(float(job_start_curr -
  //                      tasks_info.GetTask(job_curr.taskId).period -
  //                      period_prev) /
  //                period_prev) -
  //     1;  // -1 because this is last reading job
  // int job_id_try_upper_bound = min_possible_read_job_index + 2e3;
  // JobCEC job_last_read(task_prev.id, job_id_try_upper_bound);
  // // TODO: consider use binary search there?
  // for (int job_id = min_possible_read_job_index;
  //      job_id < job_id_try_upper_bound; job_id++) {
  //   JobCEC job_curr_try(task_prev.id, job_id);
  //   int job_curr_try_finish = GetFinishTime(job_curr_try, schedule,
  //   tasks_info); if (job_curr_try_finish <= job_start_curr) {
  //     job_last_read = job_curr_try;
  //   } else {
  //     if (GetFinishTime(job_last_read, schedule, tasks_info) <=
  //     job_start_curr)
  //       return job_last_read;
  //     else
  //       CoutError("Didn't find reading job in GetLastReadJob!");
  //   }
  // }
  return possible_read;
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
      JobCEC jobs_possible_match = GetFirstReactJob(
          job_curr, task_next, super_period, tasks_info, schedule);
      job_matches[job_curr] = jobs_possible_match;
    }
  } else if (IfDA_Trait(type_trait) || IfSF_Trait(type_trait)) {
    for (int i = 0; i < super_period / task_next.period; i++) {
      JobCEC job_curr(next_task_id, i);
      JobCEC jobs_possible_match = GetLastReadJob(
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