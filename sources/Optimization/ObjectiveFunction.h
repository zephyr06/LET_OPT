#pragma once
#include "sources/Optimization/ChainsPermutation.h"
#include "sources/Optimization/Variable.h"
#include "sources/Utils/profilier.h"
namespace DAG_SPACE
{

    // task_index_in_chain: the index of a task in a cause-effect chain
    // for example: consider a chain 0->1->2, task0's index is 0, task1's index is
    // 1, task2's index is 2;
    JobCEC GetFirstReactJobWithSuperPeriod(
        const JobCEC &job_curr, const SinglePairPermutation &pair_perm_curr);

    JobCEC GetFirstReactJob(const JobCEC &job_curr,
                            const ChainsPermutation &chain_perm,
                            const Edge &edge_curr,
                            const TaskSetInfoDerived &tasks_info);

    std::unordered_map<JobCEC, JobCEC> GetFirstReactMap(
        const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
        const ChainsPermutation &chain_perm, const std::vector<int> &chain);

    class ObjectiveFunctionBaseIntermediate
    {
    public:
        static const std::string type_trait;

        virtual double ObjSingleChain(const DAG_Model &dag_tasks,
                                      const TaskSetInfoDerived &tasks_info,
                                      const ChainsPermutation &chain_perm,
                                      const std::vector<int> &chain,
                                      const VariableOD &variable_od)
        {
            CoutError("Base function should not be called!");
            return 0;
        }

        double Obj(const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
                   const ChainsPermutation &chain_perm,
                   const VariableOD &variable_od);
    };

    class ObjReactionTimeIntermediate : public ObjectiveFunctionBaseIntermediate
    {
    public:
        static const std::string type_trait;
        double ObjSingleChain(const DAG_Model &dag_tasks,
                              const TaskSetInfoDerived &tasks_info,
                              const ChainsPermutation &chain_perm,
                              const std::vector<int> &chain,
                              const VariableOD &variable_od) override;
    };

    class ObjDataAgeIntermediate : public ObjectiveFunctionBaseIntermediate
    {
    public:
        static const std::string type_trait;
        double ObjSingleChain(const DAG_Model &dag_tasks,
                              const TaskSetInfoDerived &tasks_info,
                              const ChainsPermutation &chain_perm,
                              const std::vector<int> &chain,
                              const VariableOD &variable_od) override;
    };

    class ObjReactionTime
    {
    public:
        static const std::string type_trait;
        static double Obj(const DAG_Model &dag_tasks,
                          const TaskSetInfoDerived &tasks_info,
                          const ChainsPermutation &chain_perm,
                          const VariableOD &variable_od)
        {
            ObjReactionTimeIntermediate obj;
            return obj.Obj(dag_tasks, tasks_info, chain_perm, variable_od);
        }
    };

    class ObjDataAge
    {
    public:
        static const std::string type_trait;
        static double Obj(const DAG_Model &dag_tasks,
                          const TaskSetInfoDerived &tasks_info,
                          const ChainsPermutation &chain_perm,
                          const VariableOD &variable_od)
        {
            ObjDataAgeIntermediate obj;
            return obj.Obj(dag_tasks, tasks_info, chain_perm, variable_od);
        }
    };

    class ObjSensorFusion
    {
    public:
        static const std::string type_trait;
        static double Obj(const DAG_Model &dag_tasks,
                          const TaskSetInfoDerived &tasks_info,
                          const ChainsPermutation &chain_perm,
                          const VariableOD &variable_od);
    };

} // namespace DAG_SPACE