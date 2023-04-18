
#include "sources/Permutations/SinglePairPermutation.h"

#include "sources/ObjectiveFunction/ObjTraitEval.h"
namespace DAG_SPACE {

SinglePairPermutation::SinglePairPermutation(
    int task_prev_id, int task_next_id,
    std::unordered_map<JobCEC, JobCEC> job_first_react_matches,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
    const std::string& type_trait)
    : inequality_(
          PermutationInequality(task_prev_id, task_next_id, type_trait)),
      job_matches_(job_first_react_matches),
      index_global_(-1),
      type_trait_(type_trait) {
  for (const auto& [job_source, job_match] : job_matches_) {
    PermutationInequality perm_new(job_source, job_match, tasks_info,
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
  job_matches_.reserve(superperiod /
                       tasks_info.GetTask(inequality.task_prev_id_).period);
}

// constructors for the convenience of iteration in TwoTaskPermutations
SinglePairPermutation::SinglePairPermutation(
    PermutationInequality inequality,
    std::unordered_map<JobCEC, JobCEC> job_first_react_matches,
    const std::string& type_trait)
    : inequality_(inequality),
      job_matches_(job_first_react_matches),
      type_trait_(type_trait) {}

// copy constructor
SinglePairPermutation::SinglePairPermutation(
    const SinglePairPermutation& other) {
  BeginTimer("SinglePairPermutation_copy");
  inequality_ = other.inequality_;
  job_matches_ = other.job_matches_;
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
  for (const auto& [key, job_next] : job_matches_) {
    std::cout << key.ToString();
    if (type_trait_.find("ReactionTime") != std::string::npos)
      std::cout << "'s following jobs are ";
    else if (type_trait_.find("DataAge") != std::string::npos)
      std::cout << "'s reading jobs are ";
    else
      CoutError("Unrecognized type_trait!");
    std::cout << job_next.ToString() << ", ";
    std::cout << "\n";
  }
  std::cout << "\n";
}

bool SinglePairPermutation::AppendJobs(
    const JobCEC& job_curr, const JobCEC& job_match,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
    const VariableRange& variable_od_range) {
  PermutationInequality perm_new(job_curr, job_match, tasks_info, type_trait_);
  PermutationInequality perm_merged =
      MergeTwoSinglePermutations(perm_new, inequality_);
  if (perm_merged.IsValid()) {
    // Add bound constraints // TODO_Minor: move it to constructor
    PermutationInequality perm_bound = GenerateBoxPermutationConstraints(
        job_curr.taskId, job_match.taskId, variable_od_range, type_trait_);
    perm_merged = MergeTwoSinglePermutations(perm_merged, perm_bound);
  }

  if (perm_merged.IsValid()) {
    inequality_ = perm_merged;
    return AddMatchJobPair(job_curr, job_match);
  } else
    return false;
}

bool SinglePairPermutation::AddMatchJobPair(const JobCEC& job_curr,
                                            const JobCEC& job_match) {
  auto itr = job_matches_.find(job_curr);
  if (itr == job_matches_.end())
    job_matches_[job_curr] = job_match;
  else {
    JobCEC last_matched_job = itr->second;
    if (job_match.jobId < last_matched_job.jobId) return false;
    if (job_matches_[job_curr].jobId > job_match.jobId)
      CoutError("Wrong order in AddMatchJobPair!");
    job_matches_[job_curr] = job_match;
  }

  return true;
}

void SinglePairPermutation::PopMatchJobPair(const JobCEC& job_curr,
                                            const JobCEC& job_match) {
  auto itr = job_matches_.find(job_curr);
  if (itr == job_matches_.end())
    return;
  else {
    JobCEC last_matched_job = itr->second;
    if (job_match.jobId == last_matched_job.jobId) {
      // itr->second.pop_back();
      // if (itr->second.size() == 0)
      job_matches_.erase(job_curr);
    }
  }
}

// return true if perm_another can be safely skipped
bool IfSkipAnotherPerm(const SinglePairPermutation& perm_base,
                       const SinglePairPermutation& perm_another,
                       const std::string& obj_trait) {
  // If the obj is "ReactionTime"/"DataAge", then it cannot always be be skipped
  // safely, though it should happen not frequently
  // however, in experiments, the Sorted method could only optimize the
  // approximated RT/DA, therefore, we assume that the objective function is
  // always the approximated RT/DA if the *Sorted* method is used
  // Correction again: we added *Approx back for the convenience of
  // PerformTOM_OPT_SortBound
  if (IfRT_Trait(obj_trait))
    return IfSkipAnotherPermRT(perm_base, perm_another);
  else if (IfRT_Trait(obj_trait))
    return IfSkipAnotherPermDA(perm_base, perm_another);
  else
    return false;
  // CoutError("Not implemented for obj_trait: " + obj_trait);
  return false;
}

bool IfSkipAnotherPermRT(const SinglePairPermutation& perm_base,
                         const SinglePairPermutation& perm_another) {
  ASSERT(perm_base.GetPrevTaskId() == perm_another.GetPrevTaskId() &&
             perm_base.GetNextTaskId() == perm_another.GetNextTaskId(),
         "perm_base and perm_another should be about the same tasks");
  for (auto itr = perm_base.job_matches_.begin();
       itr != perm_base.job_matches_.end(); itr++) {
    const JobCEC& job_curr = itr->first;
    auto itr2 = perm_another.job_matches_.find(job_curr);
    ASSERT(itr2 != perm_another.job_matches_.end(),
           "perm_base and perm_another should have the same jobs");
    if (itr->second.jobId > itr2->second.jobId) return false;
  }
  return true;
}

// return true if perm_another can be safely skipped
bool IfSkipAnotherPermDA(const SinglePairPermutation& perm_base,
                         const SinglePairPermutation& perm_another) {
  ASSERT(perm_base.GetPrevTaskId() == perm_another.GetPrevTaskId() &&
             perm_base.GetNextTaskId() == perm_another.GetNextTaskId(),
         "perm_base and perm_another should be about the same tasks");
  for (auto itr_base = perm_base.job_matches_.begin();
       itr_base != perm_base.job_matches_.end(); itr_base++) {
    const JobCEC& job_curr = itr_base->first;
    auto itr_ano = perm_another.job_matches_.find(job_curr);
    ASSERT(itr_ano != perm_another.job_matches_.end(),
           "perm_base and perm_another should have the same jobs");
    if (itr_base->second.jobId < itr_ano->second.jobId) return false;
  }
  return true;
}
}  // namespace DAG_SPACE