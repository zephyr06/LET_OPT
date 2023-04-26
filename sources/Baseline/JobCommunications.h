#pragma once

#include "sources/Baseline/JobScheduleInfo.h"
#include "sources/Permutations/ChainsPermutation.h"
#include "sources/Permutations/TwoTaskPermutations.h"
#include "sources/Utils/BatchUtils.h"
namespace DAG_SPACE {

inline PermutationInequality GetPermIneq(const DAG_Model& dag_tasks,
                                  const TaskSetInfoDerived& tasks_info,
                                  int prev_task_id, int next_task_id,
                                  const std::string& type_trait) {
  return PermutationInequality(prev_task_id, next_task_id, type_trait);
}

JobCEC GetPossibleReactingJobs(
    const JobCEC& job_curr, const Task& task_next, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
    const Schedule& schedule);

JobCEC GetPossibleReadingJobs(
    const JobCEC& job_curr, const Task& task_prev, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
    const Schedule& schedule);

std::unordered_map<JobCEC, JobCEC> GetJobMatch(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    int prev_task_id, int next_task_id, const std::string& type_trait,
    const Schedule& schedule);

inline SinglePairPermutation GetSinglePermFromVariable(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    int prev_task_id, int next_task_id, const std::string& type_trait,
    const Schedule& schedule) {
  return SinglePairPermutation(GetPermIneq(dag_tasks, tasks_info, prev_task_id,
                                           next_task_id, type_trait),
                               GetJobMatch(dag_tasks, tasks_info, prev_task_id,
                                           next_task_id, type_trait, schedule),
                               type_trait);
}

ChainsPermutation GetChainsPermFromVariable(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    const std::vector<std::vector<int>>& task_chains,
    const std::string& type_trait, const Schedule& schedule);

// ******************************************
// Some code that's only used for unit tests

inline JobCEC GetPossibleReactingJobs(
    const JobCEC& job_curr, const Task& task_next, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
    const VariableOD& variable_od) {
  Schedule schedule_actual = Variable2Schedule(tasks_info, variable_od);
  return GetPossibleReactingJobs(job_curr, task_next, superperiod, tasks_info,
                                 schedule_actual);
}
inline JobCEC GetPossibleReadingJobs(
    const JobCEC& job_curr, const Task& task_prev, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
    const VariableOD& variable_od) {
  Schedule schedule_actual = Variable2Schedule(tasks_info, variable_od);
  return GetPossibleReadingJobs(job_curr, task_prev, superperiod, tasks_info,
                                schedule_actual);
}
inline JobCEC GetPossibleReadingJobsMart(
    const JobCEC& job_curr, const Task& task_prev, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
    const VariableOD& variable_od) {
  Schedule schedule_actual = VariableMart2Schedule(tasks_info, variable_od);
  return GetPossibleReadingJobs(job_curr, task_prev, superperiod, tasks_info,
                                schedule_actual);
}

inline std::unordered_map<JobCEC, JobCEC> GetJobMatch(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    int prev_task_id, int next_task_id, const std::string& type_trait,
    const VariableOD& variable_od) {
  Schedule schedule_actual = Variable2Schedule(tasks_info, variable_od);
  return GetJobMatch(dag_tasks, tasks_info, prev_task_id, next_task_id,
                     type_trait, schedule_actual);
}

inline SinglePairPermutation GetSinglePermFromVariable(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    int prev_task_id, int next_task_id, const std::string& type_trait,
    const VariableOD& variable_od) {
  Schedule schedule_actual = Variable2Schedule(tasks_info, variable_od);
  return GetSinglePermFromVariable(dag_tasks, tasks_info, prev_task_id,
                                   next_task_id, type_trait, schedule_actual);
}

inline ChainsPermutation GetChainsPermFromVariable(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    const std::vector<std::vector<int>>& task_chains,
    const std::string& type_trait, const VariableOD& variable_od) {
  Schedule schedule_actual = Variable2Schedule(tasks_info, variable_od);
  return GetChainsPermFromVariable(dag_tasks, tasks_info, task_chains,
                                   type_trait, schedule_actual);
}

inline ChainsPermutation GetChainsPermFromMartVariable(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    const std::vector<std::vector<int>>& task_chains,
    const std::string& type_trait, const VariableOD& variable_od) {
  Schedule schedule_actual = VariableMart2Schedule(tasks_info, variable_od);
  return GetChainsPermFromVariable(dag_tasks, tasks_info, task_chains,
                                   type_trait, schedule_actual);
}

// ******************************************
}  // namespace DAG_SPACE