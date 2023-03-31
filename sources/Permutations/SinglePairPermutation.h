
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
      std::unordered_map<JobCEC, std::vector<JobCEC>> job_first_react_matches,
      const RegularTaskSystem::TaskSetInfoDerived& tasks_info);

  // constructors for the convenience of iteration in TwoTaskPermutations
  SinglePairPermutation(
      PermutationInequality inequality,
      const RegularTaskSystem::TaskSetInfoDerived& tasks_info);

  // constructors for the convenience of iteration in TwoTaskPermutations
  SinglePairPermutation(
      PermutationInequality inequality,
      std::unordered_map<JobCEC, std::vector<JobCEC>> job_first_react_matches);

  // copy constructor
  SinglePairPermutation(const SinglePairPermutation& other);

  bool AddMatchJobPair(const JobCEC& job_curr, const JobCEC& job_match);
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
  std::unordered_map<JobCEC, std::vector<JobCEC>>
      job_first_react_matches_;  // TODO: there should be only one job in the
                                 // vector, make it happen
};

typedef std::shared_ptr<const SinglePairPermutation> PermPtr;

// return true if perm_another performs worse than perm_base
bool IfSkipAnotherPermRT(const SinglePairPermutation& perm_base,
                         const SinglePairPermutation& perm_another);

bool IfSkipAnotherPerm(const SinglePairPermutation& perm_base,
                       const SinglePairPermutation& perm_another,
                       const std::string& obj_trait);

}  // namespace DAG_SPACE