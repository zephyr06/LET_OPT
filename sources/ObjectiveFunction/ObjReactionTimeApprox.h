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
    return 0;
    ObjReactionTimeIntermediate obj;
    // VariableOD variable_od_org(dag_tasks.GetTaskSet());
    return obj.Obj(dag_tasks, tasks_info, chains_perm, variable_od,
                   chains_to_analyze);
    }
  static std::vector<double> ObjAllChains(
      const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
      const ChainsPermutation &chains_perm, const VariableOD &variable_od,
      const std::vector<std::vector<int>> &chains_to_analyze) {
    ObjReactionTimeIntermediate obj;
    VariableOD variable_od_org(dag_tasks.GetTaskSet());
    return obj.ObjAllChains(dag_tasks, tasks_info, chains_perm, variable_od_org,
                            chains_to_analyze);
  }

  static double Jitter(const DAG_Model &dag_tasks,
                       const TaskSetInfoDerived &tasks_info,
                       const ChainsPermutation & /*unusedArg*/,
                       const Schedule &schedule,
                       const std::vector<std::vector<int>> /*unusedArg*/) {
    CoutWarning("Jitter Not implemented yet!");
    return INT32_MAX;
  }
  static double Jitter(const DAG_Model &dag_tasks,
                       const TaskSetInfoDerived &tasks_info,
                       const ChainsPermutation &chains_perm,
                       const VariableOD &variable_od,
                       const std::vector<std::vector<int>> &chains_to_analyze) {
    CoutWarning("Jitter Not implemented yet!");
    return INT32_MAX;
  }
};

}  // namespace DAG_SPACE