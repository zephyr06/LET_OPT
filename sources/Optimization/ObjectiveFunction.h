#pragma once
#include "sources/Optimization/ChainPermutation.h"
#include "sources/Optimization/Variable.h"
namespace DAG_SPACE {

class ObjectiveFunctionBase {
   public:
    static const std::string type_trait;

    static double Obj(const DAG_Model &dag_tasks,
                      const TaskSetInfoDerived &tasks_info,
                      const ChainPermutation &chain_perm,
                      const VariableOD &variable_od) {
        CoutError("Base function should not be called!");
        return 0;
    }
};

class ObjReactionTime : public ObjectiveFunctionBase {
   public:
    static const std::string type_trait;
    static double Obj(const DAG_Model &dag_tasks,
                      const TaskSetInfoDerived &tasks_info,
                      const ChainPermutation &chain_perm,
                      const VariableOD &variable_od);
};

// task_index_in_chain: the index of a task in a cause-effect chain
// for example: consider a chain 0->1->2, task0's index is 0, task1's index is
// 1, task2's index is 2;
JobCEC GetFirstReactJobWithSuperPeriod(const JobCEC &job_curr,
                                       const ChainPermutation &chain_perm,
                                       uint task_index_in_chain);

JobCEC GetFirstReactJob(const JobCEC &job_curr,
                        const ChainPermutation &chain_perm,
                        uint task_index_in_chain,
                        const TaskSetInfoDerived &tasks_info);
}  // namespace DAG_SPACE