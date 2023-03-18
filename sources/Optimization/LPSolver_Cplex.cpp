#include "sources/Optimization/LPSolver_Cplex.h"

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "sources/Utils/Parameters.h"

namespace DAG_SPACE {
void LPOptimizer::Init() {
    env_ = IloEnv();
    model_ = IloModel(env_);
    cplexSolver_ = IloCplex(env_);
    cplexSolver_.setOut(env_.getNullStream());
    SetOptimizedStartTimeVector();
}

void LPOptimizer::ClearCplexMemory() {
    // release memory
    cplexSolver_.end();
    model_.end();
    env_.end();
}

VectorDynamic LPOptimizer::Optimize() {
    // BeginTimer("Build_LP_Model");
    Init();
    AddVariables();  // must be called first
    AddPermutationInequalityConstraints();
    AddObjectiveFunctions();
    cplexSolver_.extract(model_);
    // EndTimer("Build_LP_Model");

    // BeginTimer("Solve_LP");
    bool found_feasible_solution = cplexSolver_.solve();
    // EndTimer("Solve_LP");
    IloNumArray values_optimized(env_, numVariables_);

    if (found_feasible_solution) {
        auto status = cplexSolver_.getStatus();
        cplexSolver_.getValues(varArray_, values_optimized);
        if (GlobalVariablesDAGOpt::debugMode) {
            std::cout << "Values are :" << values_optimized << "\n";
            std::cout << status
                      << " solution found: " << cplexSolver_.getObjValue()
                      << "\n";
        }
        UpdateOptimizedStartTimeVector(values_optimized);
    }
    ClearCplexMemory();
    return optimizedStartTimeVector_;
}

// this function doesn't include artificial variables
void LPOptimizer::AddVariables() {
    int numVariables_ = tasks_info_.N * 2;
    varArray_ = IloNumVarArray(env_, numVariables_, 0, tasks_info_.hyperPeriod,
                               IloNumVar::Float);
}

void LPOptimizer::AddPermutationInequalityConstraints() {
    for (cosnt auto &edge_curr : graph_of_all_ca_chains.edge_vec_ordered_) {
        const SinglePairPermutation &perm_curr = chains_perm_[edge_curr];
        const PermutationInequality ineq = perm_curr.inequality_;
        // ineq.task_prev_id_, ineq.task_next_id_
        model_.add(varArray_[2 * ineq.task_next_id_] + ineq.lower_bound_ <=
                   varArray_[2 * ineq.task_prev_id_ + 1] +
                       GlobalVariablesDAGOpt::kCplexInequalityThreshold);
        model_.add(varArray_[2 * ineq.task_prev_id_ + 1] <=
                   varArray_[2 * ineq.task_next_id_] + ineq.upper_bound_ <=);
    }
}

// void LPOptimizer::AddSensorFusionConstraints() {
//     IloNumVar max_sensor_fusion_interval = IloNumVar(
//         env_, 0, IloInfinity, IloNumVar::Float, "MaxSensorFusionInterval");
//     for (auto itr = dag_tasks_.mapPrev.begin(); itr !=
//     dag_tasks_.mapPrev.end();
//          itr++) {
//         if (itr->second.size() < 2) {
//             continue;
//         }
//         std::unordered_map<JobCEC, std::vector<JobCEC>> sensor_map =
//             GetSensorMapFromSingleJob(tasks_info_, itr->first, itr->second,
//                                       initialStartTimeVector_);

//         for (auto pair : sensor_map) {
//             auto &precede_jobs = pair.second;
//             if (precede_jobs.size() < 2) {
//                 continue;
//             }
//             for (uint i = 0; i < precede_jobs.size(); i++) {
//                 for (uint j = i + 1; j < precede_jobs.size(); j++) {
//                     model_.add(max_sensor_fusion_interval >=
//                                (GetFinishTimeExpression(precede_jobs[i]) -
//                                 GetFinishTimeExpression(precede_jobs[j])));
//                     model_.add(max_sensor_fusion_interval >=
//                                (GetFinishTimeExpression(precede_jobs[j]) -
//                                 GetFinishTimeExpression(precede_jobs[i])));
//                 }
//             }
//             auto succeed_job = pair.first;
//             for (auto precede_job : precede_jobs) {
//                 model_.add(GetFinishTimeExpression(precede_job) <=
//                            GetStartTimeExpression(succeed_job));
//                 precede_job.jobId++;
//                 model_.add(
//                     GetFinishTimeExpression(precede_job) >=
//                     GetStartTimeExpression(succeed_job) +
//                         GlobalVariablesDAGOpt::kCplexInequalityThreshold);
//             }
//         }
//     }
//     model_.add(max_sensor_fusion_interval <=
//                GlobalVariablesDAGOpt::sensorFusionTolerance);
// }

void LPOptimizer::AddObjectiveFunctions() {
    IloExpr rtda_expression(env_);
    std::stringstream var_name;
    int chain_count = 0;
    LLint hyper_period = tasks_info_.hyperPeriod;
    const TaskSet &tasks = tasks_info_.tasks;

    for (auto chain : dag_tasks_.chains_) {
        var_name << "Chain_" << chain_count << "_RT";
        auto theta_rt = IloNumVar(env_, 0, IloInfinity, IloNumVar::Float,
                                  var_name.str().c_str());
        var_name.str("");
        var_name << "Chain_" << chain_count << "_DA";
        auto theta_da = IloNumVar(env_, 0, IloInfinity, IloNumVar::Float,
                                  var_name.str().c_str());
        var_name.str("");
        auto react_chain_map = GetFirstReactMap(
            dag_tasks_, tasks_info_, chains_perm_, chain, variable_od_);

        LLint total_start_jobs = hyper_period / tasks[chain[0]].period + 1;
        for (LLint start_instance_index = 0;
             start_instance_index <= total_start_jobs; start_instance_index++) {
            JobCEC start_job = {chain[0], (start_instance_index)};
            auto &react_chain = react_chain_map[start_job];
            JobCEC first_react_job = react_chain.back();
            model_.add(theta_rt >= (GetFinishTimeExpression(first_react_job) -
                                    GetStartTimeExpression(start_job)));

            JobCEC last_start_job = {chain[0], (start_instance_index - 1)};
            if (start_instance_index > 0 &&
                react_chain_map[last_start_job].back() != first_react_job &&
                first_react_job.jobId > 0) {
                JobCEC last_react_job = first_react_job;
                last_react_job.jobId--;
                model_.add(theta_da >=
                           (GetFinishTimeExpression(last_react_job) -
                            GetStartTimeExpression(last_start_job)));
            }
        }
        // Normal obj to optmize RTDA: obj = max_RTs + max_DAs
        rtda_expression += theta_rt;
        rtda_expression += theta_da;
        chain_count++;
    }
    model_.add(IloMinimize(env_, rtda_expression));
    rtda_expression.end();
}

IloExpr LPOptimizer::GetStartTimeExpression(JobCEC &jobCEC) {
    IloExpr exp(env_);
    if (jobCEC.taskId < 0 || jobCEC.taskId >= tasks_info_.N) {
        CoutError("GetStartTime receives invalid jobCEC!");
    }
    int jobNumInHyperPeriod =
        tasks_info_.hyperPeriod / tasks_info_.tasks[jobCEC.taskId].period;
    exp += varArray_[GetJobUniqueId(jobCEC, tasks_info_)];
    exp += jobCEC.jobId / jobNumInHyperPeriod * tasks_info_.hyperPeriod;
    return exp;
}

IloExpr LPOptimizer::GetFinishTimeExpression(JobCEC &jobCEC) {
    return GetStartTimeExpression(jobCEC) +
           GetExecutionTime(jobCEC, tasks_info_);
}

void LPOptimizer::UpdateOptimizedStartTimeVector(
    IloNumArray &values_optimized) {
    for (int i = 0; i < numVariables_; i++) {
        optimizedStartTimeVector_(i) = values_optimized[i];
        // Round the optimized start time to interger when locate in 1e-4 range.
        // This can avoid float number precision error in further feasibility
        // check.
        if (abs(round(optimizedStartTimeVector_(i)) -
                optimizedStartTimeVector_(i)) < 1e-4) {
            optimizedStartTimeVector_(i) = round(optimizedStartTimeVector_(i));
        }
    }
}

}  // namespace DAG_SPACE