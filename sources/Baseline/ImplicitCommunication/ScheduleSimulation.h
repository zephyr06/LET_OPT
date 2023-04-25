#pragma once
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/JobCEC.h"
#include "sources/Utils/MatirxConvenient.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/colormod.h"
#include "unordered_map"
namespace DAG_SPACE {

struct JobStartFinish {
  JobStartFinish() {}
  JobStartFinish(int s, int f) : start(s), finish(f) {}

  bool operator==(const JobStartFinish &other) const {
    return start == other.start && finish == other.finish;
  }
  bool operator!=(const JobStartFinish &other) const {
    return !(*this == other);
  }
  // data members
  int start;
  int finish;
};

typedef std::unordered_map<JobCEC, JobStartFinish> Schedule;

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

class RunQueue {
 public:
  RunQueue(const TaskSetInfoDerived tasks_info) : tasks_info_(tasks_info) {
    N = tasks_info.N;
    job_queue_.reserve(N);
    schedule_.reserve(tasks_info.length);
  }

  inline size_t size() const { return job_queue_.size(); }

  Schedule inline GetSchedule() const { return schedule_; }
  void inline RangeCheck(size_t job_info_index_in_queue) {
    if (job_info_index_in_queue < 0 ||
        job_info_index_in_queue >= job_queue_.size())
      CoutError("Index out of range in RunQueue");
  }

  void insert(JobCEC job) {
    JobScheduleInfo job_info(job);
    if (job_queue_.size() == 0)
      job_queue_.push_back(job_info);
    else {
      LLint prev = FindPrev(job.taskId);
      job_queue_.insert(job_queue_.begin() + prev, job_info);
    }
  }

  // exam whether the running job has highest priority, if not, preempt it
  void RunJobHigestPriority(int time_now) {
    if (empty())
      return;
    else if (size() == 1) {
      RunJob(0, time_now);
    } else {
      auto job_info = front();
      if (job_info.running)
        return;
      else {
        PreemptJob(time_now);  // preempty running jobs, if any
        if (!RunJob(0, time_now))
          CoutError("Error in RunQueue RunJobHigestPriority!");
      }
    }
  }

  void PreemptJob(int time_now) {
    // TODO: improve efficiency
    for (uint i = 0; i < size(); i++) {
      PreemptJob(i, time_now);
    }
  }

  void PreemptJob(size_t job_info_index_in_queue, int time_now) {
    RangeCheck(job_info_index_in_queue);
    JobScheduleInfo &job_info = job_queue_[job_info_index_in_queue];
    if (job_info.running == false)
      return;
    else {
      job_info.UpdateAccumTime(time_now);
      job_info.running = false;
      processor_free_ = true;
    }
  }

  bool RunJob(size_t job_info_index_in_queue, int time_now) {
    RangeCheck(job_info_index_in_queue);
    if (!processor_free_) return false;
    processor_free_ = false;
    JobScheduleInfo &job_info = job_queue_[job_info_index_in_queue];
    job_info.StartRun(time_now);
    JobStartFinish job_sf(time_now, -1);
    schedule_[job_info.job] = job_sf;
    next_free_time_ = time_now +
                      tasks_info_.GetTask(job_info.job.taskId).executionTime -
                      job_info.accum_run_time;
    return true;
  }

  void RemoveFinishedJob(int time_now) {
    for (size_t i = 0; i < job_queue_.size(); i++) {
      auto &job_info = job_queue_[i];
      if (job_info.running) {
        job_info.UpdateAccumTime(time_now);
      }
      if (job_info.IfFinished(tasks_info_)) {
        if (job_info.running) {
          job_info.running = false;
          schedule_[job_info.job].finish = time_now;
          processor_free_ = true;
        }
        job_queue_.erase(job_queue_.begin() + i);
      }
    }
  }

  /**
   * @brief Given a new task's id, find its position in the queue
   * such that the queue is ordered from highest priority to lowest priority
   *
   * @param id: new task's id
   * @return LLint
   */
  LLint FindPrev(LLint task_id) {
    Task taskCurr = tasks_info_.GetTask(task_id);
    double priorityCurr = taskCurr.priority();

    LLint left = 0;
    LLint right = job_queue_.size();
    while (left < right) {
      LLint mid = (left + right) / 2;
      // double priorityMid = tasks[job_queue_[mid].job.taskId].priority();
      double priorityMid =
          tasks_info_.GetTask(job_queue_[mid].job.taskId).priority();
      if (priorityMid == priorityCurr) {
        return mid;
      } else if (priorityCurr < priorityMid) {
        left = mid + 1;
      } else {
        right = mid;
      }
    }
    return left;
  }

  void erase(LLint task_id) {
    for (size_t i = 0; i < job_queue_.size(); i++) {
      if (job_queue_[i].job.taskId == task_id) {
        job_queue_.erase(job_queue_.begin() + i);
        return;
      }
    }
    CoutError("The element to erase is not found!");
  }

  inline JobScheduleInfo front() const {
    if (job_queue_.empty()) CoutError("job_queue_ is empty!");

    return job_queue_[0];
  }

  inline JobScheduleInfo PopFront() {
    auto first = front();
    job_queue_.erase(job_queue_.begin());
    return first;
  }

  inline bool empty() { return job_queue_.empty(); }

  // data members

  // typedef std::pair<int, LLint> ID_INSTANCE_PAIR;  // taskId, jobId
  const TaskSetInfoDerived tasks_info_;
  int N;
  std::vector<JobScheduleInfo> job_queue_;
  int next_free_time_ = 0;
  bool processor_free_ = true;
  Schedule schedule_;
};

void AddTasksToRunQueue(RunQueue &run_queue, const TaskSet &tasks,
                        int processor_id, LLint time_now);

Schedule SimulatedFTP_SingleCore(const DAG_Model &dag_tasks,
                                 const TaskSetInfoDerived &tasks_info,
                                 int processor_id);

VectorDynamic SimulateFixedPrioritySched(const DAG_Model &dag_tasks,
                                         const TaskSetInfoDerived &tasks_info,
                                         LLint timeInitial = 0);

}  // namespace DAG_SPACE
