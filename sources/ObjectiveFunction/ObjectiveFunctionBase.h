#pragma once
#include "sources/Baseline/JobScheduleInfo.h"
#include "sources/Optimization/Variable.h"
#include "sources/Permutations/ChainsPermutation.h"
#include "sources/Utils/profilier.h"
namespace DAG_SPACE {

class ObjectiveFunctionBaseIntermediate {
 public:
  static const std::string type_trait;

  virtual double ObjSingleChain(const DAG_Model &dag_tasks,
                                const TaskSetInfoDerived &tasks_info,
                                const ChainsPermutation &chains_perm,
                                const std::vector<int> &chain,
                                const VariableOD &variable_od) {
    CoutError("Base function should not be called!");
    return 0;
  }
  virtual double ObjSingleChain(const DAG_Model &dag_tasks,
                                const TaskSetInfoDerived &tasks_info,
                                const ChainsPermutation &chains_perm,
                                const std::vector<int> &chain,
                                const Schedule &schedule) {
    CoutError("Base function should not be called!");
    return 0;
  }

  std::vector<double> ObjPerChain(
      const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
      const ChainsPermutation &chains_perm, const VariableOD &variable_od,
      const std::vector<std::vector<int>> &chains_to_analyze);
  // overload for schedule arguments
  std::vector<double> ObjPerChain(
      const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
      const ChainsPermutation &chains_perm, const Schedule &schedule,
      const std::vector<std::vector<int>> &chains_to_analyze);

  inline double Obj(const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
             const ChainsPermutation &chains_perm,
             const VariableOD &variable_od,
             const std::vector<std::vector<int>> &chains_to_analyze) {
    std::vector<double> obj_vec = ObjPerChain(
        dag_tasks, tasks_info, chains_perm, variable_od, chains_to_analyze);
    int max_obj = std::accumulate(obj_vec.begin(), obj_vec.end(), 0);
    return max_obj;
  }
  // overload for schedule arguments
  inline double Obj(const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
             const ChainsPermutation &chains_perm, const Schedule &schedule,
             const std::vector<std::vector<int>> &chains_to_analyze) {
    std::vector<double> obj_vec = ObjPerChain(
        dag_tasks, tasks_info, chains_perm, schedule, chains_to_analyze);
    int max_obj = std::accumulate(obj_vec.begin(), obj_vec.end(), 0);

    return max_obj;
  }
};

}  // namespace DAG_SPACE