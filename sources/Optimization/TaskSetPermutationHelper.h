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

}  // namespace DAG_SPACE
