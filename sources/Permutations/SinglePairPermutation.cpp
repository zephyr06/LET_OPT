
#include "sources/Permutations/SinglePairPermutation.h"

namespace DAG_SPACE {

SinglePairPermutation::SinglePairPermutation(
    int task_prev_id, int task_next_id,
    std::unordered_map<JobCEC, std::vector<JobCEC>> job_first_react_matches,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
    const std::string& type_trait)
    : inequality_(
          PermutationInequality(task_prev_id, task_next_id, type_trait)),
      job_first_react_matches_(job_first_react_matches),
      index_global_(-1),
      type_trait_(type_trait) {
  for (const auto& [job_source, job_matches] : job_first_react_matches_) {
    PermutationInequality perm_new(job_source, job_matches[0], tasks_info,
                                   "ReactionTime");
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
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
    const std::string& type_trait)
    : inequality_(inequality), type_trait_(type_trait) {
  int superperiod =
      GetSuperPeriod(tasks_info.GetTask(inequality.task_prev_id_),
                     tasks_info.GetTask(inequality.task_next_id_));
  job_first_react_matches_.reserve(
      superperiod / tasks_info.GetTask(inequality.task_prev_id_).period);
}

// constructors for the convenience of iteration in TwoTaskPermutations
SinglePairPermutation::SinglePairPermutation(
    PermutationInequality inequality,
    std::unordered_map<JobCEC, std::vector<JobCEC>> job_first_react_matches,
    const std::string& type_trait)
    : inequality_(inequality),
      job_first_react_matches_(job_first_react_matches),
      type_trait_(type_trait) {}

// copy constructor
SinglePairPermutation::SinglePairPermutation(
    const SinglePairPermutation& other) {
  BeginTimer("SinglePairPermutation_copy");
  inequality_ = other.inequality_;
  job_first_react_matches_ = other.job_first_react_matches_;
  index_global_ = other.index_global_;
  index_local_ = other.index_local_;
  type_trait_ = other.type_trait_;
  EndTimer("SinglePairPermutation_copy");
}

void SinglePairPermutation::print() const {
  if (index_global_ >= 0)
    std::cout << "Global index of the SinglePairPermutation is "
              << index_global_ << "\n";
  if (index_local_ >= 0)
    std::cout << "Local index of the SinglePairPermutation is " << index_local_
              << "\n";
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

// return true if perm_another can be safely skipped
bool IfSkipAnotherPerm(const SinglePairPermutation& perm_base,
                       const SinglePairPermutation& perm_another,
                       const std::string& obj_trait) {
  // TODO: remove "ReactionTime" because it cannot be skipped safely?
  if (obj_trait == "ReactionTime" || obj_trait == "ReactionTimeApprox")
    return IfSkipAnotherPermRT(perm_base, perm_another);
  // TODO: check the following, seek theoretical verification
  else if (obj_trait == "DataAge" || obj_trait == "DataAgeApprox")
    return IfSkipAnotherPermRT(perm_base, perm_another);
  else
    CoutError("Not implemented for obj_trait: " + obj_trait);
  return false;
}

bool IfSkipAnotherPermRT(const SinglePairPermutation& perm_base,
                         const SinglePairPermutation& perm_another) {
  ASSERT(perm_base.GetPrevTaskId() == perm_another.GetPrevTaskId() &&
             perm_base.GetNextTaskId() == perm_another.GetNextTaskId(),
         "perm_base and perm_another should be about the same tasks");
  for (auto itr = perm_base.job_first_react_matches_.begin();
       itr != perm_base.job_first_react_matches_.end(); itr++) {
    const JobCEC& job_curr = itr->first;
    auto itr2 = perm_another.job_first_react_matches_.find(job_curr);
    ASSERT(itr2 != perm_another.job_first_react_matches_.end(),
           "perm_base and perm_another should have the same jobs");
    if (itr->second[0].jobId > itr2->second[0].jobId) return false;
  }
  return true;
}
}  // namespace DAG_SPACE