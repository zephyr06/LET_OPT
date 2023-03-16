#include "sources/Optimization/ObjectiveFunction.h"

namespace DAG_SPACE {

const std::string ObjectiveFunctionBaseIntermediate::type_trait(
    "ObjectiveFunctionBase");
const std::string ObjReactionTimeIntermediate::type_trait(
    "ObjReactionTimeIntermediate");
const std::string ObjReactionTime::type_trait("ReactionTime");

JobCEC GetFirstReactJobWithSuperPeriod(const JobCEC &job_curr,
                                       const ChainPermutation &chain_perm,
                                       const Edge &edge_curr) {
    const SinglePairPermutation &pair_perm_curr = chain_perm[edge_curr];
    // pair_perm_curr.print();
    auto itr = pair_perm_curr.job_first_react_matches_.find(job_curr);
    if (itr == pair_perm_curr.job_first_react_matches_.end())
        CoutError(
            "Didn't find job_curr records in "
            "GetFirstReactJobWithSuperPeriod!");
    else
        return itr->second.front();  // assume it is sorted, TODO: guarantee it

    return JobCEC(-1, -1);
}

JobCEC GetFirstReactJob(const JobCEC &job_curr,
                        const ChainPermutation &chain_perm,
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
        JobCEC(job_curr.taskId, job_curr.jobId % prev_jobs_in_sp), chain_perm,
        edge_curr);
    react_job.jobId += sp_index * next_jobs_in_sp;
    return react_job;
}

double ObjReactionTimeIntermediate::ObjSingleChain(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainPermutation &chain_perm, const std::vector<int> &chain,
    const VariableOD &variable_od) {
    int max_reaction_time = -1;
    for (uint j = 0; j < tasks_info.sizeOfVariables[chain[0]];
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
        int deadline_curr =
            variable_od.at(job_curr.taskId).deadline +
            tasks_info.GetTask(job_curr.taskId).period * job_curr.jobId;
        // int offset_curr = variable_od[job_source.taskId].offset;
        max_reaction_time = std::max(
            max_reaction_time,
            int(deadline_curr - GetActivationTime(job_source, tasks_info)));
    }
    return max_reaction_time;
}

double ObjectiveFunctionBaseIntermediate::Obj(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainPermutation &chain_perm, const VariableOD &variable_od) {
#ifdef PROFILE_CODE
    BeginTimer(__FUNCTION__);
#endif
    int max_obj = 0;

    for (const auto &chain : dag_tasks.chains_) {
        ChainPermutation chain_perm_curr;
        for (uint i = 0; i < chain.size() - 1; i++) {
            Edge edge_curr(chain[i], chain[i + 1]);
            chain_perm_curr.push_back(chain_perm[edge_curr]);
        }
        max_obj += ObjSingleChain(dag_tasks, tasks_info, chain_perm_curr, chain,
                                  variable_od);
    }

#ifdef PROFILE_CODE
    EndTimer(__FUNCTION__);
#endif
    return max_obj;
}
}  // namespace DAG_SPACE