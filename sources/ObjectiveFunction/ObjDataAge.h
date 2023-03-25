#pragma once
#include "sources/ObjectiveFunction/ObjReactionTime.h"
namespace DAG_SPACE {

std::unordered_map<JobCEC, JobCEC> GetFirstReactMap(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chains_perm, const std::vector<int> &chain);

class ObjDataAgeIntermediate : public ObjectiveFunctionBaseIntermediate {
   public:
    static const std::string type_trait;
    double ObjSingleChain(const DAG_Model &dag_tasks,
                          const TaskSetInfoDerived &tasks_info,
                          const ChainsPermutation &chains_perm,
                          const std::vector<int> &chain,
                          const VariableOD &variable_od) override;

    double ObjWithoutAllPermsSingleChain(
        const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
        const ChainsPermutation &chains_perm, const std::vector<int> &chain,
        const VariableOD &variable_od) override;
};

class ObjDataAge {
   public:
    static const std::string type_trait;
    static double Obj(const DAG_Model &dag_tasks,
                      const TaskSetInfoDerived &tasks_info,
                      const ChainsPermutation &chains_perm,
                      const VariableOD &variable_od,
                      const std::vector<std::vector<int>> &chains_to_analyze) {
        ObjDataAgeIntermediate obj;
        return obj.Obj(dag_tasks, tasks_info, chains_perm, variable_od,chains_to_analyze);
    }

    static double ObjWithoutAllPermsSingleChain(
        const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
        const ChainsPermutation &chains_perm, const std::vector<int> &chain,
        const VariableOD &variable_od,const std::vector<std::vector<int>> &chains_to_analyze) {
        ObjDataAgeIntermediate obj;
        return obj.ObjWithoutAllPerms(dag_tasks, tasks_info, chains_perm,
                                      variable_od,chains_to_analyze);
    }
};
}  // namespace DAG_SPACE