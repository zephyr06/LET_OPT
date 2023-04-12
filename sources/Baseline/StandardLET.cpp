#include "sources/Baseline/StandardLET.h"

namespace DAG_SPACE {
PermutationInequality GetPermIneq(const DAG_Model& dag_tasks,
                                  const TaskSetInfoDerived& tasks_info,
                                  int prev_task_id, int next_task_id,
                                  const std::string& type_trait) {
  return PermutationInequality(prev_task_id, next_task_id, type_trait);
}


SinglePairPermutation GetSinglePermutationStanLET(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    int prev_task_id, int next_task_id, const std::string& type_trait,
    const VariableOD& variable_od) {
  return SinglePairPermutation(
      GetPermIneq(dag_tasks, tasks_info, prev_task_id, next_task_id,
                  type_trait),
      GetJobMatch(dag_tasks, tasks_info, prev_task_id, next_task_id, type_trait,
                  variable_od),
      type_trait);
}

ChainsPermutation GetStandardLETChain(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    const std::vector<std::vector<int>>& task_chains,
    const std::string& type_trait, const VariableOD& variable_od) {
  ChainsPermutation chains_perm(1e2);
  for (const auto& chain : task_chains) {
    for (uint i = 0; i < chain.size() - 1; i++) {
      chains_perm.push_back(std::make_shared<const SinglePairPermutation>(
          GetSinglePermutationStanLET(dag_tasks, tasks_info, chain[i],
                                      chain[i + 1], type_trait, variable_od)));
    }
  }

  return chains_perm;
}

}  // namespace DAG_SPACE