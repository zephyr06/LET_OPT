#include "sources/ObjectiveFunction/ObjectiveFunctionBase.h"

namespace DAG_SPACE {

const std::string ObjectiveFunctionBaseIntermediate::type_trait(
    "ObjectiveFunctionBase");

double ObjectiveFunctionBaseIntermediate::Obj(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chains_perm, const VariableOD &variable_od,
    const std::vector<std::vector<int>> &chains_to_analyze) {
#ifdef PROFILE_CODE
  BeginTimer(__FUNCTION__);
#endif

  int max_obj = 0;
  for (const auto &chain : chains_to_analyze) {
    if (chain.size() == 0) continue;
    max_obj +=
        ObjSingleChain(dag_tasks, tasks_info, chains_perm, chain, variable_od);
  }

#ifdef PROFILE_CODE
  EndTimer(__FUNCTION__);
#endif
  return max_obj;
}

}  // namespace DAG_SPACE