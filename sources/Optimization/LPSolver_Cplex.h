#pragma once

#include "ilcplex/cplex.h"
#include "ilcplex/ilocplex.h"
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
#include "sources/Permutations/ChainsPermutation.h"
#include "sources/Optimization/GraphOfChains.h"
#include "sources/TaskModel/DAG_Model.h"

namespace DAG_SPACE {
// TODO: make it work with templates
// rta is organized by task_id
class LPOptimizer {
   public:
    LPOptimizer(const DAG_Model &dag_tasks,
                const TaskSetInfoDerived &tasks_info,
                // const ChainsPermutation &chains_perm,
                const GraphOfChains &graph_of_all_ca_chains,
                const std::string &obj_trait, const std::vector<int> &rta)
        : dag_tasks_(dag_tasks),
          tasks_info_(tasks_info),
          //   chains_perm_(chains_perm),
          graph_of_all_ca_chains_(graph_of_all_ca_chains),
          obj_trait_(obj_trait),
          rta_(rta),
          // cplex's environments
          env_(IloEnv()),
          model_(env_),
          cplexSolver_(env_),
          constraint_array_(env_) {
        variable_od_opt_.valid_ = false;
        cplexSolver_.setOut(env_.getNullStream());
        name2ilo_const_.reserve(1e3);
        react_chain_map_prevs_.reserve(dag_tasks.chains_.size());
    }

    void Init();
    void ClearCplexMemory();
    std::pair<VariableOD, int> Optimize(const ChainsPermutation &chains_perm);
    std::pair<VariableOD, int> OptimizeWithoutClear(
        const ChainsPermutation &chains_perm);
    std::pair<VariableOD, int> OptimizeAfterUpdate(
        const ChainsPermutation &chains_perm);

    inline const std::pair<VariableOD, int> IncrementOptimize(
        const ChainsPermutation &chains_perm) {
        UpdateSystem(chains_perm);
        return OptimizeAfterUpdate(chains_perm);
    }

    inline void AddVariables() {
        AddVariablesOD(tasks_info_.N);
        AddArtificialVariables();
    }

    int FindMinOffset(int task_id, const ChainsPermutation &chains_perm);

    void AddVariablesOD(int number_of_tasks_to_opt);
    void AddArtificialVariables();
    void AddPermutationInequalityConstraints(
        const ChainsPermutation &chains_perm, bool allow_partial_edges = false);
    void AddSchedulabilityConstraints();

    void AddConstantObjectiveFunctions(const ChainsPermutation &chains_perm);
    void AddObjectiveFunctions(
        const ChainsPermutation &chains_perm);  // RTDA obj
    // void AddObjectiveFunctionDataAge();
    // void AddObjectiveFunctionReactionTime();

    void UpdatePermutationInequalityConstraints(
        const ChainsPermutation &chains_perm);

    void UpdateObjectiveFunctions(const ChainsPermutation &chains_perm);

    inline void UpdateSystem(const ChainsPermutation &chains_perm) {
        UpdatePermutationInequalityConstraints(chains_perm);
        UpdateObjectiveFunctions(chains_perm);
        // chains_perm_ = chains_perm;
    }

    IloExpr GetStartTimeExpression(JobCEC &jobCEC);
    IloExpr GetFinishTimeExpression(JobCEC &jobCEC);
    IloExpr GetStartTimeExpressionApprox(JobCEC &jobCEC);
    IloExpr GetFinishTimeExpressionApprox(JobCEC &jobCEC);
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

    inline void WriteModelToFile(const std::string file_name = "LP_Model.lp") {
        cplexSolver_.extract(model_);
        cplexSolver_.exportModel(file_name.c_str());
    }
    inline std::string GetPermuIneqConstraintNamePrev(int edge_id) {
        return "perm_constraint_" + std::to_string(edge_id * 2);
    }
    inline std::string GetPermuIneqConstraintNameNext(int edge_id) {
        return "perm_constraint_" + std::to_string(edge_id * 2 + 1);
    }
    inline std::string GetReactConstraintName(int chain_id, int job_id) {
        return "chain_" + std::to_string(chain_id) + "constraint_" +
               std::to_string(job_id);
    }

   public:
    const DAG_Model &dag_tasks_;
    const TaskSetInfoDerived &tasks_info_;
    // const ChainsPermutation &chains_perm_;
    const GraphOfChains &graph_of_all_ca_chains_;
    VariableOD variable_od_opt_;
    int numVariables_;
    std::string obj_trait_;
    std::vector<std::unordered_map<JobCEC, JobCEC>> react_chain_map_prevs_;
    const std::vector<int> &rta_;

    IloEnv env_;
    IloModel model_;
    IloCplex cplexSolver_;
    IloNumVarArray varArray_;
    IloNumVarArray varArray_art_;
    IloConstraintArray constraint_array_;
    int optimal_obj_ = 1e8;
    std::unordered_map<std::string, IloRange> name2ilo_const_;
    std::unordered_map<std::string, IloNumVar> name2ilo_var_;
    // std::unordered_map<int, uint> task_id2position_cplex_;
};

inline std::pair<VariableOD, int> FindODWithLP(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
    const ChainsPermutation &chains_perm,
    const GraphOfChains &graph_of_all_ca_chains, const std::string &obj_trait,
    const std::vector<int> &rta) {
    LPOptimizer lp_optimizer(dag_tasks, tasks_info, graph_of_all_ca_chains,
                             obj_trait, rta);
    return lp_optimizer.Optimize(chains_perm);
}

inline int GetMinOffSet(int task_id, const DAG_Model &dag_tasks,
                        const TaskSetInfoDerived &tasks_info,
                        const ChainsPermutation &chains_perm,
                        const GraphOfChains &graph_of_all_ca_chains,
                        const std::string &obj_trait,
                        const std::vector<int> &rta) {
    LPOptimizer lp_optimizer(dag_tasks, tasks_info, graph_of_all_ca_chains,
                             obj_trait, rta);
    int res = lp_optimizer.FindMinOffset(task_id, chains_perm);
    lp_optimizer.ClearCplexMemory();
    return res;
}
// inline bool ExamFeasibility(
//     const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
//     const ChainsPermutation &chains_perm,
//     const GraphOfChains &graph_of_all_ca_chains, const std::string
//     &obj_trait, const std::unordered_map<JobCEC, JobCEC> &react_chain_map,
//     const std::vector<int> &rta) {
//     LPOptimizer lp_optimizer(dag_tasks, tasks_info, chains_perm,
//                              graph_of_all_ca_chains, obj_trait,
//                              react_chain_map, rta);
//     return lp_optimizer.OptimizeConstant().first.valid_;
// }
}  // namespace DAG_SPACE