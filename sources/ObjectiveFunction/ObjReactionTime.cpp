#include "sources/ObjectiveFunction/ObjReactionTime.h"
namespace DAG_SPACE {

const std::string ObjReactionTime::type_trait("ReactionTime");
const std::string ObjReactionTimeIntermediate::type_trait(
    "ObjReactionTimeIntermediate");

JobCEC GetFirstReactJobWithSuperPeriod(
    const JobCEC &job_curr, const SinglePairPermutation &pair_perm_curr) {
  auto itr = pair_perm_curr.job_matches_.find(job_curr);
  if (itr == pair_perm_curr.job_matches_.end())
    CoutError(
        "Didn't find job_curr records in "
        "GetFirstReactJobWithSuperPeriod!");
  else {
    return itr->second;
  }

  return JobCEC(-1, -1);
}

JobCEC GetFirstReactJob(const JobCEC &job_curr,
                        const SinglePairPermutation &pair_perm_curr,
                        const TaskSetInfoDerived &tasks_info) {
  // const SinglePairPermutation &pair_perm_curr = *chains_perm[edge_curr];
  int task_id_prev = job_curr.taskId;
  if (task_id_prev != pair_perm_curr.inequality_.task_prev_id_)
    CoutError("Wrong task_index_in_chain index in GetFirstReactJob!");
  int task_id_next = pair_perm_curr.inequality_.task_next_id_;

  int super_period = GetSuperPeriod(tasks_info.GetTask(task_id_prev),
                                    tasks_info.GetTask(task_id_next));
  int prev_jobs_in_sp = super_period / tasks_info.GetTask(task_id_prev).period;
  int next_jobs_in_sp = super_period / tasks_info.GetTask(task_id_next).period;

  int sp_index = job_curr.jobId / prev_jobs_in_sp;
  JobCEC react_job = GetFirstReactJobWithSuperPeriod(
      JobCEC(job_curr.taskId, job_curr.jobId % prev_jobs_in_sp),
      pair_perm_curr);
  react_job.jobId += sp_index * next_jobs_in_sp;
  return react_job;
}

JobCEC GetFirstReactJob(const JobCEC job_source,
                        const ChainsPermutation &chains_perm,
                        const std::vector<int> &chain,
                        const TaskSetInfoDerived &tasks_info) {
  JobCEC job_curr = job_source;
  for (uint i = 0; i < chain.size() - 1;
       i++)  // iterate through task-level cause-effect chain
  {
    Edge edge_i(chain[i], chain[i + 1]);
    job_curr = GetFirstReactJob(job_curr, *chains_perm[edge_i], tasks_info);
  }
  return job_curr;
}

double ObjReactionTimeIntermediate::ObjSingleChain(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chains_perm, const std::vector<int> &chain,
    const VariableOD &variable_od) {
  std::vector<double> all_reaction_time_instance =
      ObjAllInstances(dag_tasks, tasks_info, chains_perm, chain, variable_od);
  return max(*std::max_element(all_reaction_time_instance.begin(),
                               all_reaction_time_instance.end()),
             0);
}

std::vector<double> ObjReactionTimeIntermediate::ObjAllInstances(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chains_perm, const std::vector<int> &chain,
    const VariableOD &variable_od) {
  int hyper_period = GetHyperPeriod(tasks_info, chain);
  std::vector<double> all_reaction_time_instance;
  all_reaction_time_instance.reserve(hyper_period /
                                     tasks_info.GetTask(chain[0]).period);
  for (uint j = 0; j < hyper_period / tasks_info.GetTask(chain[0]).period;
       j++)  // iterate each source job within a hyper-period
  {
    JobCEC job_source(chain[0], j);
    JobCEC job_react =
        GetFirstReactJob(job_source, chains_perm, chain, tasks_info);

    int deadline_curr = GetDeadline(job_react, variable_od, tasks_info);
    all_reaction_time_instance.push_back(
        int(deadline_curr - GetStartTime(job_source, variable_od, tasks_info)));
  }
  return all_reaction_time_instance;
}

double ObjReactionTimeIntermediate::ObjSingleChain(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chains_perm, const std::vector<int> &chain,
    const Schedule &schedule) {
  std::vector<double> all_reaction_time_instance =
      ObjAllInstances(dag_tasks, tasks_info, chains_perm, chain, schedule);
  return max(*std::max_element(all_reaction_time_instance.begin(),
                               all_reaction_time_instance.end()),
             0);
}

std::vector<double> ObjReactionTimeIntermediate::ObjAllInstances(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chains_perm, const std::vector<int> &chain,
    const Schedule &schedule) {
  int hyper_period = GetHyperPeriod(tasks_info, chain);

  std::vector<double> all_reaction_time_instance;
  all_reaction_time_instance.reserve(hyper_period /
                                     tasks_info.GetTask(chain[0]).period);
  for (uint j = 0; j < hyper_period / tasks_info.GetTask(chain[0]).period;
       j++)  // iterate each source job within a hyper-period
  {
    JobCEC job_source(chain[0], j);
    JobCEC job_react =
        GetFirstReactJob(job_source, chains_perm, chain, tasks_info);
    // *************** Unique code compared from above
    int deadline_curr = GetFinishTime(job_react, schedule, tasks_info);
    all_reaction_time_instance.push_back(
        int(deadline_curr - GetStartTime(job_source, schedule, tasks_info)));
    // ***************
  }
  return all_reaction_time_instance;
}
}  // namespace DAG_SPACE