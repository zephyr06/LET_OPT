

#include "sources/Baseline/Maia23.h"
namespace DAG_SPACE {

VariableOD GetMaia23VariableOD(const DAG_Model& dag_tasks,
                               const TaskSetInfoDerived& tasks_info) {
  Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
  VariableOD variable(dag_tasks.tasks);
  for (auto itr = schedule.begin(); itr != schedule.end(); itr++) {
    const JobCEC job_cur = itr->first;
    const JobStartFinish& startfinish = itr->second;
    int initial_release_time =
        job_cur.jobId * tasks_info.GetTask(job_cur.taskId).period;
    int rel_start = startfinish.start - initial_release_time;
    int rel_finish = startfinish.finish - initial_release_time;
    variable[job_cur.taskId].offset =
        max(variable[job_cur.taskId].offset, rel_start);
    variable[job_cur.taskId].deadline =
        min(variable[job_cur.taskId].deadline, rel_finish);
  }
  return variable;
}

}  // namespace DAG_SPACE