
#include "sources/Permutations/SinglePairPermutation.h"

namespace DAG_SPACE {

SinglePairPermutation::SinglePairPermutation(
    int task_prev_id, int task_next_id,
    std::unordered_map<JobCEC, std::vector<JobCEC>> job_first_react_matches,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info)
    : inequality_(PermutationInequality(task_prev_id, task_next_id)),
      job_first_react_matches_(job_first_react_matches) {
  for (const auto& [job_source, job_matches] : job_first_react_matches_) {
    PermutationInequality perm_new(job_source, job_matches[0], tasks_info);
    PermutationInequality perm_merged =
        MergeTwoSinglePermutations(perm_new, inequality_);
    if (perm_merged.IsValid())
      inequality_ = perm_merged;
    else
      CoutError(
          "Conflicted matched jobs in SinglePairPermutation "
          "constructor!");
  }
}

// constructors for the convenience of iteration in TwoTaskPermutations
SinglePairPermutation::SinglePairPermutation(
    PermutationInequality inequality,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info)
    : inequality_(inequality) {
  int superperiod =
      GetSuperPeriod(tasks_info.GetTask(inequality.task_prev_id_),
                     tasks_info.GetTask(inequality.task_next_id_));
  job_first_react_matches_.reserve(
      superperiod / tasks_info.GetTask(inequality.task_prev_id_).period);
}

// constructors for the convenience of iteration in TwoTaskPermutations
SinglePairPermutation::SinglePairPermutation(
    PermutationInequality inequality,
    std::unordered_map<JobCEC, std::vector<JobCEC>> job_first_react_matches)
    : inequality_(inequality),
      job_first_react_matches_(job_first_react_matches) {}

// copy constructor
SinglePairPermutation::SinglePairPermutation(
    const SinglePairPermutation& other) {
  BeginTimer("SinglePairPermutation_copy");
  inequality_ = other.inequality_;
  job_first_react_matches_ = other.job_first_react_matches_;
  EndTimer("SinglePairPermutation_copy");
}

void SinglePairPermutation::print() const {
  inequality_.print();
  for (const auto& [key, value] : job_first_react_matches_) {
    std::cout << key.ToString() << "'s following jobs are ";
    for (auto job_next : value) std::cout << job_next.ToString() << ", ";
    std::cout << "\n";
  }
  std::cout << "\n";
}

bool SinglePairPermutation::AddMatchJobPair(const JobCEC& job_curr,
                                            const JobCEC& job_match) {
  auto itr = job_first_react_matches_.find(job_curr);
  if (itr == job_first_react_matches_.end())
    job_first_react_matches_[job_curr] = {job_match};
  else {
    JobCEC last_matched_job = itr->second.back();
    if (job_match.jobId < last_matched_job.jobId) return false;
    if (job_first_react_matches_[job_curr].size() > 0 &&
        job_first_react_matches_[job_curr].back().jobId > job_match.jobId)
      CoutError("Wrong order in AddMatchJobPair!");
    job_first_react_matches_[job_curr].push_back(job_match);
  }

  return true;
}

void SinglePairPermutation::PopMatchJobPair(const JobCEC& job_curr,
                                            const JobCEC& job_match) {
  auto itr = job_first_react_matches_.find(job_curr);
  if (itr == job_first_react_matches_.end())
    return;
  else {
    if (itr->second.size() > 0) {
      JobCEC last_matched_job = itr->second.back();
      if (job_match.jobId == last_matched_job.jobId) {
        itr->second.pop_back();
        if (itr->second.size() == 0) job_first_react_matches_.erase(job_curr);
      }
    }
  }
}
// if perm1 is feasible, return true if perm2 can be safely skipped
bool CompareSinglePerms(const SinglePairPermutation& perm1,
                        const SinglePairPermutation& perm2,
                        const std::string& obj_trait) {
  if (obj_trait == "ReactionTime" || obj_trait == "ReactionTimeApprox")
    return CompareSinglePerMRT(perm1, perm2);
  else
    CoutError("Not implemented for obj_trait: " + obj_trait);
  return false;
}

bool CompareSinglePerMRT(const SinglePairPermutation& perm1,
                         const SinglePairPermutation& perm2) {
  ASSERT(perm1.GetPrevTaskId() == perm2.GetPrevTaskId() &&
             perm1.GetNextTaskId() == perm2.GetNextTaskId(),
         "perm1 and perm2 should be about the same tasks");
  for (auto itr = perm1.job_first_react_matches_.begin();
       itr != perm1.job_first_react_matches_.end(); itr++) {
    const JobCEC& job_curr = itr->first;
    auto itr2 = perm2.job_first_react_matches_.find(job_curr);
    ASSERT(itr2 != perm2.job_first_react_matches_.end(),
           "perm1 and perm2 should have the same jobs");
    if (itr->second[0].jobId > itr2->second[0].jobId) return false;
  }
  return true;
}
}  // namespace DAG_SPACE