#pragma once

#include "sources/Baseline/ImplicitCommunication/ScheduleSimulation.h"
#include "sources/Baseline/JobCommunications.h"
#include "sources/Optimization/Variable.h"
#include "sources/Permutations/ChainsPermutation.h"
#include "sources/Permutations/TwoTaskPermutations.h"
#include "sources/Utils/BatchUtils.h"
namespace DAG_SPACE {

VariableOD GetMaia23VariableOD(const DAG_Model& dag_tasks,
                               const TaskSetInfoDerived& tasks_info,
                               const Schedule& schedule);

VariableOD GetMaia23VariableOD(const DAG_Model& dag_tasks,
                               const TaskSetInfoDerived& tasks_info);
                               
template <typename ObjectiveFunctionBase>
ScheduleResult PerformMaia23Analysis(const DAG_Model& dag_tasks) {
  auto start = std::chrono::high_resolution_clock::now();

  ScheduleResult res;
  const TaskSet& tasks = dag_tasks.GetTaskSet();
  TaskSetInfoDerived tasks_info(tasks);
  VariableOD variable_od_Maia = GetMaia23VariableOD(dag_tasks, tasks_info);
  Schedule schedule_actual = Variable2Schedule(tasks_info, variable_od_Maia);

  ChainsPermutation chains_perm = GetChainsPermFromVariable(
      dag_tasks, tasks_info, dag_tasks.chains_,
      ObjectiveFunctionBase::type_trait, schedule_actual);
  res.obj_ = ObjectiveFunctionBase::Obj(dag_tasks, tasks_info, chains_perm,
                                        variable_od_Maia, dag_tasks.chains_);
  res.jitter_ = ObjectiveFunctionBase::Jitter(
      dag_tasks, tasks_info, chains_perm, variable_od_Maia, dag_tasks.chains_);

  res.schedulable_ = CheckSchedulability(dag_tasks);

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  res.timeTaken_ = double(duration.count()) / 1e6;
  return res;
}
}  // namespace DAG_SPACE