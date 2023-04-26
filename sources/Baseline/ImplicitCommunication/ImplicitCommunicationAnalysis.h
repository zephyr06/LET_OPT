#pragma once
#include "sources/Baseline/ImplicitCommunication/ScheduleSimulation.h"

namespace DAG_SPACE {

template <typename ObjectiveFunctionBase>
ScheduleResult PerformImplicitCommuAnalysis(const DAG_Model& dag_tasks) {
  auto start = std::chrono::high_resolution_clock::now();

  ScheduleResult res;
  const TaskSet& tasks = dag_tasks.GetTaskSet();
  TaskSetInfoDerived tasks_info(tasks);
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
  ChainsPermutation chains_perm =
      GetChainsPermFromVariable(dag_tasks, tasks_info, dag_tasks.chains_,
                                ObjectiveFunctionBase::type_trait, schedule);
  //   chains_perm.print();
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
