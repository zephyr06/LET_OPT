#include "sources/Optimization/LPSolver_Cplex.h"

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "sources/Utils/Parameters.h"

namespace DAG_SPACE {
void LPOptimizer::Init() {
#ifdef PROFILE_CODE
  BeginTimer("Init");
#endif
  // env_ = IloEnv();
  // model_ = IloModel(env_);
  // cplexSolver_ = IloCplex(env_);
  cplexSolver_.setOut(env_.getNullStream());
  cplexSolver_.setParam(
      IloCplex::Param::Emphasis::MIP,
      5);  // emphasize speed than optimality during optimization
  cplexSolver_.setParam(IloCplex::Param::Threads, 1);
  cplexSolver_.setParam(IloCplex::Param::Parallel, -1);
  // constraint_array_(env_);
#ifdef PROFILE_CODE
  EndTimer("Init");
#endif
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
  Init();
  auto res = OptimizeWithoutClear(chains_perm);
  ClearCplexMemory();
  return res;
}

std::pair<VariableOD, int> LPOptimizer::OptimizeWithoutClear(
    const ChainsPermutation &chains_perm) {
#ifdef PROFILE_CODE
  BeginTimer("Build_LP_Model");
#endif
  AddVariables();  // must be called first
  if (optimize_offset_only_) AddConstantDeadlineConstraint();
  if (optimize_deadline_only_) AddConstantOffsetConstraint();
  if (!optimize_offset_only_) AddSchedulabilityConstraints();
  AddPermutationInequalityConstraints(chains_perm);
  AddObjectiveFunctions(chains_perm);
  cplexSolver_.extract(model_);
  if (GlobalVariablesDAGOpt::debugMode) {
    WriteModelToFile();
  }
#ifdef PROFILE_CODE
  EndTimer("Build_LP_Model");
#endif
#ifdef PROFILE_CODE
  BeginTimer("Solve_LP");
#endif
  bool found_feasible_solution = cplexSolver_.solve();
#ifdef PROFILE_CODE
  EndTimer("Solve_LP");
#endif

#ifdef PROFILE_CODE
  BeginTimer("AfterSolve_LP");
#endif
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
  } else {
    if (GlobalVariablesDAGOpt::debugMode)
      std::cout << "No feasible solution found!\n";
    variable_od_opt_.valid_ = false;
  }
#ifdef PROFILE_CODE
  EndTimer("AfterSolve_LP");
#endif
  return std::make_pair(variable_od_opt_, optimal_obj_);
}

// this function doesn't include artificial variables
void LPOptimizer::AddVariablesOD(int number_of_tasks_to_opt) {
  numVariables_ = number_of_tasks_to_opt * 2;
  varArray_ =
      IloNumVarArray(env_, numVariables_, 0, IloInfinity, IloNumVar::Float);
}

void LPOptimizer::AddConstantDeadlineConstraint() {
  // TODO: is this deadline setting correct?
  for (int task_id = 0; task_id < tasks_info_.N; task_id++) {
    int deadline = tasks_info_.GetTask(task_id).deadline;
    IloRange myConstraint1(
        env_, deadline,
        varArray_[GetVariableIndexVirtualDeadline(task_id)] -
            varArray_[GetVariableIndexVirtualOffset(task_id)],
        deadline);
    model_.add(myConstraint1);
  }
}

void LPOptimizer::AddConstantOffsetConstraint() {
  for (int task_id = 0; task_id < tasks_info_.N; task_id++) {
    int deadline = tasks_info_.GetTask(task_id).deadline;
    IloRange myConstraint1(env_, variable_pre_opt_[task_id].offset,
                           varArray_[GetVariableIndexVirtualOffset(task_id)],
                           variable_pre_opt_[task_id].offset);
    model_.add(myConstraint1);
  }
}

void LPOptimizer::AddArtificialVariables() {
  if (IfRT_Trait(obj_trait_) || IfDA_Trait(obj_trait_)) {
    varArray_art_max_ = IloNumVarArray(
        env_, static_cast<int>(graph_of_all_ca_chains_.chains_.size()), 0,
        IloInfinity, IloNumVar::Float);
    // if (optimize_jitter_weight_ != 0)
    //   varArray_art_min_ =
    //       IloNumVarArray(env_, static_cast<int>(dag_tasks_.chains_.size()),
    //       0,
    //                      IloInfinity, IloNumVar::Float);
  } else if (IfSF_Trait(obj_trait_)) {
    varArray_art_max_ =
        IloNumVarArray(env_, static_cast<int>(dag_tasks_.sf_forks_.size()), 0,
                       IloInfinity, IloNumVar::Float);
    if (optimize_jitter_weight_ != 0) {
      // varArray_art_min_ =
      //     IloNumVarArray(env_, static_cast<int>(dag_tasks_.sf_forks_.size()),
      //     0,
      //                    IloInfinity, IloNumVar::Float);

      // varArray_sf_instances_ =
      //     IloNumVarArray(env_,
      //     dag_tasks_.getSF_Fork_InstanceCount(tasks_info_),
      //                    0, IloInfinity, IloNumVar::Float);
    }

  } else
    CoutError("Unrecognized obj_trait in LPSolver!");
}

void LPOptimizer::AddPermutationInequalityConstraints(
    const ChainsPermutation &chains_perm, bool allow_partial_edges) {
#ifdef PROFILE_CODE
  BeginTimer("AddPermutationInequalityConstraints");
#endif
  for (uint i = 0; i < graph_of_all_ca_chains_.edge_vec_ordered_.size(); i++) {
    const Edge &edge_curr = graph_of_all_ca_chains_.edge_vec_ordered_[i];
    if (allow_partial_edges && !chains_perm.exist(edge_curr)) continue;
    const PermutationInequality &ineq = chains_perm[edge_curr]->inequality_;
    // std::string const_name1 = GetPermuIneqConstraintNamePrev(i);
    // std::string const_name2 = GetPermuIneqConstraintNameNext(i);
    if (IfRT_Trait(ineq.type_trait_)) {
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
              ineq.lower_bound_);  // ,const_name1.c_str()
      model_.add(myConstraint1);
      // name2ilo_const_[const_name1] = myConstraint1;

      // model_.add(
      //     varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)] <=
      //     varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)] +
      //         ineq.upper_bound_);
      IloRange myConstraint2(
          env_, -IloInfinity,
          varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)] -
              varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)],
          ineq.upper_bound_);  // , const_name2.c_str()
      model_.add(myConstraint2);
      // name2ilo_const_[const_name2] = myConstraint2;
    } else if (IfDA_Trait(ineq.type_trait_) || IfSF_Trait(ineq.type_trait_)) {
      IloRange myConstraint1(
          env_, ineq.lower_bound_,
          varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)] -
              varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)],
          IloInfinity);  // , const_name1.c_str()
      model_.add(myConstraint1);
      // name2ilo_const_[const_name1] = myConstraint1;

      IloRange myConstraint2(
          env_, -IloInfinity,
          varArray_[GetVariableIndexVirtualOffset(ineq.task_next_id_)] -
              varArray_[GetVariableIndexVirtualDeadline(ineq.task_prev_id_)],
          ineq.upper_bound_ -
              GlobalVariablesDAGOpt::
                  kCplexInequalityThreshold);  // , const_name2.c_str()
      model_.add(myConstraint2);
      // name2ilo_const_[const_name2] = myConstraint2;
    }

    if (GlobalVariablesDAGOpt::debugMode) {
      ineq.print();
    }
  }
#ifdef PROFILE_CODE
  EndTimer("AddPermutationInequalityConstraints");
#endif
}

void LPOptimizer::AddSchedulabilityConstraints() {
#ifdef PROFILE_CODE
  BeginTimer("AddSchedulabilityConstraints");
#endif
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
#ifdef PROFILE_CODE
  EndTimer("AddSchedulabilityConstraints");
#endif
}

void LPOptimizer::AddTwoJobLengthConstraint(const JobCEC &start_job,
                                            const JobCEC &finish_job,
                                            int chain_count,
                                            int job_pair_index) {
  IloExpr finish_expr = GetFinishTimeExpression(finish_job);
  IloExpr start_expr = GetStartTimeExpression(start_job);
  IloRange myConstraint1(
      env_, 0, varArray_art_max_[chain_count] - finish_expr + start_expr,
      IloInfinity);  // , const_name.c_str()
  model_.add(myConstraint1);
  // if (optimize_jitter_weight_ != 0) {
  //   IloRange myConstraint2(
  //       env_, 0, finish_expr - start_expr - varArray_art_min_[chain_count],
  //       IloInfinity);
  //   model_.add(myConstraint2);
  // }
  finish_expr.end();
  start_expr.end();
}
void LPOptimizer::AddTwoJobApproxLengthConstraint(const JobCEC &start_job,
                                                  const JobCEC &finish_job,
                                                  int chain_count,
                                                  int job_pair_index) {
  IloExpr finish_expr = GetFinishTimeExpressionApprox(finish_job);
  IloExpr start_expr = GetStartTimeExpressionApprox(start_job);
  IloRange myConstraint1(
      env_, 0, varArray_art_max_[chain_count] - finish_expr + start_expr,
      IloInfinity);  // , const_name.c_str()
  model_.add(myConstraint1);

  // if (optimize_jitter_weight_ != 0) {
  //   IloRange myConstraint2(
  //       env_, 0, finish_expr - start_expr - varArray_art_min_[chain_count],
  //       IloInfinity);  // , const_name.c_str()
  //   model_.add(myConstraint2);
  // }
  finish_expr.end();
  start_expr.end();
}

void LPOptimizer::AddObjectiveFunctions(const ChainsPermutation &chains_perm) {
  if (IfRT_Trait(obj_trait_) || IfDA_Trait(obj_trait_)) {
    AddRTDAObjectiveFunctions(chains_perm);
  } else if (IfSF_Trait(obj_trait_)) {
    // if (optimize_jitter_weight_)
    //   AddSFObjectiveFunctions_WithJitter(chains_perm);
    // else
    AddSFObjectiveFunctions(chains_perm);
  } else
    CoutError("Unrecognized type trait in AddObjectiveFunctions!");
}

void LPOptimizer::AddRTDAObjectiveFunctions(
    const ChainsPermutation &chains_perm) {
#ifdef PROFILE_CODE
  BeginTimer("AddObjective");
#endif
  IloExpr rtda_expression(env_);
  int chain_count = 0;
  for (auto chain : graph_of_all_ca_chains_.chains_) {
    int hyper_period = GetHyperPeriod(tasks_info_, chain);
    if (IfRT_Trait(obj_trait_)) {
      LLint total_start_jobs =
          hyper_period / dag_tasks_.GetTask(chain[0]).period + 1;
      for (LLint start_instance_index = 0;
           start_instance_index <= total_start_jobs; start_instance_index++) {
        JobCEC start_job = {chain[0], (start_instance_index)};
        // JobCEC first_react_job = react_chain_map[start_job];
        JobCEC first_react_job =
            GetFirstReactJob(start_job, chains_perm, chain, tasks_info_);
        std::string const_name =
            GetReactConstraintName(chain_count, start_instance_index);
        if (obj_trait_ == "ReactionTime")
          AddTwoJobLengthConstraint(start_job, first_react_job, chain_count,
                                    start_instance_index);
        else
          AddTwoJobApproxLengthConstraint(start_job, first_react_job,
                                          chain_count, start_instance_index);
      }
    } else if (IfDA_Trait(obj_trait_)) {
      LLint total_start_jobs =
          hyper_period / dag_tasks_.GetTask(chain.back()).period + 1;
      for (LLint start_instance_index = 0;
           start_instance_index < total_start_jobs; start_instance_index++) {
        JobCEC start_job = {chain.back(), (start_instance_index)};
        JobCEC last_read_job =
            GetLastReadJob(start_job, chains_perm, chain, tasks_info_);
        std::string const_name =
            GetReactConstraintName(chain_count, start_instance_index);
        if (obj_trait_ == "DataAge")
          AddTwoJobLengthConstraint(last_read_job, start_job, chain_count,
                                    start_instance_index);
        else
          AddTwoJobApproxLengthConstraint(last_read_job, start_job, chain_count,
                                          start_instance_index);
      }
    }

    // Normal obj to optmize RTDA: obj = max_RTs + max_DAs
    rtda_expression += varArray_art_max_[chain_count];
    // if (optimize_jitter_weight_ != 0)
    //   rtda_expression +=
    //       (varArray_art_max_[chain_count] - varArray_art_min_[chain_count]) *
    //       optimize_jitter_weight_;
    // rtda_expression += theta_da;
    chain_count++;
  }
  model_.add(IloMinimize(env_, rtda_expression));
  rtda_expression.end();
#ifdef PROFILE_CODE
  EndTimer("AddObjective");
#endif
}

void LPOptimizer::AddSFObjectiveFunctions(
    const ChainsPermutation &chains_perm) {
  if (!(IfSF_Trait(obj_trait_))) return;
#ifdef PROFILE_CODE
  BeginTimer("AddObjective");
#endif
  IloExpr sf_expression(env_);
  int fork_count = 0;
  for (const auto &fork_curr : dag_tasks_.sf_forks_) {
    for (int job_id = 0; job_id < tasks_info_.hyper_period /
                                      dag_tasks_.GetTask(fork_curr.sink).period;
         job_id++) {
      SF_JobFork job_forks =
          GetSF_JobFork(JobCEC(fork_curr.sink, job_id), fork_curr.source,
                        tasks_info_, chains_perm);
      // TODO: more constraints are added?
      for (const auto &job1 : job_forks.source_jobs) {
        for (const auto &job2 : job_forks.source_jobs) {
          if (job1 != job2) AddTwoJobDiffConstraint(job1, job2, fork_count);
        }
      }
    }
    sf_expression += varArray_art_max_[fork_count];
    fork_count++;
  }
  model_.add(IloMinimize(env_, sf_expression));
  sf_expression.end();
#ifdef PROFILE_CODE
  EndTimer("AddObjective");
#endif
}

// TODO: SF can speed up by improving this function, but probably not very
// worthywhile?
void LPOptimizer::AddTwoJobDiffConstraint(const JobCEC &finish_job1,
                                          const JobCEC &finish_job2,
                                          int fork_count) {
  IloExpr finish_expr1 = GetFinishTimeExpression(finish_job1);
  IloExpr finish_expr2 = GetFinishTimeExpression(finish_job2);
  IloRange myConstraint1(
      env_, 0, varArray_art_max_[fork_count] - (finish_expr1 - finish_expr2),
      IloInfinity);
  // IloRange myConstraint2(
  //     env_, 0, varArray_art_max_[fork_count] - (finish_expr2 - finish_expr1),
  //     IloInfinity);
  if (GlobalVariablesDAGOpt::debugMode) {
    std::cout << myConstraint1 << "\n"
              << "\n";  //  << myConstraint2
  }
  model_.add(myConstraint1);
  // if (optimize_jitter_weight_ != 0) {
  //   IloRange myConstraint2(
  //       env_, 0, (finish_expr1 - finish_expr2) -
  //       varArray_art_min_[fork_count], IloInfinity);
  //   model_.add(myConstraint2);
  // }
  // model_.add(myConstraint2);
  finish_expr1.end();
  finish_expr2.end();
}

VariableOD LPOptimizer::ExtratOptSolution(IloNumArray &values_optimized) {
  VariableOD variable_od_opt(dag_tasks_.GetTaskSet());
  for (int task_id = 0; task_id < tasks_info_.N; task_id++) {
    variable_od_opt.SetOffset(
        task_id, values_optimized[GetVariableIndexVirtualOffset(task_id)]);
    if (optimize_offset_only_)
      variable_od_opt.SetDeadlineOnly(
          task_id, values_optimized[GetVariableIndexVirtualDeadline(task_id)]);
    else
      variable_od_opt.SetDeadline(
          task_id, values_optimized[GetVariableIndexVirtualDeadline(task_id)]);
  }
  // if (optimize_offset_only_)
  //   variable_od_opt.valid_ =
  //       true;  // TODO: always schedulable in Mart model unless the LP is
  //       not!!!
  return variable_od_opt;
}

// ****************following functions are commented**************

// void LPOptimizer::AddTwoJob_ForkConstraint(const JobCEC &finish_job1,
//                                            const JobCEC &finish_job2,
//                                            int fork_count, int
//                                            sink_job_index) {
//   // IloNumVarArray sf_fork_curr(env_, 1, 0, IloInfinity, IloNumVar::Float);
//   IloExpr finish_expr1 = GetFinishTimeExpression(finish_job1);
//   IloExpr finish_expr2 = GetFinishTimeExpression(finish_job2);
//   IloRange myConstraint1(env_, 0,
//                          varArray_sf_instances_[dag_tasks_.getSFInstance(
//                              fork_count, sink_job_index, tasks_info_)] -
//                              (finish_expr1 - finish_expr2),
//                          IloInfinity);
//   IloRange myConstraint2(env_, 0,
//                          varArray_sf_instances_[dag_tasks_.getSFInstance(
//                              fork_count, sink_job_index, tasks_info_)] -
//                              (finish_expr2 - finish_expr1),
//                          IloInfinity);
//   model_.add(myConstraint1);
//   model_.add(myConstraint2);
//   if (GlobalVariablesDAGOpt::debugMode) {
//     std::cout << myConstraint1 << "\n"
//               << "\n";  //  << myConstraint2
//   }
//   finish_expr1.end();
//   finish_expr2.end();
// }
// void LPOptimizer::AddForkSinkJobMinConstraint(int fork_count,
//                                               int sink_job_index) {
//   IloRange myConstraint3(env_, 0,
//                          varArray_sf_instances_[dag_tasks_.getSFInstance(
//                              fork_count, sink_job_index, tasks_info_)] -
//                              varArray_art_min_[fork_count],
//                          IloInfinity);
//   model_.add(myConstraint3);
//   if (GlobalVariablesDAGOpt::debugMode) {
//     std::cout << myConstraint3 << "\n"
//               << "\n";  //  << myConstraint2
//   }
// }

// void LPOptimizer::AddSFObjectiveFunctions_WithJitter(
//     const ChainsPermutation &chains_perm) {
//   if (!(IfSF_Trait(obj_trait_))) return;
// #ifdef PROFILE_CODE
//   BeginTimer("AddSFObjectiveFunctions_WithJitter");
// #endif
//   if (GlobalVariablesDAGOpt::debugMode) {
//     std::cout << "Add SF-Fork constraints:\n";
//   }
//   IloExpr sf_expression(env_);
//   int fork_count = 0;
//   for (const auto &fork_curr : dag_tasks_.sf_forks_) {
//     for (int job_id = 0; job_id < tasks_info_.hyper_period /
//                                       dag_tasks_.GetTask(fork_curr.sink).period;
//          job_id++) {
//       SF_JobFork job_forks =
//           GetSF_JobFork(JobCEC(fork_curr.sink, job_id), fork_curr.source,
//                         tasks_info_, chains_perm);
//       // TODO: more constraints are added?
//       for (const auto &job1 : job_forks.source_jobs) {
//         for (const auto &job2 : job_forks.source_jobs) {
//           if (job1 != job2) {
//             AddTwoJobDiffConstraint(job1, job2,
//                                     fork_count);  // add max-SF constraint
//             // AddTwoJob_ForkConstraint(
//             //     job1, job2, fork_count,
//             //     job_id);  // add another artificial variable to describe
//             the
//             // SF
//             // Fork result of this sink job
//           }
//         }
//       }
//       // AddForkSinkJobMinConstraint(fork_count,
//       //                             job_id);  // add min-SF constraints
//     }
//     sf_expression += varArray_art_max_[fork_count] +
//                      optimize_jitter_weight_ * (varArray_art_max_[fork_count]
//                      -
//                                                 varArray_art_min_[fork_count]);

//     fork_count++;
//   }
//   if (GlobalVariablesDAGOpt::debugMode) {
//     std::cout << "Add SF-Fork constraints finished:\n";
//   }
//   model_.add(IloMinimize(env_, sf_expression));
//   sf_expression.end();
// #ifdef PROFILE_CODE
//   EndTimer("AddSFObjectiveFunctions_WithJitter");
// #endif
// }

// TO_Commented_DO: consider whether it's necessary to improve efficiency there
// by reducing problem size; int LPOptimizer::FindMinOffset(int task_id,
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