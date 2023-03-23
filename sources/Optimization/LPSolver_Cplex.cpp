#include "sources/Optimization/LPSolver_Cplex.h"

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "sources/Utils/Parameters.h"

namespace DAG_SPACE {
void LPOptimizer::Init() {
    BeginTimer("Init");
    env_ = IloEnv();
    model_ = IloModel(env_);
    cplexSolver_ = IloCplex(env_);
    cplexSolver_.setOut(env_.getNullStream());
    // constraint_array_(env_);
    EndTimer("Init");
}

void LPOptimizer::ClearCplexMemory() {
#ifdef PROFILE_CODE
    BeginTimer(__FUNCTION__);
#endif
    // release memory
    cplexSolver_.end();
    model_.end();
    env_.end();
#ifdef PROFILE_CODE
    EndTimer(__FUNCTION__);
#endif
}

std::pair<VariableOD, int> LPOptimizer::Optimize() {
    auto res = OptimizeWithoutClear();
    ClearCplexMemory();
    return res;
}

std::pair<VariableOD, int> LPOptimizer::OptimizeAfterUpdate() {
    BeginTimer("extract_model_AfterUpdate");
    cplexSolver_.extract(model_);
    EndTimer("extract_model_AfterUpdate");

    BeginTimer("Solve_LP_AfterUpdate");
    bool found_feasible_solution = cplexSolver_.solve();
    EndTimer("Solve_LP_AfterUpdate");

    BeginTimer("AfterSolve_LP_AfterUpdate");
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
        variable_od_opt_ = ExtratOptSolution(values_optimized);
        optimal_obj_ = cplexSolver_.getObjValue();
    } else if (GlobalVariablesDAGOpt::debugMode)
        std::cout << "No feasible solution found!\n";
    EndTimer("AfterSolve_LP_AfterUpdate");
    return std::make_pair(variable_od_opt_, optimal_obj_);
}

std::pair<VariableOD, int> LPOptimizer::OptimizeWithoutClear() {
    BeginTimer("Build_LP_Model");
    AddVariables();  // must be called first
    AddPermutationInequalityConstraints();
    AddSchedulabilityConstraints();
    AddObjectiveFunctions();
    cplexSolver_.extract(model_);
    EndTimer("Build_LP_Model");

    BeginTimer("Solve_LP");
    bool found_feasible_solution = cplexSolver_.solve();
    EndTimer("Solve_LP");

    BeginTimer("AfterSolve_LP");
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
        variable_od_opt_ = ExtratOptSolution(values_optimized);
        optimal_obj_ = cplexSolver_.getObjValue();
    } else if (GlobalVariablesDAGOpt::debugMode)
        std::cout << "No feasible solution found!\n";
    EndTimer("AfterSolve_LP");
    return std::make_pair(variable_od_opt_, optimal_obj_);
}

// this function doesn't include artificial variables
void LPOptimizer::AddVariables() {
    BeginTimer("AddVariables");
    numVariables_ = tasks_info_.N * 2;
    varArray_ = IloNumVarArray(env_, numVariables_, 0, tasks_info_.hyper_period,
                               IloNumVar::Float);
    EndTimer("AddVariables");
}

void LPOptimizer::AddPermutationInequalityConstraints() {
    BeginTimer("AddPermutationInequalityConstraints");

    for (uint i = 0; i < graph_of_all_ca_chains_.edge_vec_ordered_.size();
         i++) {
        const Edge &edge_curr = graph_of_all_ca_chains_.edge_vec_ordered_[i];
        const PermutationInequality &ineq = chains_perm_[edge_curr].inequality_;
        // model_.add(
        //     varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)] +
        //         ineq.lower_bound_ <=
        //     varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)] +
        //         GlobalVariablesDAGOpt::kCplexInequalityThreshold);
        std::string const_name1 = "perm_constraint_" + std::to_string(i * 2);
        IloRange myConstraint1(
            env_, -IloInfinity,
            varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)] -
                varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)],
            GlobalVariablesDAGOpt::kCplexInequalityThreshold -
                ineq.lower_bound_,
            const_name1.c_str());
        model_.add(myConstraint1);
        name2ilo_const_[const_name1] = myConstraint1;

        // model_.add(
        //     varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)] <=
        //     varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)] +
        //         ineq.upper_bound_);
        std::string const_name2 =
            "perm_constraint_" + std::to_string(i * 2 + 1);
        IloRange myConstraint2(
            env_, -IloInfinity,
            varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)] -
                varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)],
            ineq.upper_bound_, const_name2.c_str());
        model_.add(myConstraint2);
        name2ilo_const_[const_name2] = myConstraint2;

        if (GlobalVariablesDAGOpt::debugMode) {
            ineq.print();
        }
    }
    EndTimer("AddPermutationInequalityConstraints");
}

void LPOptimizer::UpdatePermutationInequalityConstraints(
    const ChainsPermutation &chains_perm) {
    BeginTimer("UpdatePermutationInequalityConstraints");
    for (uint i = 0; i < graph_of_all_ca_chains_.edge_vec_ordered_.size();
         i++) {
        const Edge &edge_curr = graph_of_all_ca_chains_.edge_vec_ordered_[i];
        const PermutationInequality &ineq = chains_perm[edge_curr].inequality_;
        // model_.add(
        //     varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)] +
        //         ineq.lower_bound_ <=
        //     varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)] +
        //         GlobalVariablesDAGOpt::kCplexInequalityThreshold);
        std::string const_name1 = "perm_constraint_" + std::to_string(i * 2);
        double ub_curr = GlobalVariablesDAGOpt::kCplexInequalityThreshold -
                         ineq.lower_bound_;
        if (name2ilo_const_[const_name1].getUB() != ub_curr)
            name2ilo_const_[const_name1].setUB(ub_curr);

        // model_.add(
        //     varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)] <=
        //     varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)] +
        //         ineq.upper_bound_);
        std::string const_name2 =
            "perm_constraint_" + std::to_string(i * 2 + 1);
        double ub_curr2 = ineq.upper_bound_;
        if (name2ilo_const_[const_name2].getUB() != ub_curr2)
            name2ilo_const_[const_name2].setUB(ub_curr2);
    }
    EndTimer("UpdatePermutationInequalityConstraints");
}

void LPOptimizer::AddSchedulabilityConstraints() {
    BeginTimer("AddSchedulabilityConstraints");
    for (int task_id = 0; task_id < tasks_info_.N; task_id++) {
        model_.add(varArray_[GetVariableIndexVirtualOffset(task_id)] +
                       rta_[task_id] <=
                   varArray_[GetVariableIndexVirtualDeadline(task_id)]);

        model_.add(varArray_[GetVariableIndexVirtualDeadline(task_id)] <=
                   dag_tasks_.GetTask(task_id).deadline);
        if (GlobalVariablesDAGOpt::debugMode) {
            std::cout << "o_" << task_id << " + " << rta_[task_id] << " <= d_"
                      << task_id << "\n";

            std::cout << "d_" << task_id
                      << " <= " << dag_tasks_.GetTask(task_id).deadline << "\n";
        }
    }
    EndTimer("AddSchedulabilityConstraints");
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

// TODO: replace this function with other functions
void LPOptimizer::AddObjectiveFunctions() {
    BeginTimer("AddObjective");
    IloExpr rtda_expression(env_);
    int chain_count = 0;
    for (auto chain : dag_tasks_.chains_) {
        std::string var_name = "Chain_" + std::to_string(chain_count) + "_RT";
        auto theta_rt =
            IloNumVar(env_, 0, IloInfinity, IloNumVar::Float, var_name.c_str());
        name2ilo_var_[var_name] = theta_rt;
        var_name = "Chain_" + std::to_string(chain_count) + "_DA";
        auto theta_da =
            IloNumVar(env_, 0, IloInfinity, IloNumVar::Float, var_name.c_str());
        name2ilo_var_[var_name] = theta_da;

        auto react_chain_map =
            GetFirstReactMap(dag_tasks_, tasks_info_, chains_perm_, chain);
        // react_chain_map_prevs_.push_back(react_chain_map);

        int hyper_period = GetHyperPeriod(tasks_info_, chain);
        LLint total_start_jobs =
            hyper_period / dag_tasks_.GetTask(chain[0]).period + 1;
        for (LLint start_instance_index = 0;
             start_instance_index <= total_start_jobs; start_instance_index++) {
            JobCEC start_job = {chain[0], (start_instance_index)};
            JobCEC first_react_job = react_chain_map[start_job];
            if (obj_trait_ == "ReactionTime") {
                std::string const_name =
                    "rt_constraint_" + std::to_string(start_instance_index);
                IloExpr finish_expr = GetFinishTimeExpression(first_react_job);
                IloExpr start_expr = GetStartTimeExpression(start_job);
                IloRange myConstraint1(env_, 0,
                                       theta_rt - finish_expr + start_expr,
                                       IloInfinity, const_name.c_str());
                model_.add(myConstraint1);
                name2ilo_const_[const_name] = myConstraint1;
                finish_expr.end();
                start_expr.end();
            } else if (obj_trait_ == "DataAge") {
                JobCEC last_start_job = {chain[0], (start_instance_index - 1)};
                if (start_instance_index > 0 &&
                    react_chain_map[last_start_job] != first_react_job &&
                    first_react_job.jobId > 0) {
                    JobCEC last_react_job(first_react_job.taskId,
                                          first_react_job.jobId - 1);
                    //   TODO: make the constraint name trick work!
                    model_.add(theta_da >=
                               (GetFinishTimeExpression(last_react_job) -
                                GetStartTimeExpression(last_start_job)));
                }
            }
        }
        // Normal obj to optmize RTDA: obj = max_RTs + max_DAs
        rtda_expression += theta_rt;
        rtda_expression += theta_da;
        chain_count++;
    }
    model_.add(IloMinimize(env_, rtda_expression));
    rtda_expression.end();
    EndTimer("AddObjective");
}

void LPOptimizer::UpdateObjectiveFunctions(
    const ChainsPermutation &chains_perm) {
    BeginTimer("UpdateObjectiveFunctions");
    // IloExpr rtda_expression(env_);
    int chain_count = 0;
    for (auto chain : dag_tasks_.chains_) {
        std::string var_name = "Chain_" + std::to_string(chain_count) + "_RT";
        auto theta_rt = name2ilo_var_[var_name];
        var_name = "Chain_" + std::to_string(chain_count) + "_DA";
        auto theta_da = name2ilo_var_[var_name];

        std::unordered_map<JobCEC, JobCEC> react_chain_map =
            GetFirstReactMap(dag_tasks_, tasks_info_, chains_perm, chain);
        // std::unordered_map<JobCEC, JobCEC> react_chain_map_prev =
        //     react_chain_map_prevs_[chain_count];

        int hyper_period = GetHyperPeriod(tasks_info_, chain);
        LLint total_start_jobs =
            hyper_period / dag_tasks_.GetTask(chain[0]).period + 1;
        for (LLint start_instance_index = 0;
             start_instance_index <= total_start_jobs; start_instance_index++) {
            JobCEC start_job = {chain[0], (start_instance_index)};
            JobCEC first_react_job = react_chain_map[start_job];
            // if (react_chain_map_prev[start_job] == first_react_job) continue;
            if (obj_trait_ == "ReactionTime") {
                // model_.add(theta_rt >=
                //            (GetFinishTimeExpression(first_react_job) -
                //             GetStartTimeExpression(start_job)));
                std::string const_name =
                    "rt_constraint_" + std::to_string(start_instance_index);
                model_.remove(name2ilo_const_[const_name]);
                IloExpr finish_expr = GetFinishTimeExpression(first_react_job);
                IloExpr start_expr = GetStartTimeExpression(start_job);
                IloExpr full_expr = theta_rt - finish_expr + start_expr;
                IloRange myConstraint1(env_, 0, full_expr, IloInfinity,
                                       const_name.c_str());
                // IloExpr full_exp = theta_rt -
                //                    GetFinishTimeExpression(first_react_job) +
                //                    GetStartTimeExpression(start_job);
                // myConstraint1.setExpr(full_exp >= 0);
                // name2ilo_const_[const_name].setExpr(
                //     theta_rt - GetFinishTimeExpression(first_react_job) +
                //         GetStartTimeExpression(start_job) >=
                //     0);
                // full_exp.end();
                // model_.add(theta_rt -
                // GetFinishTimeExpression(first_react_job) +
                //                GetStartTimeExpression(start_job) >=
                //            0);
                name2ilo_const_[const_name] = myConstraint1;
                model_.add(myConstraint1);
                finish_expr.end();
                start_expr.end();
                full_expr.end();
                if (GlobalVariablesDAGOpt::debugMode) {
                    std::cout << "Chain_" << chain_count << "_RT"
                              << " >= "
                              << "Deadline_" << first_react_job.ToString()
                              << " - "
                              << "Offset_" << start_job.ToString() << "\n";
                }
            } else if (obj_trait_ == "DataAge") {
                JobCEC last_start_job = {chain[0], (start_instance_index - 1)};
                if (start_instance_index > 0 &&
                    react_chain_map[last_start_job] != first_react_job &&
                    first_react_job.jobId > 0) {
                    JobCEC last_react_job(first_react_job.taskId,
                                          first_react_job.jobId - 1);
                    //   TODO: make the constraint name trick work!
                    model_.add(theta_da >=
                               (GetFinishTimeExpression(last_react_job) -
                                GetStartTimeExpression(last_start_job)));
                }
            }
        }
        // Normal obj to optmize RTDA: obj = max_RTs + max_DAs
        // rtda_expression += theta_rt;
        // rtda_expression += theta_da;
        chain_count++;
    }
    // model_.add(IloMinimize(env_, rtda_expression));
    // rtda_expression.end();
    EndTimer("UpdateObjectiveFunctions");
}
void LPOptimizer::AddConstantObjectiveFunctions() {
    IloExpr rtda_expression(env_);
    std::stringstream var_name;
    auto theta_rt = IloNumVar(env_, 3, IloInfinity, IloNumVar::Float,
                              var_name.str().c_str());
    model_.add(IloMinimize(env_, rtda_expression));
    rtda_expression.end();
}

// void LPOptimizer::AddObjectiveFunctionReactionTime() {
//     IloExpr rtda_expression(env_);
//     std::stringstream var_name;
//     int chain_count = 0;
//     const TaskSet &tasks = dag_tasks_.GetTaskSet();

//     for (auto chain : dag_tasks_.chains_) {
//         var_name.str("");
//         var_name << "Chain_" << chain_count << "_RT";
//         auto theta_rt = IloNumVar(env_, 0, IloInfinity, IloNumVar::Float,
//                                   var_name.str().c_str());
//         auto react_chain_map =
//             GetFirstReactMap(dag_tasks_, tasks_info_, chains_perm_,
//             chain);

//         int hyper_period = GetHyperPeriod(tasks_info_, chain);
//         LLint total_start_jobs = hyper_period / tasks[chain[0]].period + 1;
//         for (LLint start_instance_index = 0;
//              start_instance_index <= total_start_jobs;
//              start_instance_index++) {
//             JobCEC start_job = {chain[0], (start_instance_index)};
//             JobCEC first_react_job = react_chain_map[start_job];
//             model_.add(theta_rt >= (GetFinishTimeExpression(first_react_job)
//             -
//                                     GetStartTimeExpression(start_job)));
//         }
//         rtda_expression += theta_rt;
//         chain_count++;
//     }
//     model_.add(IloMinimize(env_, rtda_expression));
//     rtda_expression.end();
// }
// void LPOptimizer::AddObjectiveFunctionDataAge() {
//     IloExpr rtda_expression(env_);
//     std::stringstream var_name;
//     int chain_count = 0;
//     const TaskSet &tasks = dag_tasks_.GetTaskSet();

//     for (auto chain : dag_tasks_.chains_) {
//         var_name.str("");
//         var_name << "Chain_" << chain_count << "_DA";
//         auto theta_da = IloNumVar(env_, 0, IloInfinity, IloNumVar::Float,
//                                   var_name.str().c_str());
//         auto react_chain_map =
//             GetFirstReactMap(dag_tasks_, tasks_info_, chains_perm_,
//             chain);

//         int hyper_period = GetHyperPeriod(tasks_info_, chain);
//         LLint total_start_jobs = hyper_period / tasks[chain[0]].period + 1;
//         for (LLint start_instance_index = 1;
//              start_instance_index <= total_start_jobs;
//              start_instance_index++) {
//             JobCEC start_job = {chain[0], (start_instance_index)};
//             JobCEC first_react_job = react_chain_map[start_job];
//             JobCEC last_start_job = {chain[0], (start_instance_index - 1)};
//             if (start_instance_index > 0 &&
//                 react_chain_map[last_start_job] != first_react_job &&
//                 first_react_job.jobId > 0) {
//                 JobCEC last_react_job(first_react_job.taskId,
//                                       first_react_job.jobId - 1);
//                 model_.add(theta_da >=
//                            (GetFinishTimeExpression(last_react_job) -
//                             GetStartTimeExpression(last_start_job)));
//             }
//         }
//         rtda_expression += theta_da;
//         chain_count++;
//     }
//     model_.add(IloMinimize(env_, rtda_expression));
//     rtda_expression.end();
// }

IloExpr LPOptimizer::GetStartTimeExpression(JobCEC &job) {
    IloExpr exp(env_);
    exp += varArray_[GetVariableIndexVirtualOffset(job)];
    exp += job.jobId * tasks_info_.GetTask(job.taskId).period;
    return exp;
}

IloExpr LPOptimizer::GetFinishTimeExpression(JobCEC &job) {
    IloExpr exp(env_);
    exp += varArray_[GetVariableIndexVirtualDeadline(job)];
    exp += job.jobId * tasks_info_.GetTask(job.taskId).period;
    return exp;
}

VariableOD LPOptimizer::ExtratOptSolution(IloNumArray &values_optimized) {
    VariableOD variable_od_opt(dag_tasks_.GetTaskSet());
    for (int task_id = 0; task_id < tasks_info_.N; task_id++) {
        variable_od_opt.SetOffset(
            task_id, values_optimized[GetVariableIndexVirtualOffset(task_id)]);
        variable_od_opt.SetDeadline(
            task_id,
            values_optimized[GetVariableIndexVirtualDeadline(task_id)]);
    }
    return variable_od_opt;
}

}  // namespace DAG_SPACE