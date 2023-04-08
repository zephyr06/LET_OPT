#pragma once
#include "sources/Optimization/Variable.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Interval.h"
#include "sources/Utils/JobCEC.h"
namespace DAG_SPACE {

// lower&upper bound consistency permutation is described by
//      o_{task_next_id} + lower_bound < d_{task_prev_id} ;
//      d_{task_prev_id} <= o_{task_next_id} + upper_bound ;
// This is the same as the following in the paper:
// o_{i+1} + x < d_i < o_{i+1} + y
// Currently, this struct only saves the first-reaction relationship, that
// means, if J_{11} triggers J_{22}, then J_{11} cannot trigger J_{21}
class PermutationInequality {
 public:
  PermutationInequality() {}

  PermutationInequality(int task_prev_id, int task_next_id,
                        const std::string& type_trait)
      : task_prev_id_(task_prev_id),
        task_next_id_(task_next_id),
        lower_bound_(-1e9),
        lower_bound_valid_(false),
        upper_bound_(1e9),
        upper_bound_valid_(false),
        type_trait_(type_trait) {}

  PermutationInequality(int task_prev_id, int task_next_id, int lower_bound,
                        bool lower_bound_valid, int upper_bound,
                        bool upper_bound_valid, const std::string& type_trait)
      : task_prev_id_(task_prev_id),
        task_next_id_(task_next_id),
        lower_bound_(lower_bound),
        lower_bound_valid_(lower_bound_valid),
        upper_bound_(upper_bound),
        upper_bound_valid_(upper_bound_valid),
        type_trait_(type_trait) {}

  // job_curr -> job_match
  PermutationInequality(const JobCEC& job_curr, const JobCEC& job_match,
                        const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
                        const std::string& type_trait)
      : task_prev_id_(job_curr.taskId),
        task_next_id_(job_match.taskId),
        lower_bound_(1e9),
        lower_bound_valid_(true),
        upper_bound_(1e9),
        upper_bound_valid_(true),
        type_trait_(type_trait) {
    if (type_trait == "ReactionTimeApprox" || type_trait == "ReactionTime") {
      upper_bound_ = GetActivationTime(job_match, tasks_info) -
                     GetActivationTime(job_curr, tasks_info);
      JobCEC job_match_prev_job(job_match.taskId, job_match.jobId - 1);
      lower_bound_ = GetActivationTime(job_match_prev_job, tasks_info) -
                     GetActivationTime(job_curr, tasks_info);
    } else if (type_trait == "DataAgeApprox" || type_trait == "DataAge") {
      lower_bound_ = GetActivationTime(job_curr, tasks_info) -
                     GetActivationTime(job_match, tasks_info);
      JobCEC job_curr_next_job(job_curr.taskId, job_curr.jobId + 1);
      upper_bound_ = GetActivationTime(job_curr_next_job, tasks_info) -
                     GetActivationTime(job_match, tasks_info);
    } else
      CoutError("Type trait not found in PermIneq!");
  }

  void print() const {
    if (type_trait_.find("ReactionTime") != std::string::npos)
      std::cout << "o_{" << task_next_id_ << "} + " << lower_bound_ + 1
                << " <= "
                << "d_{" << task_prev_id_ << "} <= o_{" << task_next_id_
                << "} + " << upper_bound_ << "\n";
    else if (type_trait_.find("DataAge") != std::string::npos)
      std::cout << "d_{" << task_prev_id_ << "} + " << lower_bound_ << " <= "
                << "o_{" << task_next_id_ << "} <= d_{" << task_prev_id_
                << "} + " << upper_bound_ + 1 << "\n";
  }

  inline bool operator==(const PermutationInequality& other) const {
    return lower_bound_valid_ == other.lower_bound_valid_ &&
           upper_bound_valid_ == other.upper_bound_valid_ &&
           task_prev_id_ == other.task_prev_id_ &&
           task_next_id_ == other.task_next_id_ &&
           lower_bound_ == other.lower_bound_ &&
           upper_bound_ == other.upper_bound_ &&
           type_trait_ == other.type_trait_;
  }
  bool operator!=(const PermutationInequality& other) const {
    return !((*this) == other);
  }

  inline void setInvalid() {
    lower_bound_valid_ = true;
    upper_bound_valid_ = true;
    lower_bound_ = upper_bound_;
  }

  inline bool IsValid() const {
    if (lower_bound_valid_)
      if (upper_bound_valid_)
        if (lower_bound_ >= upper_bound_) return false;

    return true;
  }

  inline Interval GetInterval() const {
    int lower_bound = -1e9, upper_bound = 1e9;
    if (lower_bound_valid_) lower_bound = lower_bound_;
    if (upper_bound_valid_) upper_bound = upper_bound_;
    return Interval(lower_bound, upper_bound - lower_bound);
  }

  // data member
  int task_prev_id_;
  int task_next_id_;
  int lower_bound_;
  bool lower_bound_valid_;
  int upper_bound_;
  bool upper_bound_valid_;
  std::string type_trait_;
};

/**
 * @brief
 *
 * @param perm1
 * @param perm2
 * @return true: perm1 and perm2 have confliction
 * @return false: no confliction
 */
bool ExamConfliction(const PermutationInequality& perm1,
                     const PermutationInequality& perm2);

int MergeSmallerThanValue(const PermutationInequality& perm1,
                          const PermutationInequality& perm2);

int MergeLargerThanValue(const PermutationInequality& perm1,
                         const PermutationInequality& perm2);

PermutationInequality MergeTwoSinglePermutations(
    const PermutationInequality& perm1, const PermutationInequality& perm2);

PermutationInequality GenerateBoxPermutationConstraints(
    int task_prev_id, int task_next_id, const VariableRange& variable_range,
    const std::string& type_trait);
}  // namespace DAG_SPACE