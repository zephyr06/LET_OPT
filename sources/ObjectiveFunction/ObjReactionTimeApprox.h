#pragma once
#include "sources/ObjectiveFunction/ObjReactionTime.h"

namespace DAG_SPACE {

class ObjReactionTimeApprox {
 public:
  static const std::string type_trait;
  static double Obj(const DAG_Model &dag_tasks,
                    const TaskSetInfoDerived &tasks_info,
                    const ChainsPermutation &chains_perm,
                    const VariableOD &variable_od,
                    const std::vector<std::vector<int>> &chains_to_analyze) {
    ObjReactionTimeIntermediate obj;
    VariableOD variable_od_org(dag_tasks.GetTaskSet());
    return obj.Obj(dag_tasks, tasks_info, chains_perm, variable_od_org,
                   chains_to_analyze);
  }
  static std::vector<double> ObjPerChain(
      const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
      const ChainsPermutation &chains_perm, const VariableOD &variable_od,
      const std::vector<std::vector<int>> &chains_to_analyze) {
    ObjReactionTimeIntermediate obj;
    VariableOD variable_od_org(dag_tasks.GetTaskSet());
    return obj.ObjPerChain(dag_tasks, tasks_info, chains_perm, variable_od_org,
                           chains_to_analyze);
  }
};

}  // namespace DAG_SPACE