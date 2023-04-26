
#include "sources/Baseline/JobScheduleInfo.h"
namespace DAG_SPACE {
Schedule Variable2Schedule(const TaskSetInfoDerived &tasks_info,
                           const VariableOD &variable_od) {
  Schedule schedule_all;
  schedule_all.reserve(tasks_info.length);
  for (int i = 0; i < tasks_info.N; i++) {
    Task task_curr = tasks_info.GetTask(i);
    for (int j = 0; j < int(tasks_info.hyper_period / task_curr.period); j++) {
      JobCEC job_curr(i, j);
      JobStartFinish jsf(GetStartTime(job_curr, variable_od, tasks_info),
                         GetDeadline(job_curr, variable_od, tasks_info));
      schedule_all[job_curr] = jsf;
    }
  }
  return schedule_all;
}

Schedule VariableMart2Schedule(const TaskSetInfoDerived &tasks_info,
                               const VariableOD &variable_od) {
  Schedule schedule_all;
  schedule_all.reserve(tasks_info.length);
  for (int i = 0; i < tasks_info.N; i++) {
    Task task_curr = tasks_info.GetTask(i);
    for (int j = 0; j < int(tasks_info.hyper_period / task_curr.period); j++) {
      JobCEC job_curr(i, j);
      JobStartFinish jsf(
          GetStartTime(job_curr, variable_od,
                       tasks_info),  // GetStartTime equals GetStartMart
          GetDeadlineMart(job_curr, variable_od, tasks_info));
      schedule_all[job_curr] = jsf;
    }
  }
  return schedule_all;
}
}  // namespace DAG_SPACE