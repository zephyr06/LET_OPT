
#pragma once

#include <memory>

#include "sources/Optimization/Variable.h"
#include "sources/Permutations/PermutationInequality.h"
#include "sources/Utils/profilier.h"
namespace DAG_SPACE {

inline int GetSuperPeriod(const Task& task1, const Task& task2) {
  return std::lcm(task1.period, task2.period);
}

struct SinglePairPermutation {
  SinglePairPermutation() {}

  SinglePairPermutation(
      int task_prev_id, int task_next_id,
      std::unordered_map<JobCEC, JobCEC> job_first_react_matches,
      const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
      const std::string& type_trait);

  // constructors for the convenience of iteration in TwoTaskPermutations
  SinglePairPermutation(PermutationInequality inequality,
                        const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
                        const std::string& type_trait);

  // constructors for the convenience of iteration in TwoTaskPermutations
  SinglePairPermutation(
      PermutationInequality inequality,
      std::unordered_map<JobCEC, JobCEC> job_first_react_matches,
      const std::string& type_trait);

  // copy constructor
  SinglePairPermutation(const SinglePairPermutation& other);

  // try to append a pair of job matches to permutation_current, return true
  // if success, false if conflicted
  bool AppendJobs(const JobCEC& job_curr, const JobCEC& job_match,
                  const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
                  const VariableRange& variable_od_range);

  // job_curr -> job_match
  bool AddMatchJobPair(const JobCEC& job_curr, const JobCEC& job_match);
  // job_curr -> job_match
  void PopMatchJobPair(const JobCEC& job_curr, const JobCEC& job_match);

  void print() const;

  inline int GetPrevTaskId() const { return inequality_.task_prev_id_; }
  inline int GetNextTaskId() const { return inequality_.task_next_id_; }

  inline bool operator==(const SinglePairPermutation& other) const {
    return inequality_ == other.inequality_ &&
           job_first_react_matches_ == other.job_first_react_matches_;
  }

  inline bool operator!=(const SinglePairPermutation& other) const {
    return !(*this == other);
  }

  // data members
  PermutationInequality
      inequality_;  //  for convenience of skipping permutations
  std::unordered_map<JobCEC, JobCEC> job_first_react_matches_;
  int index_global_;
  int index_local_;
  std::string type_trait_;
};

typedef std::shared_ptr<const SinglePairPermutation> PermPtr;

// return true if perm_another performs worse than perm_base
bool IfSkipAnotherPermRT(const SinglePairPermutation& perm_base,
                         const SinglePairPermutation& perm_another);

bool IfSkipAnotherPerm(const SinglePairPermutation& perm_base,
                       const SinglePairPermutation& perm_another,
                       const std::string& obj_trait);

}  // namespace DAG_SPACE

template <>
struct std::hash<DAG_SPACE::SinglePairPermutation> {
  std::size_t operator()(
      const DAG_SPACE::SinglePairPermutation& perm_single) const {
    if (perm_single.index_global_ >= 0)
      return perm_single.index_global_;
    else
      CoutError(
          "Please initialize the index_global_ in SinglePairPermutation for "
          "the usage of hash!");
    std::hash<std::string> hasher;
    const DAG_SPACE::PermutationInequality& ineq = perm_single.inequality_;
    return hasher(std::to_string(ineq.task_prev_id_) + "," +
                  std::to_string(ineq.task_next_id_) + "," +
                  std::to_string(ineq.lower_bound_) + "," +
                  std::to_string(ineq.upper_bound_));
  }
};
