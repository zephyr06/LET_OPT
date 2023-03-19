#pragma once

#include "ilcplex/cplex.h"
#include "ilcplex/ilocplex.h"
#include "sources/Optimization/ChainsPermutation.h"
#include "sources/Optimization/GraphOfChains.h"
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
#include "sources/TaskModel/DAG_Model.h"

namespace DAG_SPACE {
// TODO: make it work with templates
// rta is organized by task_id
class LPOptimizer {
   public:
    LPOptimizer(const DAG_Model &dag_tasks,
                const TaskSetInfoDerived &tasks_info,
                const ChainsPermutation &chains_perm,
                const GraphOfChains &graph_of_all_ca_chains,
                const std::string &obj_trait,
                const std::unordered_map<JobCEC, JobCEC> &react_chain_map,
                const std::vector<int> &rta)
        : dag_tasks_(dag_tasks),
          tasks_info_(tasks_info),
          chains_perm_(chains_perm),
          graph_of_all_ca_chains_(graph_of_all_ca_chains),
          obj_trait_(obj_trait),
          react_chain_map_(react_chain_map),
          rta_(rta) {
        env_.end();
    }

    void Init();
    void ClearCplexMemory();
    std::pair<VariableOD, int> Optimize();

    // protected:
    void AddVariables();
    void AddPermutationInequalityConstraints();
    void AddSchedulabilityConstraints();

    void AddObjectiveFunctions();  // RTDA obj
    void AddObjectiveFunctionDataAge();
    void AddObjectiveFunctionReactionTime();
    IloExpr GetStartTimeExpression(JobCEC &jobCEC);
    IloExpr GetFinishTimeExpression(JobCEC &jobCEC);
    VariableOD ExtratOptSolution(IloNumArray &values_optimized);

    inline int GetVariableIndexVirtualOffset(int task_id) {
        return task_id * 2;
    }
    inline int GetVariableIndexVirtualOffset(const JobCEC &job) {
        return GetVariableIndexVirtualOffset(job.taskId);
    }
    inline int GetVariableIndexVirtualDeadline(int task_id) {
        return task_id * 2 + 1;
    }
    inline int GetVariableIndexVirtualDeadline(const JobCEC &job) {
        return GetVariableIndexVirtualDeadline(job.taskId);
    }

   public:
    DAG_Model dag_tasks_;
    TaskSetInfoDerived tasks_info_;
    ChainsPermutation chains_perm_;
    GraphOfChains graph_of_all_ca_chains_;
    VariableOD variable_od_opt_;
    int numVariables_;
    std::string obj_trait_;
    std::unordered_map<JobCEC, JobCEC> react_chain_map_;  // TODO: make it work
    std::vector<int> rta_;

    IloEnv env_;
    IloModel model_;
    IloCplex cplexSolver_;
    // we organize the variables following task-ids, e.g., o_0,d_0,o_1,d_1,...,
    IloNumVarArray varArray_;
    VectorDynamic initialStartTimeVector_;
    VectorDynamic optimizedStartTimeVector_;
    int optimal_obj_ = 1e8;
    // std::unordered_map<int, uint> task_id2position_cplex_;
};

inline std::pair<VariableOD, int> FindODWithLP(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chains_perm,
    const GraphOfChains &graph_of_all_ca_chains, const std::string &obj_trait,
    const std::unordered_map<JobCEC, JobCEC> &react_chain_map,
    const std::vector<int> &rta) {
    LPOptimizer lp_optimizer(dag_tasks, tasks_info, chains_perm,
                             graph_of_all_ca_chains, obj_trait, react_chain_map,
                             rta);
    return lp_optimizer.Optimize();
}
}  // namespace DAG_SPACE