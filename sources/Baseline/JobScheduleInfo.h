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

Schedule Variable2Schedule(const TaskSetInfoDerived &tasks_info,
                           const VariableOD &variable_od);

// inline int GetDeadlineMart(const JobCEC &job_curr,
//                            const VariableOD &variable_od,
//                            const TaskSetInfoDerived &tasks_info) {
//   return variable_od.at(job_curr.taskId).offset +
//          variable_od.at(job_curr.taskId).deadline +
//          tasks_info.GetTask(job_curr.taskId).period * job_curr.jobId;
// }

Schedule VariableMart2Schedule(const TaskSetInfoDerived &tasks_info,
                               const VariableOD &variable_od);

void PrintSchedule(const Schedule &schedule) {
  for (auto itr = schedule.begin(); itr != schedule.end(); itr++) {
    JobCEC job = itr->first;
    JobStartFinish sf = itr->second;
    std::cout << "(" << job.taskId << ", " << job.jobId << ")"
              << ": " << sf.start << ", " << sf.finish << "\n";
  }
}

bool CheckSchedulability(const DAG_Model &dag_tasks, const Schedule &schedule);

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

struct IndexInfoMultiHp {
  IndexInfoMultiHp(int hp_index, int job_index_within_hp)
      : hp_index(hp_index), job_index_within_hp(job_index_within_hp) {}
  int hp_index;
  int job_index_within_hp;
};

inline IndexInfoMultiHp GetIndexInfoMultiHp(
    const JobCEC &job_curr, const TaskSetInfoDerived &tasks_info) {
  int jobs_per_hp =
      tasks_info.hyper_period / tasks_info.GetTask(job_curr.taskId).period;
  int hp_index = std::floor(float(job_curr.jobId) / jobs_per_hp);
  int job_index_within_hp =
      (job_curr.jobId % jobs_per_hp + jobs_per_hp) % jobs_per_hp;
  return IndexInfoMultiHp(hp_index, job_index_within_hp);
}

inline int GetStartTime(const JobCEC &job_curr, const Schedule &schedule,
                        const TaskSetInfoDerived &tasks_info) {
  IndexInfoMultiHp info = GetIndexInfoMultiHp(job_curr, tasks_info);
  JobCEC job_mapped_hp(job_curr.taskId, info.job_index_within_hp);
  if (schedule.find(job_mapped_hp) == schedule.end()) {
    CoutError("Schedule didn't find job!");
  }
  return schedule.at(job_mapped_hp).start +
         info.hp_index * tasks_info.hyper_period;
}

inline int GetFinishTime(const JobCEC &job_curr, const Schedule &schedule,
                         const TaskSetInfoDerived &tasks_info) {
  IndexInfoMultiHp info = GetIndexInfoMultiHp(job_curr, tasks_info);
  JobCEC job_mapped_hp(job_curr.taskId, info.job_index_within_hp);
  if (schedule.find(job_mapped_hp) == schedule.end()) {
    CoutError("Schedule didn't find job!");
  }
  return schedule.at(job_mapped_hp).finish +
         info.hp_index * tasks_info.hyper_period;
}

}  // namespace DAG_SPACE