#pragma once

#include "ilcplex/cplex.h"
#include "ilcplex/ilocplex.h"
#include "sources/Optimization/TaskSetPermutations.h"
#include "sources/TaskModel/DAG_Model.h"

namespace DAG_SPACE {
// TODO: make it work with templates
class LPOptimizer {
   public:
    LPOptimizer(const DAG_Model &dagTasks, const TaskSetInfoDerived &tasks_info,
                const ChainsPermutation &chains_perm,
                const GraphOfChains &graph_of_all_ca_chains,
                const VariableOD &variable_od, const std::string &obj_trait)
        : dag_tasks_(dagTasks),
          tasks_info_(tasks_info_),
          chains_perm_(chains_perm),
          graph_of_all_ca_chains_(graph_of_all_ca_chains),
          variable_od_(variable_od),
          obj_trait_(obj_trait) {
        env_.end();
    }

    void Init();
    void ClearCplexMemory();
    void Optimize();

    // protected:
    void AddVariables();
    void AddPermutationInequalityConstraints();
    void AddSchedulabilityConstraints();

    void AddObjectiveFunctions();  // RTDA obj
    IloExpr GetStartTimeExpression(JobCEC &jobCEC);
    IloExpr GetFinishTimeExpression(JobCEC &jobCEC);
    void UpdateOptimizedStartTimeVector(IloNumArray &values_optimized);

    // default values if infeasible
    inline void SetOptimizedStartTimeVector() {
        optimizedStartTimeVector_ =
            GenerateVectorDynamic(tasks_info_.variableDimension);
    }

   public:
    DAG_Model dag_tasks_;
    TaskSetInfoDerived tasks_info_;
    ChainsPermutation chains_perm_;
    GraphOfChains graph_of_all_ca_chains_;
    VariableOD variable_od_;
    int numVariables_;
    std::string obj_trait_;

    IloEnv env_;
    IloModel model_;
    IloCplex cplexSolver_;
    // we organize the variables following task-ids, e.g., o_0,d_0,o_1,d_1,...,
    IloNumVarArray varArray_;
    VectorDynamic initialStartTimeVector_;
    VectorDynamic optimizedStartTimeVector_;
    // std::unordered_map<int, uint> task_id2position_cplex_;
};
}  // namespace DAG_SPACE