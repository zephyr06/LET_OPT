#include "sources/Optimization/ObjectiveFunction.h"

namespace DAG_SPACE {

const std::string ObjectiveFunctionBase::type_trait("ObjectiveFunctionBase");

const std::string ObjReactionTime::type_trait("ReactionTime");

JobCEC GetFirstReactJobWithSuperPeriod(const JobCEC &job_curr,
                                       const ChainPermutation &chain_perm,
                                       uint task_index_in_chain) {
    if (task_index_in_chain < chain_perm.size()) {
        const SinglePairPermutation &pair_perm_curr =
            chain_perm[task_index_in_chain];
        // pair_perm_curr.print();
        auto itr = pair_perm_curr.job_first_react_matches_.find(job_curr);
        if (itr == pair_perm_curr.job_first_react_matches_.end())
            CoutError(
                "Didn't find job_curr records in "
                "GetFirstReactJobWithSuperPeriod!");
        else
            return itr->second
                .front();  // assume it is sorted, TODO: guarantee it
    } else {
        CoutError("No match because of an out-of-range error!");
    }

    return JobCEC(-1, -1);
}

JobCEC GetFirstReactJob(const JobCEC &job_curr,
                        const ChainPermutation &chain_perm,
                        uint task_index_in_chain,
                        const TaskSetInfoDerived &tasks_info) {
    const SinglePairPermutation &pair_perm_curr =
        chain_perm[task_index_in_chain];
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
        task_index_in_chain);
    react_job.jobId += sp_index * next_jobs_in_sp;
    return react_job;
}

double ObjReactionTime::Obj(const DAG_Model &dag_tasks,
                            const TaskSetInfoDerived &tasks_info,
                            const ChainPermutation &chain_perm,
                            const VariableOD &variable_od) {
    int max_reaction_time = -1;
    for (uint j = 0; j < tasks_info.sizeOfVariables[chain_perm.GetTaskId(0)];
         j++)  // iterate each source job within a hyper-period
    {
        JobCEC job_source(chain_perm.GetTaskId(0), j);
        JobCEC job_curr = job_source;
        for (uint i = 0; i < chain_perm.size();
             i++)  // iterate through task-level cause-effect chain
        {
            job_curr = GetFirstReactJob(job_curr, chain_perm, i, tasks_info);
        }
        int deadline_curr =
            variable_od[job_curr.taskId].deadline +
            tasks_info.GetTask(job_curr.taskId).period * job_curr.jobId;
        // int offset_curr = variable_od[job_source.taskId].offset;
        max_reaction_time = std::max(
            max_reaction_time,
            int(deadline_curr - GetActivationTime(job_source, tasks_info)));
    }
    return max_reaction_time;
}
}  // namespace DAG_SPACE