#include "sources/ObjectiveFunction/ObjectiveFunctionBase.h"

namespace DAG_SPACE {

const std::string ObjectiveFunctionBaseIntermediate::type_trait(
    "ObjectiveFunctionBase");

std::vector<double> ObjectiveFunctionBaseIntermediate::ObjPerChain(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chains_perm, const VariableOD &variable_od,
    const std::vector<std::vector<int>> &chains_to_analyze) {
#ifdef PROFILE_CODE
  BeginTimer(__FUNCTION__);
#endif
  std::vector<double> obj_per_chain;
  obj_per_chain.reserve(chains_to_analyze.size());
  for (const auto &chain : chains_to_analyze) {
    if (chain.size() == 0) continue;
    obj_per_chain.push_back(
        ObjSingleChain(dag_tasks, tasks_info, chains_perm, chain, variable_od));
  }
#ifdef PROFILE_CODE
  EndTimer(__FUNCTION__);
#endif
  return obj_per_chain;
}
std::vector<double> ObjectiveFunctionBaseIntermediate::ObjPerChain(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chains_perm, const Schedule &schedule,
    const std::vector<std::vector<int>> &chains_to_analyze) {
  std::vector<double> obj_per_chain;
  obj_per_chain.reserve(chains_to_analyze.size());
  for (const auto &chain : chains_to_analyze) {
    if (chain.size() == 0) continue;
    obj_per_chain.push_back(
        ObjSingleChain(dag_tasks, tasks_info, chains_perm, chain, schedule));
  }
  return obj_per_chain;
}

// double ObjectiveFunctionBaseIntermediate::Obj(
//     const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
//     const ChainsPermutation &chains_perm, const VariableOD &variable_od,
//     const std::vector<std::vector<int>> &chains_to_analyze) {
//   std::vector<double> obj_vec = ObjPerChain(dag_tasks, tasks_info,
//   chains_perm,
//                                             variable_od, chains_to_analyze);
//   int max_obj = std::accumulate(obj_vec.begin(), obj_vec.end(), 0);
//   return max_obj;
// }

// double ObjectiveFunctionBaseIntermediate::Obj(
//     const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
//     const ChainsPermutation &chains_perm, const Schedule &schedule,
//     const std::vector<std::vector<int>> &chains_to_analyze) {
//   std::vector<double> obj_vec = ObjPerChain(dag_tasks, tasks_info,
//   chains_perm,
//                                             schedule, chains_to_analyze);
//   int max_obj = std::accumulate(obj_vec.begin(), obj_vec.end(), 0);

//   return max_obj;
// }

}  // namespace DAG_SPACE