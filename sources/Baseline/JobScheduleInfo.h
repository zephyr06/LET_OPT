#pragma once
#include "sources/Optimization/Variable.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/JobCEC.h"
#include "unordered_map"

namespace DAG_SPACE {
struct JobStartFinish {
  JobStartFinish() {}
  JobStartFinish(int s, int f) : start(s), finish(f) {}

  inline bool operator==(const JobStartFinish &other) const {
    return start == other.start && finish == other.finish;
  }
  inline bool operator!=(const JobStartFinish &other) const {
    return !(*this == other);
  }
  // data members
  int start;
  int finish;
};

typedef std::unordered_map<JobCEC, JobStartFinish> Schedule;

Schedule Variable2Schedule(const DAG_Model &dag_tasks,
                           const TaskSetInfoDerived &tasks_info,
                           const VariableOD &variable_od);
struct JobScheduleInfo {
  JobScheduleInfo(JobCEC job) : job(job), accum_run_time(0) {}

  inline bool IfFinished(const TaskSetInfoDerived &tasks_info) const {
    return accum_run_time >= tasks_info.GetTask(job.taskId).executionTime;
  }

  inline void UpdateAccumTime(int time_now) {
    if (running) {
      accum_run_time += time_now - start_time_last;
      start_time_last = time_now;
    }
  }

  inline void StartRun(int time_now) {
    if (!running) {
      running = true;
      start_time_last = time_now;
    }
  }
  // data members
  JobCEC job;
  int start_time_last;
  int accum_run_time;
  bool running = false;
};

}  // namespace DAG_SPACE