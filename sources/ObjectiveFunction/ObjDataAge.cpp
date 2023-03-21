#include "sources/ObjectiveFunction/ObjDataAge.h"
namespace DAG_SPACE {

const std::string ObjDataAgeIntermediate::type_trait("ObjDataAgeIntermediate");
const std::string ObjDataAge::type_trait("DataAge");

std::unordered_map<JobCEC, JobCEC> GetFirstReactMap(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chain_perm, const std::vector<int> &chain) {
#ifdef PROFILE_CODE
    BeginTimer(__FUNCTION__);
#endif

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
#ifdef PROFILE_CODE
    EndTimer(__FUNCTION__);
#endif
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

double ObjDataAgeIntermediate::ObjWithoutAllPermsSingleChain(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chain_perm, const std::vector<int> &chain,
    const VariableOD &variable_od) {
    return ObjSingleChain(dag_tasks, tasks_info, chain_perm, chain,
                          variable_od);
}

}  // namespace DAG_SPACE