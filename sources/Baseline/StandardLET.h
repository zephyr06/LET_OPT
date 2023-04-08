#pragma once

#include "sources/Permutations/ChainsPermutation.h"
#include "sources/Permutations/TwoTaskPermutations.h"
#include "sources/Utils/BatchUtils.h"
namespace DAG_SPACE {

PermutationInequality GetPermIneq(const DAG_Model& dag_tasks,
                                  const TaskSetInfoDerived& tasks_info,
                                  int prev_task_id, int next_task_id,
                                  const std::string& type_trait);

JobCEC GetPossibleReactingJobsLET(
    const JobCEC& job_curr, const Task& task_next, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info);

std::unordered_map<JobCEC, JobCEC> GetJobMatch(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    int prev_task_id, int next_task_id, const std::string& type_trait);

SinglePairPermutation GetSinglePermutationStanLET(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    int prev_task_id, int next_task_id, const std::string& type_trait);

ChainsPermutation GetStandardLETChain(
    const DAG_Model& dag_tasks, const TaskSetInfoDerived& tasks_info,
    const std::vector<std::vector<int>>& task_chains,
    const std::string& type_trait);

template <typename ObjectiveFunctionBase>
ScheduleResult PerformStandardLETAnalysis(const DAG_Model& dag_tasks) {
  auto start = std::chrono::high_resolution_clock::now();

  ScheduleResult res;
  const TaskSet& tasks = dag_tasks.GetTaskSet();
  TaskSetInfoDerived tasks_info(tasks);
  ChainsPermutation chains_perm =
      GetStandardLETChain(dag_tasks, tasks_info, dag_tasks.chains_,
                          ObjectiveFunctionBase::type_trait);
  VariableOD variable_od = VariableOD(tasks);
  res.obj_ = ObjectiveFunctionBase::Obj(dag_tasks, tasks_info, chains_perm,
                                        variable_od, dag_tasks.chains_);
  res.schedulable_ = CheckSchedulability(dag_tasks);

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  res.timeTaken_ = double(duration.count()) / 1e6;
  return res;
}
}  // namespace DAG_SPACE