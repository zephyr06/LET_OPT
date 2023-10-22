
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
          GetDeadline(job_curr, variable_od, tasks_info));
      schedule_all[job_curr] = jsf;
    }
  }
  return schedule_all;
}

void PrintSchedule(const Schedule &schedule) {
  for (auto itr = schedule.begin(); itr != schedule.end(); itr++) {
    JobCEC job = itr->first;
    JobStartFinish sf = itr->second;
    std::cout << "(" << job.taskId << ", " << job.jobId << ")"
              << ": " << sf.start << ", " << sf.finish << "\n";
  }
}

bool CheckSchedulability(const DAG_Model &dag_tasks,
                         const TaskSetInfoDerived &tasks_info,
                         const Schedule &schedule, const VariableOD &variable) {
  for (int i = 0; i < tasks_info.N; i++) {
    const Task &task_curr = tasks_info.GetTask(i);
    for (int j = 0; j < int(tasks_info.hyper_period / task_curr.period); j++) {
      JobCEC job_curr(i, j);
      if (schedule.at(job_curr).finish >
          GetActivationTime(job_curr, tasks_info) + variable.at(i).deadline) {
        CoutWarning("Unschedulable after Maia!");
        return false;
      }
    }
  }
  return true;
}

std::vector<int> GetResponseTimeTaskSet(const DAG_Model &dag_tasks,
                                        const TaskSetInfoDerived &tasks_info,
                                        const Schedule &schedule) {
  std::vector<int> res(tasks_info.N, 0);
  for (int task_id = 0; task_id < tasks_info.N; task_id++) {
    int rta_cur = 0;
    for (int job_id = 0;
         job_id < tasks_info.hyper_period / tasks_info.GetTask(task_id).period;
         job_id++) {
      JobCEC job_cur(task_id, job_id);
      rta_cur = max(rta_cur, int(schedule.at(job_cur).length()));
    }
    res[task_id] = rta_cur;
  }
  return res;
}
}  // namespace DAG_SPACE