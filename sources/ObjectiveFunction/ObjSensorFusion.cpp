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
// TODO: imprvoe efficiency there
JobCEC GetLastReadJob(JobCEC sink_job, int source_task_id,
                      const TaskSetInfoDerived &tasks_info,
                      const Schedule &schedule) {
  int start_sink = GetStartTime(sink_job, schedule, tasks_info);
  int min_source_id =
      -1 * tasks_info.hyper_period / tasks_info.GetTask(source_task_id).period;
  int max_source =
      tasks_info.hyper_period / tasks_info.GetTask(source_task_id).period;
  for (int job_id = min_source_id; job_id <= max_source; job_id++) {
    JobCEC job_curr(source_task_id, job_id);
    int finish_curr = GetFinishTime(job_curr, schedule, tasks_info);
    int finish_next =
        GetFinishTime(JobCEC(source_task_id, job_id + 1), schedule, tasks_info);
    if (finish_curr <= start_sink && finish_next > start_sink) {
      return job_curr;
    }
  }
  CoutError("Didn't find source job");
  return JobCEC(0, 0);
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

SF_JobFork GetSF_JobFork(JobCEC sink_job, const std::vector<int> &source_tasks,
                         const TaskSetInfoDerived &tasks_info,
                         const Schedule &schedule) {
  SF_JobFork job_forks;
  job_forks.sink_job = sink_job;
  for (int source_id : source_tasks) {
    JobCEC last_read_job =
        GetLastReadJob(sink_job, source_id, tasks_info, schedule);
    job_forks.source_jobs.push_back(last_read_job);
  }
  return job_forks;
}
std::vector<double> ObjSensorFusion::ObjAllInstances_SingleFork(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chains_perm, const VariableOD &variable_od,
    const SF_Fork &fork_curr) {
  std::vector<double> all_instances;
  int fork_num =
      tasks_info.hyper_period / dag_tasks.GetTask(fork_curr.sink).period;
  all_instances.reserve(fork_num);

  int for_curr_max_diff = 0;
  for (int job_id = 0; job_id < fork_num; job_id++) {
    SF_JobFork job_forks =
        GetSF_JobFork(JobCEC(fork_curr.sink, job_id), fork_curr.source,
                      tasks_info, chains_perm);
    all_instances.push_back(GetSF_Diff(job_forks, variable_od, tasks_info));
  }
  return all_instances;
}

double ObjSensorFusion::Obj(const DAG_Model &dag_tasks,
                            const TaskSetInfoDerived &tasks_info,
                            const ChainsPermutation &chains_perm,
                            const VariableOD &variable_od,
                            const std::vector<std::vector<int>> /*unusedArg*/) {
  int diff_all_forks = 0;
  for (const auto &fork_curr : dag_tasks.sf_forks_) {
    std::vector<double> all_instances = ObjAllInstances_SingleFork(
        dag_tasks, tasks_info, chains_perm, variable_od, fork_curr);
    int for_curr_max_diff =
        *max_element(all_instances.begin(), all_instances.end());
    diff_all_forks += for_curr_max_diff;
  }
  return diff_all_forks;
}

std::vector<double> ObjSensorFusion::ObjAllInstances_SingleFork(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const Schedule &schedule, const SF_Fork &fork_curr) {
  std::vector<double> all_instances;
  int fork_num =
      tasks_info.hyper_period / dag_tasks.GetTask(fork_curr.sink).period;
  all_instances.reserve(fork_num);
  for (int job_id = 0; job_id < fork_num; job_id++) {
    SF_JobFork job_forks = GetSF_JobFork(
        JobCEC(fork_curr.sink, job_id), fork_curr.source, tasks_info, schedule);
    all_instances.push_back(GetSF_Diff(job_forks, schedule, tasks_info));
  }
  return all_instances;
}

double ObjSensorFusion::Obj(const DAG_Model &dag_tasks,
                            const TaskSetInfoDerived &tasks_info,
                            const ChainsPermutation & /*unusedArg*/,
                            const Schedule &schedule,
                            const std::vector<std::vector<int>> /*unusedArg*/) {
  int diff_all_forks = 0;
  for (const auto &fork_curr : dag_tasks.sf_forks_) {
    std::vector<double> all_instances =
        ObjAllInstances_SingleFork(dag_tasks, tasks_info, schedule, fork_curr);
    int for_curr_max_diff =
        *max_element(all_instances.begin(), all_instances.end());
    diff_all_forks += for_curr_max_diff;
  }
  return diff_all_forks;
}
double ObjSensorFusion::Jitter(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chains_perm, const VariableOD &variable_od,
    const std::vector<std::vector<int>> & /*unusedArg*/) {
  double jitter_forks = 0;
  for (const auto &fork_curr : dag_tasks.sf_forks_) {
    std::vector<double> all_instances = ObjAllInstances_SingleFork(
        dag_tasks, tasks_info, chains_perm, variable_od, fork_curr);
    jitter_forks += JitterOfVector(all_instances);
  }
  return jitter_forks;
}

double ObjSensorFusion::Jitter(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation & /*unusedArg*/, const Schedule &schedule,
    const std::vector<std::vector<int>> & /*unusedArg*/) {
  double jitter_forks = 0;
  for (const auto &fork_curr : dag_tasks.sf_forks_) {
    std::vector<double> all_instances =
        ObjAllInstances_SingleFork(dag_tasks, tasks_info, schedule, fork_curr);
    jitter_forks += JitterOfVector(all_instances);
  }
  return jitter_forks;
}

bool ForkIntersect(const SF_Fork &fork1, const SF_Fork &fork2) {
  std::unordered_set<int> id_record1;
  id_record1.reserve(fork1.source.size() + 1);
  id_record1.insert(fork1.sink);
  for (int id : fork1.source) id_record1.insert(id);
  if (id_record1.find(fork2.sink) != id_record1.end()) return true;
  for (int id : fork2.source)
    if (id_record1.find(id) != id_record1.end()) return true;
  return false;
}
bool ForkIntersect(const std::vector<SF_Fork> &forks1,
                   const std::vector<SF_Fork> &forks2) {
  for (const auto &fork1 : forks1) {
    for (const auto &fork2 : forks2) {
      if (ForkIntersect(fork1, fork2)) return true;
    }
  }
  return false;
}

void Merge_j2i(std::vector<std::vector<SF_Fork>> &decoupled_sf_forks, uint i,
               uint j) {
  std::vector<SF_Fork> forks_j = decoupled_sf_forks[j];
  decoupled_sf_forks.erase(decoupled_sf_forks.begin() + j);
  if (i > j) i--;
  for (const auto &fork : forks_j) decoupled_sf_forks[i].push_back(fork);
}

std::vector<std::vector<SF_Fork>> ExtractDecoupledForks(
    const std::vector<SF_Fork> &sf_forks_all) {
  std::vector<std::vector<SF_Fork>> decoupled_sf_forks;
  for (uint i = 0; i < sf_forks_all.size(); i++)
    decoupled_sf_forks.push_back({sf_forks_all[i]});

  for (uint i = 0; i < decoupled_sf_forks.size(); i++) {
    for (uint j = 0; j < decoupled_sf_forks.size(); j++) {
      if (i == j) continue;
      if (ForkIntersect(decoupled_sf_forks[i], decoupled_sf_forks[j])) {
        Merge_j2i(decoupled_sf_forks, i, j);
        j--;
      }
    }
  }
  return decoupled_sf_forks;
}

std::vector<DAG_Model> ExtractDAGsWithIndependentForks(
    const DAG_Model &dag_tasks) {
  if (dag_tasks.sf_forks_.size() <= 1) return {dag_tasks};
  std::vector<std::vector<SF_Fork>> decoupled_sf_forks =
      ExtractDecoupledForks(dag_tasks.sf_forks_);
  std::vector<DAG_Model> dag_tasks_decoupled_forks;
  dag_tasks_decoupled_forks.reserve(decoupled_sf_forks.size());
  for (uint i = 0; i < decoupled_sf_forks.size(); i++) {
    DAG_Model dags = dag_tasks;
    dags.sf_forks_ = decoupled_sf_forks[i];
    dag_tasks_decoupled_forks.push_back(dags);
  }
  return dag_tasks_decoupled_forks;
}

}  // namespace DAG_SPACE