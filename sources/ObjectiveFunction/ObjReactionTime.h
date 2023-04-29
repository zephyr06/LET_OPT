#pragma once
#include "sources/ObjectiveFunction/ObjectiveFunctionBase.h"

namespace DAG_SPACE {

// task_index_in_chain: the index of a task in a cause-effect chain
// for example: consider a chain 0->1->2, task0's index is 0, task1's index is
// 1, task2's index is 2;
JobCEC GetFirstReactJobWithSuperPeriod(
    const JobCEC &job_curr, const SinglePairPermutation &pair_perm_curr);

JobCEC GetFirstReactJob(const JobCEC &job_curr,
                        const SinglePairPermutation &pair_perm_curr,
                        const TaskSetInfoDerived &tasks_info);

JobCEC GetFirstReactJob(const JobCEC job_source,
                        const ChainsPermutation &chains_perm,
                        const std::vector<int> &chain,
                        const TaskSetInfoDerived &tasks_info);

class ObjReactionTimeIntermediate : public ObjectiveFunctionBaseIntermediate {
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
};

class ObjReactionTime {
 public:
  static const std::string type_trait;
  static double Obj(const DAG_Model &dag_tasks,
                    const TaskSetInfoDerived &tasks_info,
                    const ChainsPermutation &chains_perm,
                    const VariableOD &variable_od,
                    const std::vector<std::vector<int>> &chains_to_analyze) {
    ObjReactionTimeIntermediate obj;
    return obj.Obj(dag_tasks, tasks_info, chains_perm, variable_od,
                   chains_to_analyze);
  }
  static double Obj(const DAG_Model &dag_tasks,
                    const TaskSetInfoDerived &tasks_info,
                    const ChainsPermutation &chains_perm,
                     const Schedule &schedule,
                    const std::vector<std::vector<int>> &chains_to_analyze) {
    ObjReactionTimeIntermediate obj;
    return obj.Obj(dag_tasks, tasks_info, chains_perm, schedule,
                   chains_to_analyze);
  }
  static std::vector<double> ObjPerChain(
      const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
      const ChainsPermutation &chains_perm, const VariableOD &variable_od,
      const std::vector<std::vector<int>> &chains_to_analyze) {
    ObjReactionTimeIntermediate obj;
    return obj.ObjPerChain(dag_tasks, tasks_info, chains_perm, variable_od,
                           chains_to_analyze);
  }
};

}  // namespace DAG_SPACE