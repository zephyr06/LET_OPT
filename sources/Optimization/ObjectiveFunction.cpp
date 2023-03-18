#include "sources/Optimization/ObjectiveFunction.h"

namespace DAG_SPACE {

const std::string ObjectiveFunctionBaseIntermediate::type_trait(
    "ObjectiveFunctionBase");
const std::string ObjReactionTimeIntermediate::type_trait(
    "ObjReactionTimeIntermediate");
const std::string ObjReactionTime::type_trait("ReactionTime");
const std::string ObjDataAgeIntermediate::type_trait("ObjDataAgeIntermediate");
const std::string ObjDataAge::type_trait("DataAge");
const std::string ObjSensorFusion::type_trait("SensorFusion");

JobCEC GetFirstReactJobWithSuperPeriod(
    const JobCEC &job_curr, const SinglePairPermutation &pair_perm_curr) {
    auto itr = pair_perm_curr.job_first_react_matches_.find(job_curr);
    if (itr == pair_perm_curr.job_first_react_matches_.end())
        CoutError(
            "Didn't find job_curr records in "
            "GetFirstReactJobWithSuperPeriod!");
    else {
        return itr->second.front();  // assume it is sorted, TODO: guarantee it
    }

    return JobCEC(-1, -1);
}

JobCEC GetFirstReactJob(const JobCEC &job_curr,
                        const ChainsPermutation &chain_perm,
                        const Edge &edge_curr,
                        const TaskSetInfoDerived &tasks_info) {
    const SinglePairPermutation &pair_perm_curr = chain_perm[edge_curr];
    int task_id_prev = job_curr.taskId;
    if (task_id_prev != pair_perm_curr.inequality_.task_prev_id_)
        CoutError("Wrong task_index_in_chain index in GetFirstReactJob!");
    int task_id_next = pair_perm_curr.inequality_.task_next_id_;

    int super_period = GetSuperPeriod(tasks_info.GetTask(task_id_prev),
                                      tasks_info.GetTask(task_id_next));
    int prev_jobs_in_sp =
        super_period / tasks_info.GetTask(task_id_prev).period;
    int next_jobs_in_sp =
        super_period / tasks_info.GetTask(task_id_next).period;

    int sp_index = job_curr.jobId / prev_jobs_in_sp;
    JobCEC react_job = GetFirstReactJobWithSuperPeriod(
        JobCEC(job_curr.taskId, job_curr.jobId % prev_jobs_in_sp),
        pair_perm_curr);
    react_job.jobId += sp_index * next_jobs_in_sp;
    return react_job;
}

double ObjReactionTimeIntermediate::ObjSingleChain(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chain_perm, const std::vector<int> &chain,
    const VariableOD &variable_od) {
    int max_reaction_time = -1;
    int hyper_period = GetHyperPeriod(tasks_info, chain);
    for (uint j = 0; j < hyper_period / tasks_info.GetTask(chain[0]).period;
         j++)  // iterate each source job within a hyper-period
    {
        JobCEC job_source(chain[0], j);
        JobCEC job_curr = job_source;
        for (uint i = 0; i < chain.size() - 1;
             i++)  // iterate through task-level cause-effect chain
        {
            Edge edge_i(chain[i], chain[i + 1]);
            job_curr =
                GetFirstReactJob(job_curr, chain_perm, edge_i, tasks_info);
        }

        int deadline_curr = GetDeadline(job_curr, variable_od, tasks_info);
        max_reaction_time = std::max(
            max_reaction_time,
            int(deadline_curr - GetActivationTime(job_source, tasks_info)));
    }
    return max_reaction_time;
}

double ObjectiveFunctionBaseIntermediate::Obj(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chain_perm, const VariableOD &variable_od) {
#ifdef PROFILE_CODE
    BeginTimer(__FUNCTION__);
#endif
    int max_obj = 0;
    for (const auto &chain : dag_tasks.chains_) {
        max_obj += ObjSingleChain(dag_tasks, tasks_info, chain_perm, chain,
                                  variable_od);
    }

#ifdef PROFILE_CODE
    EndTimer(__FUNCTION__);
#endif
    return max_obj;
}

std::unordered_map<JobCEC, JobCEC> GetFirstReactMap(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chain_perm, const std::vector<int> &chain) {
    std::unordered_map<JobCEC, JobCEC> first_react_map;
    int hyper_period = GetHyperPeriod(tasks_info, chain);
    int job_num_in_hyper_period =
        hyper_period / tasks_info.GetTask(chain[0]).period;
    first_react_map.reserve(job_num_in_hyper_period);
    for (int j = 0; j < job_num_in_hyper_period + 2;
         j++)  // iterate each source job within a hyper-period
    {
        JobCEC job_source(chain[0], j);
        JobCEC job_curr = job_source;
        for (uint i = 0; i < chain.size() - 1;
             i++)  // iterate through task-level cause-effect chain
        {
            Edge edge_i(chain[i], chain[i + 1]);
            job_curr =
                GetFirstReactJob(job_curr, chain_perm, edge_i, tasks_info);
        }
        first_react_map[job_source] = job_curr;
    }
    return first_react_map;
}

double ObjDataAgeIntermediate::ObjSingleChain(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chain_perm, const std::vector<int> &chain,
    const VariableOD &variable_od) {
    int max_data_age = -1;
    int hyper_period = GetHyperPeriod(tasks_info, chain);
    int job_num_in_hyper_period =
        hyper_period / tasks_info.GetTask(chain[0]).period;
    std::unordered_map<JobCEC, JobCEC> first_react_map =
        GetFirstReactMap(dag_tasks, tasks_info, chain_perm, chain);
    for (int j = 1; j <= job_num_in_hyper_period + 1;
         j++)  // iterate each source job within a hyper-period
    {
        JobCEC job_source(chain[0], j);
        JobCEC job_first_reacted = first_react_map[job_source];
        JobCEC job_source_prev_job(job_source.taskId, job_source.jobId - 1);
        if (first_react_map[job_source_prev_job] != job_first_reacted &&
            job_first_reacted.jobId > 0) {
            JobCEC first_reacted_job_prev_job(job_first_reacted.taskId,
                                              job_first_reacted.jobId - 1);
            int deadline_curr = GetDeadline(first_reacted_job_prev_job,
                                            variable_od, tasks_info);
            max_data_age = std::max(
                max_data_age,
                int(deadline_curr -
                    GetActivationTime(
                        JobCEC(job_source.taskId, job_source.jobId - 1),
                        tasks_info)));
        }
    }
    return max_data_age;
}

double ObjSensorFusion::Obj(const DAG_Model &dag_tasks,
                            const TaskSetInfoDerived &tasks_info,
                            const ChainsPermutation &chain_perm,
                            const VariableOD &variable_od) {
    return 0;
}
}  // namespace DAG_SPACE