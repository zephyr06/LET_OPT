#pragma once
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
#include "sources/Permutations/ChainsPermutation.h"

namespace DAG_SPACE {

bool ExamVariableFeasibility(const VariableOD& variable,
                             const ChainsPermutation& chains_perm,
                             const GraphOfChains& graph_of_all_ca_chains,
                             const DAG_Model& dag_task,
                             const std::vector<int>& chain);

VariableOD FindODFromSingleChainPermutation(
    const DAG_Model& dag_tasks, const ChainsPermutation& chains_perm,
    const GraphOfChains& graph_of_all_ca_chains, const std::vector<int>& chain,
    const std::vector<int>& rta);

std::vector<std::vector<int>> GetSubChains(
    const std::vector<std::vector<int>>& chains_full_length,
    const ChainsPermutation& chains_perm);
// Note: this function doesn't change chains_perm
std::vector<std::unordered_map<JobCEC, JobCEC>> GetFirstReactMaps(
    ChainsPermutation& chains_perm,
    const std::shared_ptr<const SinglePairPermutation> single_perm,
    const std::vector<std::vector<int>>& chains, const DAG_Model& dag_tasks,
    const TaskSetInfoDerived& tasks_info);

std::vector<std::unordered_map<JobCEC, JobCEC>> GetFirstReactMaps(
    const ChainsPermutation& chains_perm,
    const std::vector<std::vector<int>>& chains, const DAG_Model& dag_tasks,
    const TaskSetInfoDerived& tasks_info);

// return true if it's possible for curr_first_job_maps to achieve better
// performance than curr_best_first_job_maps
// bool CompareNewPerm(
//     const std::vector<std::unordered_map<JobCEC, JobCEC>>&
//     curr_first_job_maps, const std::vector<std::unordered_map<JobCEC,
//     JobCEC>>&
//         curr_best_first_job_maps);

}  // namespace DAG_SPACE
