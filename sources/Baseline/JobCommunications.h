#pragma once

#include "sources/Permutations/ChainsPermutation.h"
#include "sources/Permutations/TwoTaskPermutations.h"
#include "sources/Utils/BatchUtils.h"
namespace DAG_SPACE {

PermutationInequality GetPermIneq(const DAG_Model& dag_tasks,
                                  const TaskSetInfoDerived& tasks_info,
                                  int prev_task_id, int next_task_id,
                                  const std::string& type_trait);

JobCEC GetPossibleReactingJobs(
    const JobCEC& job_curr, const Task& task_next, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
    const VariableOD& variable_od);

JobCEC GetPossibleReadingJobs(
    const JobCEC& job_curr, const Task& task_prev, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
    const VariableOD& variable_od);

std::unordered_map<JobCEC, JobCEC> GetJobMatch(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    int prev_task_id, int next_task_id, const std::string& type_trait,
    const VariableOD& variable_od);

std::unordered_map<JobCEC, JobCEC> GetJobMatch(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    int prev_task_id, int next_task_id, const std::string& type_trait,
    const VariableOD& variable_od);

SinglePairPermutation GetSinglePermFromVariable(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    int prev_task_id, int next_task_id, const std::string& type_trait,
    const VariableOD& variable_od);

ChainsPermutation GetChainsPermFromVariable(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    const std::vector<std::vector<int>>& task_chains,
    const std::string& type_trait, const VariableOD& variable_od);

}  // namespace DAG_SPACE