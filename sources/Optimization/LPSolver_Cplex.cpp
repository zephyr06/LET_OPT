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

std::pair<VariableOD, int> LPOptimizer::Optimize(
    const ChainsPermutation &chains_perm) {
  auto res = OptimizeWithoutClear(chains_perm);
  ClearCplexMemory();
  return res;
}

std::pair<VariableOD, int> LPOptimizer::OptimizeWithoutClear(
    const ChainsPermutation &chains_perm) {
  BeginTimer("Build_LP_Model");
  AddVariables();  // must be called first
  AddSchedulabilityConstraints();
  AddPermutationInequalityConstraints(chains_perm);
  AddObjectiveFunctions(chains_perm);
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
      std::cout << status << " solution found: " << cplexSolver_.getObjValue()
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
void LPOptimizer::AddVariablesOD(int number_of_tasks_to_opt) {
  numVariables_ = number_of_tasks_to_opt * 2;
  varArray_ = IloNumVarArray(env_, numVariables_, 0, tasks_info_.hyper_period,
                             IloNumVar::Float);
}

void LPOptimizer::AddArtificialVariables() {
  if (obj_trait_ == "ReactionTime" || obj_trait_ == "ReactionTimeApprox" ||
      obj_trait_ == "DataAge" || obj_trait_ == "DataAgeApprox")
    varArray_art_ =
        IloNumVarArray(env_, static_cast<int>(dag_tasks_.chains_.size()), 0,
                       IloInfinity, IloNumVar::Float);
  else
    CoutError("Unrecognized obj_trait in LPSolver!");
}

void LPOptimizer::AddPermutationInequalityConstraints(
    const ChainsPermutation &chains_perm, bool allow_partial_edges) {
  BeginTimer("AddPermutationInequalityConstraints");

  for (uint i = 0; i < graph_of_all_ca_chains_.edge_vec_ordered_.size(); i++) {
    const Edge &edge_curr = graph_of_all_ca_chains_.edge_vec_ordered_[i];
    if (allow_partial_edges && !chains_perm.exist(edge_curr)) continue;
    const PermutationInequality &ineq = chains_perm[edge_curr]->inequality_;
    std::string const_name1 = GetPermuIneqConstraintNamePrev(i);
    std::string const_name2 = GetPermuIneqConstraintNameNext(i);
    if (ineq.type_trait_ == "ReactionTimeApprox" ||
        ineq.type_trait_ == "ReactionTime") {
      // model_.add(
      //     varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)] +
      //         ineq.lower_bound_ <=
      //     varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)] -
      //         GlobalVariablesDAGOpt::kCplexInequalityThreshold);
      IloRange myConstraint1(
          env_, -IloInfinity,
          varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)] -
              varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)],
          -1 * GlobalVariablesDAGOpt::kCplexInequalityThreshold -
              ineq.lower_bound_,
          const_name1.c_str());
      model_.add(myConstraint1);
      name2ilo_const_[const_name1] = myConstraint1;

      // model_.add(
      //     varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)] <=
      //     varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)] +
      //         ineq.upper_bound_);
      IloRange myConstraint2(
          env_, -IloInfinity,
          varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)] -
              varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)],
          ineq.upper_bound_, const_name2.c_str());
      model_.add(myConstraint2);
      name2ilo_const_[const_name2] = myConstraint2;
    } else if (ineq.type_trait_ == "DataAgeApprox" ||
               ineq.type_trait_ == "DataAge") {
      IloRange myConstraint1(
          env_, ineq.lower_bound_,
          varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)] -
              varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)],
          IloInfinity, const_name1.c_str());
      model_.add(myConstraint1);
      name2ilo_const_[const_name1] = myConstraint1;

      IloRange myConstraint2(
          env_, -IloInfinity,
          varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)] -
              varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)],
          ineq.upper_bound_ - GlobalVariablesDAGOpt::kCplexInequalityThreshold,
          const_name2.c_str());
      model_.add(myConstraint2);
      name2ilo_const_[const_name2] = myConstraint2;
    }

    if (GlobalVariablesDAGOpt::debugMode) {
      ineq.print();
    }
  }
  EndTimer("AddPermutationInequalityConstraints");
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

void LPOptimizer::AddTwoJobLengthConstraint(const JobCEC &start_job,
                                            const JobCEC &finish_job,
                                            int chain_count,
                                            int job_pair_index) {
  std::string const_name = GetReactConstraintName(chain_count, job_pair_index);
  IloExpr finish_expr = GetFinishTimeExpression(finish_job);
  IloExpr start_expr = GetStartTimeExpression(start_job);
  IloRange myConstraint1(env_, 0,
                         varArray_art_[chain_count] - finish_expr + start_expr,
                         IloInfinity, const_name.c_str());
  model_.add(myConstraint1);
  name2ilo_const_[const_name] = myConstraint1;
  finish_expr.end();
  start_expr.end();
}
void LPOptimizer::AddTwoJobApproxLengthConstraint(const JobCEC &start_job,
                                                  const JobCEC &finish_job,
                                                  int chain_count,
                                                  int job_pair_index) {
  std::string const_name = GetReactConstraintName(chain_count, job_pair_index);
  IloExpr finish_expr = GetFinishTimeExpressionApprox(finish_job);
  IloExpr start_expr = GetStartTimeExpressionApprox(start_job);
  IloRange myConstraint1(env_, 0,
                         varArray_art_[chain_count] - finish_expr + start_expr,
                         IloInfinity, const_name.c_str());
  model_.add(myConstraint1);
  name2ilo_const_[const_name] = myConstraint1;
  finish_expr.end();
  start_expr.end();
}

void LPOptimizer::AddObjectiveFunctions(const ChainsPermutation &chains_perm) {
  BeginTimer("AddObjective");
  IloExpr rtda_expression(env_);
  int chain_count = 0;
  for (auto chain : dag_tasks_.chains_) {
    int hyper_period = GetHyperPeriod(tasks_info_, chain);
    if (obj_trait_ == "ReactionTime" || obj_trait_ == "ReactionTimeApprox") {
      LLint total_start_jobs =
          hyper_period / dag_tasks_.GetTask(chain[0]).period + 1;
      auto react_chain_map =
          GetFirstReactMap(dag_tasks_, tasks_info_, chains_perm, chain);
      for (LLint start_instance_index = 0;
           start_instance_index <= total_start_jobs; start_instance_index++) {
        JobCEC start_job = {chain[0], (start_instance_index)};
        JobCEC first_react_job = react_chain_map[start_job];
        std::string const_name =
            GetReactConstraintName(chain_count, start_instance_index);
        if (obj_trait_ == "ReactionTime") {
          AddTwoJobLengthConstraint(start_job, first_react_job, chain_count,
                                    start_instance_index);
        } else {  // (obj_trait_ == "ReactionTimeApprox")
          AddTwoJobApproxLengthConstraint(start_job, first_react_job,
                                          chain_count, start_instance_index);
        }
      }
    } else if (obj_trait_ == "DataAge" || obj_trait_ == "DataAgeApprox") {
      LLint total_start_jobs =
          hyper_period / dag_tasks_.GetTask(chain.back()).period + 1;
      for (LLint start_instance_index = 0;
           start_instance_index < total_start_jobs; start_instance_index++) {
        JobCEC start_job = {chain.back(), (start_instance_index)};
        JobCEC last_read_job =
            GetLastReadJob(start_job, chains_perm, chain, tasks_info_);
        std::string const_name =
            GetReactConstraintName(chain_count, start_instance_index);
        if (obj_trait_ == "DataAge") {
          AddTwoJobLengthConstraint(last_read_job, start_job, chain_count,
                                    start_instance_index);
        } else {  // obj_trait_ == "DataAgeApprox"
          AddTwoJobApproxLengthConstraint(last_read_job, start_job, chain_count,
                                          start_instance_index);
        }
      }
    }

    // Normal obj to optmize RTDA: obj = max_RTs + max_DAs
    rtda_expression += varArray_art_[chain_count];
    // rtda_expression += theta_da;
    chain_count++;
  }
  model_.add(IloMinimize(env_, rtda_expression));
  rtda_expression.end();
  EndTimer("AddObjective");
}

VariableOD LPOptimizer::ExtratOptSolution(IloNumArray &values_optimized) {
  VariableOD variable_od_opt(dag_tasks_.GetTaskSet());
  for (int task_id = 0; task_id < tasks_info_.N; task_id++) {
    variable_od_opt.SetOffset(
        task_id, values_optimized[GetVariableIndexVirtualOffset(task_id)]);
    variable_od_opt.SetDeadline(
        task_id, values_optimized[GetVariableIndexVirtualDeadline(task_id)]);
  }
  return variable_od_opt;
}

// ****************following functions are commented**************

// TODO: consider whether it's necessary to improve efficiency there by
// reducing problem size;
// int LPOptimizer::FindMinOffset(int task_id,
//                                const ChainsPermutation &chains_perm) {
//   // Interval interval_res(1e8, -1e8);
//   AddVariablesOD(tasks_info_.N);
//   AddSchedulabilityConstraints();
//   AddPermutationInequalityConstraints(chains_perm, true);

//   IloObjective obj_ilo =
//       IloMinimize(env_, varArray_[GetVariableIndexVirtualOffset(task_id)]);
//   model_.add(obj_ilo);
//   cplexSolver_.extract(model_);
//   // WriteModelToFile("test_lp1.lp");
//   bool found_feasible_solution = cplexSolver_.solve();
//   if (found_feasible_solution) {
//     IloNumArray values_optimized(env_, numVariables_);
//     cplexSolver_.getValues(varArray_, values_optimized);
//     // interval_res.start =
//     return values_optimized[GetVariableIndexVirtualOffset(task_id)];
//   } else
//     return INFEASIBLE_OBJ;

//   // obj_ilo.setLinearCoef(varArray_[GetVariableIndexVirtualOffset(task_id)],
//   //                       -1);
//   // cplexSolver_.extract(model_);
//   // found_feasible_solution = cplexSolver_.solve();
//   // if (found_feasible_solution) {
//   //     IloNumArray values_optimized(env_, numVariables_);
//   //     cplexSolver_.getValues(varArray_, values_optimized);
//   //     interval_res.length =
//   //         values_optimized[GetVariableIndexVirtualOffset(task_id)] -
//   //         interval_res.start;
//   // } else
//   //     return interval_res;
//   // // WriteModelToFile("test_lp2.lp");
//   // // ClearCplexMemory();
//   // return interval_res;
// }

// void LPOptimizer::UpdatePermutationInequalityConstraints(
//     const ChainsPermutation &chains_perm) {
//   BeginTimer("UpdatePermutationInequalityConstraints");
//   for (uint i = 0; i < graph_of_all_ca_chains_.edge_vec_ordered_.size(); i++)
//   {
//     const Edge &edge_curr = graph_of_all_ca_chains_.edge_vec_ordered_[i];
//     const PermutationInequality &ineq = chains_perm[edge_curr]->inequality_;
//     // model_.add(
//     //     varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)] +
//     //         ineq.lower_bound_ <=
//     //     varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)] +
//     //         GlobalVariablesDAGOpt::kCplexInequalityThreshold);
//     std::string const_name1 = GetPermuIneqConstraintNamePrev(i);
//     double ub_curr =
//         GlobalVariablesDAGOpt::kCplexInequalityThreshold - ineq.lower_bound_;

//     if (name2ilo_const_[const_name1].getUB() != ub_curr)
//       name2ilo_const_[const_name1].setUB(ub_curr);

//     // model_.add(
//     //     varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)] <=
//     //     varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)] +
//     //         ineq.upper_bound_);
//     std::string const_name2 = GetPermuIneqConstraintNameNext(i);
//     double ub_curr2 = ineq.upper_bound_;
//     if (name2ilo_const_[const_name2].getUB() != ub_curr2)
//       name2ilo_const_[const_name2].setUB(ub_curr2);
//   }
//   EndTimer("UpdatePermutationInequalityConstraints");
// }

// std::pair<VariableOD, int> LPOptimizer::OptimizeAfterUpdate(
//     const ChainsPermutation &chains_perm) {
//   BeginTimer("extract_model_AfterUpdate");
//   cplexSolver_.extract(model_);
//   EndTimer("extract_model_AfterUpdate");

//   BeginTimer("Solve_LP_AfterUpdate");
//   bool found_feasible_solution = cplexSolver_.solve();
//   EndTimer("Solve_LP_AfterUpdate");

//   BeginTimer("AfterSolve_LP_AfterUpdate");
//   IloNumArray values_optimized(env_, numVariables_);
//   if (found_feasible_solution) {
//     auto status = cplexSolver_.getStatus();
//     cplexSolver_.getValues(varArray_, values_optimized);
//     if (GlobalVariablesDAGOpt::debugMode) {
//       std::cout << "Values are :" << values_optimized << "\n";
//       std::cout << status << " solution found: " <<
//       cplexSolver_.getObjValue()
//                 << "\n";
//     }
//     variable_od_opt_ = ExtratOptSolution(values_optimized);
//     optimal_obj_ = cplexSolver_.getObjValue();
//   } else if (GlobalVariablesDAGOpt::debugMode)
//     std::cout << "No feasible solution found!\n";
//   EndTimer("AfterSolve_LP_AfterUpdate");
//   return std::make_pair(variable_od_opt_, optimal_obj_);
// }

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
}  // namespace DAG_SPACE