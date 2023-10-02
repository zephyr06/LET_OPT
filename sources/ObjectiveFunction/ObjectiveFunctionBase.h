#pragma once
#include "sources/Baseline/JobScheduleInfo.h"
#include "sources/Optimization/Variable.h"
#include "sources/Permutations/ChainsPermutation.h"
#include "sources/Utils/profilier.h"
namespace DAG_SPACE {

inline double JitterOfVector(const std::vector<double> &data) {
  if (data.size() == 0)
    return 0;
  else {
    return *max_element(data.begin(), data.end()) -
           *min_element(data.begin(), data.end());
  }
}

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

  virtual std::vector<double> ObjAllInstances(
      const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
      const ChainsPermutation &chains_perm, const std::vector<int> &chain,
      const VariableOD &variable_od) {
    CoutError("Base function should not be called!");
    return {0};
  }

  virtual std::vector<double> ObjAllInstances(
      const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
      const ChainsPermutation &chains_perm, const std::vector<int> &chain,
      const Schedule &schedule) {
    CoutError("Base function should not be called!");
    return {0};
  }

  std::vector<double> ObjAllChains(
      const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
      const ChainsPermutation &chains_perm, const VariableOD &variable_od,
      const std::vector<std::vector<int>> &chains_to_analyze);

  // overload for schedule arguments
  std::vector<double> ObjAllChains(
      const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
      const ChainsPermutation &chains_perm, const Schedule &schedule,
      const std::vector<std::vector<int>> &chains_to_analyze);

  inline double Obj(const DAG_Model &dag_tasks,
                    const TaskSetInfoDerived &tasks_info,
                    const ChainsPermutation &chains_perm,
                    const VariableOD &variable_od,
                    const std::vector<std::vector<int>> &chains_to_analyze) {
    std::vector<double> obj_vec = ObjAllChains(
        dag_tasks, tasks_info, chains_perm, variable_od, chains_to_analyze);
    int max_obj = std::accumulate(obj_vec.begin(), obj_vec.end(), 0);
    return max_obj;
  }

  // overload for schedule arguments
  inline double Obj(const DAG_Model &dag_tasks,
                    const TaskSetInfoDerived &tasks_info,
                    const ChainsPermutation &chains_perm,
                    const Schedule &schedule,
                    const std::vector<std::vector<int>> &chains_to_analyze) {
    std::vector<double> obj_vec = ObjAllChains(
        dag_tasks, tasks_info, chains_perm, schedule, chains_to_analyze);
    int max_obj = std::accumulate(obj_vec.begin(), obj_vec.end(), 0);

    return max_obj;
  }

  // functions related to jitter --------------------------------------
  double JitterSingleChain(const DAG_Model &dag_tasks,
                           const TaskSetInfoDerived &tasks_info,
                           const ChainsPermutation &chains_perm,
                           const std::vector<int> &chain,
                           const VariableOD &variable_od) {
    std::vector<double> jitters =
        ObjAllInstances(dag_tasks, tasks_info, chains_perm, chain, variable_od);
    return JitterOfVector(jitters);
  }
  double JitterSingleChain(const DAG_Model &dag_tasks,
                           const TaskSetInfoDerived &tasks_info,
                           const ChainsPermutation &chains_perm,
                           const std::vector<int> &chain,
                           const Schedule &schedule) {
    std::vector<double> jitters =
        ObjAllInstances(dag_tasks, tasks_info, chains_perm, chain, schedule);
    return JitterOfVector(jitters);
  }

  double Jitter(const DAG_Model &dag_tasks,
                const TaskSetInfoDerived &tasks_info,
                const ChainsPermutation &chains_perm,
                const VariableOD &variable_od,
                const std::vector<std::vector<int>> &chains_to_analyze) {
    double sum = 0;
    for (const std::vector<int> &chain : chains_to_analyze)
      sum += JitterSingleChain(dag_tasks, tasks_info, chains_perm, chain,
                               variable_od);
    return sum;
  }

  double Jitter(const DAG_Model &dag_tasks,
                const TaskSetInfoDerived &tasks_info,
                const ChainsPermutation &chains_perm, const Schedule &schedule,
                const std::vector<std::vector<int>> &chains_to_analyze) {
    double sum = 0;
    for (const std::vector<int> &chain : chains_to_analyze)
      sum += JitterSingleChain(dag_tasks, tasks_info, chains_perm, chain,
                               schedule);
    return sum;
  }
};

}  // namespace DAG_SPACE