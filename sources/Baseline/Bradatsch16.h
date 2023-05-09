#pragma once

#include "sources/Baseline/JobCommunications.h"
#include "sources/Permutations/ChainsPermutation.h"
#include "sources/Permutations/TwoTaskPermutations.h"
#include "sources/Utils/BatchUtils.h"
namespace DAG_SPACE {

VariableOD GetVariableOD_Bradatsch16(const DAG_Model& dag_tasks);

template <typename ObjectiveFunctionBase>
ScheduleResult PerformBradatsch16LETAnalysis(const DAG_Model& dag_tasks) {
  auto start = std::chrono::high_resolution_clock::now();

  ScheduleResult res;
  const TaskSet& tasks = dag_tasks.GetTaskSet();
  TaskSetInfoDerived tasks_info(tasks);
  VariableOD variable_od_let = GetVariableOD_Bradatsch16(dag_tasks);

  Schedule schedule_actual = Variable2Schedule(tasks_info, variable_od_let);
  ChainsPermutation chains_perm = GetChainsPermFromVariable(
      dag_tasks, tasks_info, dag_tasks.chains_,
      ObjectiveFunctionBase::type_trait, schedule_actual);
  res.obj_ = ObjectiveFunctionBase::Obj(dag_tasks, tasks_info, chains_perm,
                                        variable_od_let, dag_tasks.chains_);

  res.schedulable_ = CheckSchedulability(dag_tasks);

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  res.timeTaken_ = double(duration.count()) / 1e6;
  return res;
}
}  // namespace DAG_SPACE