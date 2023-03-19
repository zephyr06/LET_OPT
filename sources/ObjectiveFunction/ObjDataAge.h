#pragma once
#include "sources/ObjectiveFunction/ObjReactionTime.h"
namespace DAG_SPACE {

std::unordered_map<JobCEC, JobCEC> GetFirstReactMap(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chain_perm, const std::vector<int> &chain);

class ObjDataAgeIntermediate : public ObjectiveFunctionBaseIntermediate {
   public:
    static const std::string type_trait;
    double ObjSingleChain(const DAG_Model &dag_tasks,
                          const TaskSetInfoDerived &tasks_info,
                          const ChainsPermutation &chain_perm,
                          const std::vector<int> &chain,
                          const VariableOD &variable_od) override;

    double ObjWithoutAllPermsSingleChain(
        const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
        const ChainsPermutation &chain_perm, const std::vector<int> &chain,
        const VariableOD &variable_od) override;
};

class ObjDataAge {
   public:
    static const std::string type_trait;
    static double Obj(const DAG_Model &dag_tasks,
                      const TaskSetInfoDerived &tasks_info,
                      const ChainsPermutation &chain_perm,
                      const VariableOD &variable_od) {
        ObjDataAgeIntermediate obj;
        return obj.Obj(dag_tasks, tasks_info, chain_perm, variable_od);
    }

    static double ObjWithoutAllPermsSingleChain(
        const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
        const ChainsPermutation &chain_perm, const std::vector<int> &chain,
        const VariableOD &variable_od) {
        ObjDataAgeIntermediate obj;
        return obj.ObjWithoutAllPerms(dag_tasks, tasks_info, chain_perm,
                                      variable_od);
    }
};
}  // namespace DAG_SPACE