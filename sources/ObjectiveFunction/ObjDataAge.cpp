#include "sources/ObjectiveFunction/ObjDataAge.h"
namespace DAG_SPACE {

const std::string ObjDataAgeIntermediate::type_trait("ObjDataAgeIntermediate");
const std::string ObjDataAge::type_trait("DataAge");

JobCEC GetLastReadJobWithSuperPeriod(
    const JobCEC &job_curr, const SinglePairPermutation &pair_perm_curr) {
  auto itr = pair_perm_curr.job_matches_.find(job_curr);
  if (itr == pair_perm_curr.job_matches_.end())
    CoutError(
        "Didn't find job_curr records in "
        "GetLastReadJobWithSuperPeriod!");
  else {
    return itr->second;
  }

  return JobCEC(-1, -1);
}

JobCEC GetLastReadJob(const JobCEC &job_curr,
                      const SinglePairPermutation &pair_perm_curr,
                      const TaskSetInfoDerived &tasks_info) {
  int task_id_next = job_curr.taskId;
  int task_id_prev = pair_perm_curr.inequality_.task_prev_id_;

  int super_period = GetSuperPeriod(tasks_info.GetTask(task_id_prev),
                                    tasks_info.GetTask(task_id_next));
  int next_jobs_in_sp = super_period / tasks_info.GetTask(task_id_next).period;
  int prev_jobs_in_sp = super_period / tasks_info.GetTask(task_id_prev).period;

  // int job_curr_id_slide = job_curr.jobId + next_jobs_in_sp;
  int sp_index = job_curr.jobId / next_jobs_in_sp;
  JobCEC job_curr_mapped_to_single_sp(job_curr.taskId,
                                      job_curr.jobId % next_jobs_in_sp);
  if (job_curr_mapped_to_single_sp.jobId < 0) {
    job_curr_mapped_to_single_sp.jobId += next_jobs_in_sp;
    sp_index -= 1;
  }

  JobCEC read_job = GetLastReadJobWithSuperPeriod(job_curr_mapped_to_single_sp,
                                                  pair_perm_curr);
  read_job.jobId += sp_index * prev_jobs_in_sp;
  return read_job;
}

JobCEC GetLastReadJob(const JobCEC job_source,
                      const ChainsPermutation &chains_perm,
                      const std::vector<int> &chain,
                      const TaskSetInfoDerived &tasks_info) {
  JobCEC job_last_read = job_source;
  for (uint i = chain.size() - 1; i > 0;
       i--)  // iterate through task-level cause-effect chain
  {
    Edge edge_i(chain[i - 1], chain[i]);
    job_last_read =
        GetLastReadJob(job_last_read, *chains_perm[edge_i], tasks_info);
  }
  return job_last_read;
}

double ObjDataAgeIntermediate::ObjSingleChain(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chains_perm, const std::vector<int> &chain,
    const VariableOD &variable_od) {
  int max_data_age = -1;
  int hyper_period = GetHyperPeriod(tasks_info, chain);
  int job_num_in_hyper_period =
      hyper_period / tasks_info.GetTask(chain.back()).period;
  for (int j = 0; j <= job_num_in_hyper_period;
       j++)  // iterate each source job within a hyper-period
  {
    JobCEC job_source(chain.back(), j);
    JobCEC job_last_read =
        GetLastReadJob(job_source, chains_perm, chain, tasks_info);
    max_data_age =
        std::max(max_data_age,
                 int(GetDeadline(job_source, variable_od, tasks_info) -
                     GetStartTime(job_last_read, variable_od, tasks_info)));
  }
  return max_data_age;
}

double ObjDataAgeIntermediate::ObjSingleChain(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chains_perm, const std::vector<int> &chain,
    const Schedule &schedule) {
  int max_data_age = -1;
  int hyper_period = GetHyperPeriod(tasks_info, chain);
  int job_num_in_hyper_period =
      hyper_period / tasks_info.GetTask(chain.back()).period;
  for (int j = 0; j <= job_num_in_hyper_period;
       j++)  // iterate each source job within a hyper-period
  {
    JobCEC job_source(chain.back(), j);
    JobCEC job_last_read =
        GetLastReadJob(job_source, chains_perm, chain, tasks_info);

    // *************** Unique code compared from above
    max_data_age = std::max(
        max_data_age, int(GetFinishTime(job_source, schedule, tasks_info) -
                          GetStartTime(job_last_read, schedule, tasks_info)));
    // ***************
  }
  return max_data_age;
}
}  // namespace DAG_SPACE