
#include "sources/Baseline/ImplicitCommunication/ScheduleSimulation.h"

namespace DAG_SPACE {

void AddTasksToRunQueue(RunQueue &run_queue, const DAG_Model &dag_tasks,
                        int processor_id, LLint time_now) {
  for (int task_id = 0;
       task_id < static_cast<int>(dag_tasks.GetTaskSet().size()); task_id++) {
    Task task_curr = dag_tasks.GetTask(task_id);
    if (task_curr.processorId == processor_id &&
        time_now % task_curr.period == 0) {
      JobCEC job_curr(task_id, time_now / task_curr.period);
      run_queue.insert(job_curr);
    }
  }
}

Schedule SimulatedFTP_SingleCore(const DAG_Model &dag_tasks,
                                 const TaskSetInfoDerived &tasks_info,
                                 int processor_id) {
  const TaskSet &tasks = dag_tasks.GetTaskSet();
  RunQueue run_queue(tasks);
  for (LLint time_now = 0; time_now <= tasks_info.hyper_period; time_now++) {
    // first remove jobs that have been finished at this time
    run_queue.RemoveFinishedJob(time_now);

    // check whether to add new instances
    AddTasksToRunQueue(run_queue, dag_tasks, processor_id, time_now);

    // Run jobs with highest priority
    run_queue.RunJobHigestPriority(time_now);
  }
  return run_queue.GetSchedule();
}

std::vector<int> GetProcessorIds(const DAG_Model &dag_tasks) {
  std::vector<int> processor_ids;
  std::unordered_set<int> id_record;
  processor_ids.reserve(dag_tasks.GetTaskSet().size());
  for (uint task_id = 0; task_id < dag_tasks.GetTaskSet().size(); task_id++) {
    int processor_id = dag_tasks.GetTask(task_id).processorId;
    if (id_record.find(processor_id) == id_record.end()) {
      id_record.insert(processor_id);
      processor_ids.push_back(processor_id);
    }
  }
  return processor_ids;
}

Schedule SimulateFixedPrioritySched(const DAG_Model &dag_tasks,
                                    const TaskSetInfoDerived &tasks_info) {
  Schedule schedule_all;
  schedule_all.reserve(tasks_info.length);
  std::vector<int> processor_ids = GetProcessorIds(dag_tasks);
  for (int processor_id : processor_ids) {
    Schedule schedule_curr =
        SimulatedFTP_SingleCore(dag_tasks, tasks_info, processor_id);
    schedule_all.insert(schedule_curr.begin(), schedule_curr.end());
  }

  return schedule_all;
}

}  // namespace DAG_SPACE
