#pragma once

#include <memory>

#include "sources/Optimization/Variable.h"
#include "sources/Permutations/Edge.h"
#include "sources/Permutations/PermutationInequality.h"
#include "sources/Permutations/SinglePairPermutation.h"
#include "sources/Utils/profilier.h"
namespace DAG_SPACE {

std::vector<JobCEC> GetPossibleReactingJobs(
    const JobCEC& job_curr, const Task& task_next, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasksInfo);

std::vector<JobCEC> GetPossibleReadingJobs(
    const JobCEC& job_curr, const Task& task_prev, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasksInfo);

PermutationInequality GenerateBoxPermutationConstraints(
    int task_prev_id, int task_next_id, const VariableRange& variable_range);

bool ifTimeout(TimerType start_time);

typedef std::vector<std::shared_ptr<const SinglePairPermutation>> PermPtrVec;
// typedef std::unordered_set<std::shared_ptr<const SinglePairPermutation>>
//     PermPtrSet;
typedef std::unordered_set<SinglePairPermutation> PermRefSet;
class TwoTaskPermutations {
 public:
  TwoTaskPermutations() {}
  TwoTaskPermutations(int task_prev_id, int task_next_id,
                      const DAG_Model& dag_tasks,
                      const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
                      const std::string& type_trait)
      : start_time_((std::chrono::high_resolution_clock::now())),
        task_prev_id_(task_prev_id),
        task_next_id_(task_next_id),
        tasks_info_(tasks_info),
        type_trait_(type_trait),
        perm_count_(0) {
    superperiod_ = GetSuperPeriod(tasks_info.GetTask(task_prev_id),
                                  tasks_info.GetTask(task_next_id));
    variable_od_range_ = FindVariableRange(dag_tasks);
    single_permutations_.reserve(1e4);
    FindAllPermutations();
  }
  TwoTaskPermutations(int task_prev_id, int task_next_id,
                      const DAG_Model& dag_tasks,
                      const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
                      const std::vector<int>& rta,
                      const std::string& type_trait, int perm_count_global = 0)
      : start_time_((std::chrono::high_resolution_clock::now())),
        task_prev_id_(task_prev_id),
        task_next_id_(task_next_id),
        tasks_info_(tasks_info),
        type_trait_(type_trait),
        perm_count_(perm_count_global),
        perm_count_base_(0) {
    superperiod_ = GetSuperPeriod(tasks_info.GetTask(task_prev_id),
                                  tasks_info.GetTask(task_next_id));
    variable_od_range_ = FindVariableRange(dag_tasks, rta);
    single_permutations_.reserve(1e4);
    FindAllPermutations();
  }

  inline size_t size() const { return single_permutations_.size(); }

  inline const std::shared_ptr<const SinglePairPermutation>& operator[](
      size_t i) const {
    if (i >= size()) CoutError("Out-of-range error in TwoTaskPermutations");
    return single_permutations_[i];
  }

  // try to append a pair of job matches to permutation_current, return true
  // if success, false if conflicted
  bool AppendJobs(const JobCEC& job_curr, const JobCEC& job_match,
                  SinglePairPermutation& permutation_current);

  void AppendAllPermutations(const JobCEC& job_curr,
                             SinglePairPermutation& permutation_current);

  void FindAllPermutations();

  inline Edge GetEdge() const { return Edge(task_prev_id_, task_next_id_); }

  void print() const;

  // data members
  TimerType start_time_;
  int task_prev_id_;
  int task_next_id_;
  RegularTaskSystem::TaskSetInfoDerived tasks_info_;
  int superperiod_;
  VariableRange variable_od_range_;
  PermPtrVec single_permutations_;
  std::string type_trait_;
  int perm_count_;       // to count the perm of the whole task set
  int perm_count_base_;  // start from 0, to count the perms of this edge
};

}  // namespace DAG_SPACE