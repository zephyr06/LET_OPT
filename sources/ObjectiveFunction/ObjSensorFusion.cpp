#include "sources/ObjectiveFunction/ObjSensorFusion.h"

namespace DAG_SPACE {

const std::string ObjSensorFusion::type_trait("SensorFusion");

struct SF_JobFork {
  SF_JobFork() {}
  SF_JobFork(JobCEC sink_job, std::vector<JobCEC> source_jobs)
      : sink_job(sink_job), source_jobs(source_jobs) {}
  // data members
  JobCEC sink_job;
  std::vector<JobCEC> source_jobs;
};
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

double ObjSensorFusion::Obj(const DAG_Model &dag_tasks,
                            const TaskSetInfoDerived &tasks_info,
                            const ChainsPermutation &chains_perm,
                            const VariableOD &variable_od) {
  return 0;
}

double ObjSensorFusion::Obj(const DAG_Model &dag_tasks,
                            const TaskSetInfoDerived &tasks_info,
                            const ChainsPermutation &chains_perm,
                            const Schedule &schedule) {
  int diff_all_forks = 0;
  for (const auto &fork_curr : dag_tasks.sf_forks_) {
    int for_curr_max_diff = 0;
    int sink_task_id = fork_curr.sink;
    const std::vector<int> &source_tasks = fork_curr.source;
    SF_JobFork job_forks;
    for (int job_id = 0; job_id < tasks_info.hyper_period /
                                      dag_tasks.GetTask(sink_task_id).period;
         job_id++) {
      JobCEC sink_job(sink_task_id, job_id);
      job_forks.sink_job = sink_job;
      for (int source_id : source_tasks) {
        Edge edge_curr(source_id, sink_task_id);
        JobCEC last_read_job =
            GetLastReadJob(sink_job, *chains_perm[edge_curr], tasks_info);
        job_forks.source_jobs.push_back(last_read_job);
      }
      for_curr_max_diff = std::max(for_curr_max_diff,
                                   GetSF_Diff(job_forks, schedule, tasks_info));
    }
    diff_all_forks += for_curr_max_diff;
  }
  return diff_all_forks;
}

}  // namespace DAG_SPACE