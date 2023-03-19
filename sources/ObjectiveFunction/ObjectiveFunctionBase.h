#pragma once
#include "sources/Optimization/ChainsPermutation.h"
#include "sources/Optimization/Variable.h"
#include "sources/Utils/profilier.h"
namespace DAG_SPACE {

class ObjectiveFunctionBaseIntermediate {
   public:
    static const std::string type_trait;

    virtual double ObjSingleChain(const DAG_Model &dag_tasks,
                                  const TaskSetInfoDerived &tasks_info,
                                  const ChainsPermutation &chain_perm,
                                  const std::vector<int> &chain,
                                  const VariableOD &variable_od) {
        CoutError("Base function should not be called!");
        return 0;
    }

    double Obj(const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
               const ChainsPermutation &chain_perm,
               const VariableOD &variable_od);

    virtual double ObjWithoutAllPermsSingleChain(
        const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
        const ChainsPermutation &chain_perm, const std::vector<int> &chain,
        const VariableOD &variable_od) {
        CoutError("Base function should not be called!");
        return 0;
    }

    double ObjWithoutAllPerms(const DAG_Model &dag_tasks,
                              const TaskSetInfoDerived &tasks_info,
                              const ChainsPermutation &chain_perm,
                              const VariableOD &variable_od);
};

}  // namespace DAG_SPACE