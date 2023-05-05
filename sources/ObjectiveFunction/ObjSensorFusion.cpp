#include "sources/ObjectiveFunction/ObjSensorFusion.h"

namespace DAG_SPACE {

const std::string ObjSensorFusion::type_trait("SensorFusion");

std::vector<std::vector<int>> GetChainsForSF(const DAG_Model &dag_tasks) {
  std::vector<std::vector<int>> chains;
  std::unordered_set<Edge> edge_record;
  for (const auto &fork_curr : dag_tasks.sf_forks_) {
    int sink_task_id = fork_curr.sink;
    const std::vector<int> &source_tasks = fork_curr.source;
    for (int source_id : source_tasks) {
      Edge edge_curr(source_id, sink_task_id);
      if (edge_record.find(edge_curr) == edge_record.end()) {
        edge_record.insert(edge_curr);
        chains.push_back({source_id, sink_task_id});
      }
    }
  }
  return chains;
}

int GetSF_Diff(const SF_JobFork &job_forks, const Schedule &schedule,
               const TaskSetInfoDerived &tasks_info) {
  int finish_min = 1e9;
  int finish_max = -1e9;
  for (const JobCEC &job_curr : job_forks.source_jobs) {
    int finish_curr = GetFinishTime(job_curr, schedule, tasks_info);
    finish_min = std::min(finish_min, finish_curr);
    finish_max = std::max(finish_max, finish_curr);
  }
  return finish_max - finish_min;
}

int GetSF_Diff(const SF_JobFork &job_forks, const VariableOD &variable_od,
               const TaskSetInfoDerived &tasks_info) {
  int finish_min = 1e9;
  int finish_max = -1e9;
  for (const JobCEC &job_curr : job_forks.source_jobs) {
    int finish_curr = GetDeadline(job_curr, variable_od, tasks_info);
    finish_min = std::min(finish_min, finish_curr);
    finish_max = std::max(finish_max, finish_curr);
  }
  return finish_max - finish_min;
}

SF_JobFork GetSF_JobFork(JobCEC sink_job, const std::vector<int> &source_tasks,
                         const TaskSetInfoDerived &tasks_info,
                         const ChainsPermutation &chains_perm) {
  SF_JobFork job_forks;
  job_forks.sink_job = sink_job;
  for (int source_id : source_tasks) {
    Edge edge_curr(source_id, sink_job.taskId);
    JobCEC last_read_job =
        GetLastReadJob(sink_job, *chains_perm[edge_curr], tasks_info);
    job_forks.source_jobs.push_back(last_read_job);
  }
  return job_forks;
}

double ObjSensorFusion::Obj(const DAG_Model &dag_tasks,
                            const TaskSetInfoDerived &tasks_info,
                            const ChainsPermutation &chains_perm,
                            const VariableOD &variable_od,
                            const std::vector<std::vector<int>> /*unusedArg*/) {
  int diff_all_forks = 0;
  for (const auto &fork_curr : dag_tasks.sf_forks_) {
    int for_curr_max_diff = 0;
    for (int job_id = 0; job_id < tasks_info.hyper_period /
                                      dag_tasks.GetTask(fork_curr.sink).period;
         job_id++) {
      SF_JobFork job_forks =
          GetSF_JobFork(JobCEC(fork_curr.sink, job_id), fork_curr.source,
                        tasks_info, chains_perm);
      for_curr_max_diff = std::max(
          for_curr_max_diff, GetSF_Diff(job_forks, variable_od, tasks_info));
    }
    diff_all_forks += for_curr_max_diff;
  }
  return diff_all_forks;
}

double ObjSensorFusion::Obj(const DAG_Model &dag_tasks,
                            const TaskSetInfoDerived &tasks_info,
                            const ChainsPermutation &chains_perm,
                            const Schedule &schedule,
                            const std::vector<std::vector<int>> /*unusedArg*/) {
  int diff_all_forks = 0;
  for (const auto &fork_curr : dag_tasks.sf_forks_) {
    int for_curr_max_diff = 0;
    for (int job_id = 0; job_id < tasks_info.hyper_period /
                                      dag_tasks.GetTask(fork_curr.sink).period;
         job_id++) {
      SF_JobFork job_forks =
          GetSF_JobFork(JobCEC(fork_curr.sink, job_id), fork_curr.source,
                        tasks_info, chains_perm);
      for_curr_max_diff = std::max(for_curr_max_diff,
                                   GetSF_Diff(job_forks, schedule, tasks_info));
    }
    diff_all_forks += for_curr_max_diff;
  }
  return diff_all_forks;
}

}  // namespace DAG_SPACE