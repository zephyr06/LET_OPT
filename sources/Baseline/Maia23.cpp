

#include "sources/Baseline/Maia23.h"
namespace DAG_SPACE {

VariableOD GetMaia23VariableOD(const DAG_Model& dag_tasks,
                               const TaskSetInfoDerived& tasks_info,
                               const Schedule& schedule) {
  VariableOD variable(dag_tasks.tasks);
  for (uint i = 0; i < dag_tasks.tasks.size(); i++) {
    variable[i].offset = dag_tasks.GetTask(i).deadline;
    variable[i].deadline = 0;
  }
  for (auto itr = schedule.begin(); itr != schedule.end(); itr++) {
    const JobCEC job_cur = itr->first;
    const JobStartFinish& startfinish = itr->second;
    int initial_release_time =
        job_cur.jobId * tasks_info.GetTask(job_cur.taskId).period;
    int rel_start = startfinish.start - initial_release_time;
    int rel_finish = startfinish.finish - initial_release_time;
    variable[job_cur.taskId].offset =
        min(variable[job_cur.taskId].offset, rel_start);
    variable[job_cur.taskId].deadline =
        max(variable[job_cur.taskId].deadline, rel_finish);
  }
  return variable;
}

VariableOD GetMaia23VariableOD(const DAG_Model& dag_tasks,
                               const TaskSetInfoDerived& tasks_info) {
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
  return GetMaia23VariableOD(dag_tasks, tasks_info, schedule);
}
}  // namespace DAG_SPACE