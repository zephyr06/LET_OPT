#include "sources/ObjectiveFunction/ObjectiveFunctionBase.h"

namespace DAG_SPACE {

const std::string ObjectiveFunctionBaseIntermediate::type_trait(
    "ObjectiveFunctionBase");

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

double ObjectiveFunctionBaseIntermediate::ObjWithoutAllPerms(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chain_perm, const VariableOD &variable_od) {
#ifdef PROFILE_CODE
    BeginTimer(__FUNCTION__);
#endif
    int max_obj = 0;
    for (const auto &chain : dag_tasks.chains_) {
        max_obj += ObjWithoutAllPermsSingleChain(
            dag_tasks, tasks_info, chain_perm, chain, variable_od);
    }

#ifdef PROFILE_CODE
    EndTimer(__FUNCTION__);
#endif
    return max_obj;
}

}  // namespace DAG_SPACE