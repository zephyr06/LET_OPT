#pragma once

#include "sources/ObjectiveFunction/ObjectiveFunctionBase.h"

namespace DAG_SPACE {

class ObjSensorFusion {
   public:
    static const std::string type_trait;
    static double Obj(const DAG_Model &dag_tasks,
                      const TaskSetInfoDerived &tasks_info,
                      const ChainsPermutation &chain_perm,
                      const VariableOD &variable_od);
};

}  // namespace DAG_SPACE