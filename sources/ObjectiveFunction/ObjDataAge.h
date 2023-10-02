#pragma once
#include "sources/ObjectiveFunction/ObjReactionTime.h"
namespace DAG_SPACE {

JobCEC GetLastReadJobWithSuperPeriod(
    const JobCEC &job_curr, const SinglePairPermutation &pair_perm_curr);

JobCEC GetLastReadJob(const JobCEC &job_curr,
                      const SinglePairPermutation &pair_perm_curr,
                      const TaskSetInfoDerived &tasks_info);

JobCEC GetLastReadJob(const JobCEC job_source,
                      const ChainsPermutation &chains_perm,
                      const std::vector<int> &chain,
                      const TaskSetInfoDerived &tasks_info);

class ObjDataAgeIntermediate : public ObjectiveFunctionBaseIntermediate {
 public:
  static const std::string type_trait;
  double ObjSingleChain(const DAG_Model &dag_tasks,
                        const TaskSetInfoDerived &tasks_info,
                        const ChainsPermutation &chains_perm,
                        const std::vector<int> &chain,
                        const VariableOD &variable_od) override;
  double ObjSingleChain(const DAG_Model &dag_tasks,
                        const TaskSetInfoDerived &tasks_info,
                        const ChainsPermutation &chains_perm,
                        const std::vector<int> &chain,
                        const Schedule &schedule) override;
  std::vector<double> ObjAllInstances(const DAG_Model &dag_tasks,
                                      const TaskSetInfoDerived &tasks_info,
                                      const ChainsPermutation &chains_perm,
                                      const std::vector<int> &chain,
                                      const VariableOD &variable_od) override;
  std::vector<double> ObjAllInstances(const DAG_Model &dag_tasks,
                                      const TaskSetInfoDerived &tasks_info,
                                      const ChainsPermutation &chains_perm,
                                      const std::vector<int> &chain,
                                      const Schedule &schedule) override;
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
    return obj.Obj(dag_tasks, tasks_info, chains_perm, variable_od,
                   chains_to_analyze);
  }
  static double Obj(const DAG_Model &dag_tasks,
                    const TaskSetInfoDerived &tasks_info,
                    const ChainsPermutation & /* not used*/,
                    const Schedule &schedule,
                    const std::vector<std::vector<int>> &chains_to_analyze) {
    return ObjDataAgeFromSChedule(dag_tasks, tasks_info, chains_to_analyze,
                                  schedule);
  }

  static std::vector<double> ObjAllChains(
      const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
      const ChainsPermutation &chains_perm, const VariableOD &variable_od,
      const std::vector<std::vector<int>> &chains_to_analyze) {
    ObjDataAgeIntermediate obj;
    return obj.ObjAllChains(dag_tasks, tasks_info, chains_perm, variable_od,
                            chains_to_analyze);
  }

  static double Jitter(const DAG_Model &dag_tasks,
                       const TaskSetInfoDerived &tasks_info,
                       const ChainsPermutation &chains_perm,
                       const VariableOD &variable_od,
                       const std::vector<std::vector<int>> &chains_to_analyze) {
    ObjDataAgeIntermediate obj;
    return obj.Jitter(dag_tasks, tasks_info, chains_perm, variable_od,
                      chains_to_analyze);
  }

  static double Jitter(const DAG_Model &dag_tasks,
                       const TaskSetInfoDerived &tasks_info,
                       const ChainsPermutation &chains_perm,
                       const Schedule &schedule,
                       const std::vector<std::vector<int>> &chains_to_analyze) {
    ObjDataAgeIntermediate obj;
    return obj.Jitter(dag_tasks, tasks_info, chains_perm, schedule,
                      chains_to_analyze);
  }
};
}  // namespace DAG_SPACE