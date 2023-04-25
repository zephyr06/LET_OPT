
#include "sources/Baseline/ImplicitCommunication/ScheduleSimulation.h"

namespace DAG_SPACE {

void AddTasksToRunQueue(RunQueue &run_queue, const TaskSet &tasks,
                        int processor_id, LLint time_now) {
  for (uint i = 0; i < tasks.size(); i++) {
    if (tasks[i].processorId == processor_id &&
        time_now % tasks[i].period == 0) {
      JobCEC job_curr(i, time_now / tasks[i].period);
      run_queue.insert(job_curr);
    }
  }
}

Schedule SimulatedFTP_SingleCore(const DAG_Model &dag_tasks,
                                 const TaskSetInfoDerived &tasks_info,
                                 int processor_id) {
  const TaskSet &tasks = dag_tasks.GetTaskSet();
  RunQueue run_queue(tasks);
  for (LLint time_now = 0; time_now < tasks_info.hyper_period; time_now++) {
    // first remove jobs that have been finished at this time
    run_queue.RemoveFinishedJob(time_now);

    // check whether to add new instances
    AddTasksToRunQueue(run_queue, tasks, processor_id, time_now);

    // Run jobs with highest priority
    run_queue.RunJobHigestPriority(time_now);
  }
  return run_queue.GetSchedule();
}

VectorDynamic SimulateFixedPrioritySched(const DAG_Model &dag_tasks,
                                         const TaskSetInfoDerived &tasks_info,
                                         LLint timeInitial) {
  VectorDynamic initial = GenerateVectorDynamic(tasks_info.variableDimension);

  // ProcessorTaskSet processorTaskSet =
  // ExtractProcessorTaskSet(dag_tasks.tasks); int processorNum =
  // processorTaskSet.size();
  // // it maps from tasks[i].processorId to index in runQueues&busy&nextFree
  // ProcessorId2Index processorId2Index = CreateProcessorId2Index(tasks);
  // // contains the index of tasks to run

  // std::vector<RunQueue> runQueues;
  // runQueues.reserve(processorNum);
  // for (int i = 0; i < processorNum; i++) {
  //   runQueues.push_back(RunQueue(tasks));
  // }

  // std::vector<bool> busy(processorNum, false);
  // std::vector<LLint> nextFree(processorNum, -1);
  // // LLint nextFree;

  // for (LLint time_now = timeInitial; time_now < tasks_info.hyperPeriod;
  //      time_now++) {
  //   // check whether to add new instances
  //   AddTasksToRunQueues(runQueues, tasks, processorId2Index, time_now);

  //   for (int i = 0; i < processorNum; i++) {
  //     if (time_now >= nextFree[i]) {
  //       busy[i] = false;
  //     }
  //     if (time_now >= nextFree[i] && (!runQueues[i].empty())) {
  //       auto sth = runQueues[i].pop();
  //       UpdateSTVAfterPopTask(sth, initial, time_now, nextFree, tasks, busy,
  //       i,
  //                             tasks_info.sizeOfVariables);
  //     }
  //   }
  // }

  return initial;
}

}  // namespace DAG_SPACE
