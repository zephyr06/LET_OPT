#pragma once
#include "sources/ObjectiveFunction/ObjDataAge.h"
namespace DAG_SPACE {
// TODO: test this class!
class ObjDataAgeApprox {
 public:
  static const std::string type_trait;
  static double Obj(const DAG_Model &dag_tasks,
                    const TaskSetInfoDerived &tasks_info,
                    const ChainsPermutation &chains_perm,
                    const VariableOD &variable_od,
                    const std::vector<std::vector<int>> &chains_to_analyze) {
    ObjDataAgeIntermediate obj;
    VariableOD variable_od_org(dag_tasks.GetTaskSet());
    return obj.Obj(dag_tasks, tasks_info, chains_perm, variable_od_org,
                   chains_to_analyze);
  }
};
}  // namespace DAG_SPACE