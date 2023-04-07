
#include "sources/Permutations/TwoTaskPermutations.h"

namespace DAG_SPACE {
std::vector<JobCEC> GetPossibleReactingJobs(
    const JobCEC& job_curr, const Task& task_next, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasksInfo) {
  int job_min_finish = GetActivationTime(job_curr, tasksInfo) +
                       GetExecutionTime(job_curr, tasksInfo);
  int job_max_finish = GetDeadline(job_curr, tasksInfo);

  int period_next = tasksInfo.GetTask(task_next.id).period;
  std::vector<JobCEC> reactingJobs;
  reactingJobs.reserve(superperiod / tasksInfo.GetTask(job_curr.taskId).period);
  for (int i = std::floor(float(job_min_finish) / period_next);
       i <= std::ceil(float(job_max_finish) / period_next); i++) {
    JobCEC job_next_i(task_next.id, i);
    int max_start_next = GetDeadline(job_next_i, tasksInfo) -
                         GetExecutionTime(job_next_i, tasksInfo);
    if (max_start_next < job_min_finish) continue;
    reactingJobs.push_back(job_next_i);
  }

  return reactingJobs;
}

std::vector<JobCEC> GetPossibleReadingJobs(
    const JobCEC& job_curr, const Task& task_prev, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasksInfo) {
  int job_min_start = GetActivationTime(job_curr, tasksInfo);
  int job_max_start =
      GetDeadline(job_curr, tasksInfo) - GetExecutionTime(job_curr, tasksInfo);

  int period_prev = tasksInfo.GetTask(task_prev.id).period;
  std::vector<JobCEC> readingJobs;
  readingJobs.reserve(superperiod / tasksInfo.GetTask(job_curr.taskId).period);
  for (int i = std::floor(float(job_min_start) / period_prev) - 1;
       i <= std::ceil(float(job_max_start) / period_prev); i++) {
    JobCEC job_prev_i(task_prev.id, i);
    int min_finish_prev = GetActivationTime(job_prev_i, tasksInfo) +
                          GetExecutionTime(job_prev_i, tasksInfo);
    if (min_finish_prev > job_max_start) continue;
    readingJobs.push_back(job_prev_i);
  }

  return readingJobs;
}

PermutationInequality GenerateBoxPermutationConstraints(
    int task_prev_id, int task_next_id, const VariableRange& variable_range,
    const std::string& type_trait) {
  return PermutationInequality(
      task_prev_id, task_next_id,
      variable_range.lower_bound.at(task_prev_id).deadline -
          variable_range.upper_bound.at(task_next_id).offset,
      true,
      variable_range.upper_bound.at(task_prev_id).deadline -
          variable_range.lower_bound.at(task_next_id).offset,
      true, type_trait);
}

bool ifTimeout(TimerType start_time) {
  auto curr_time = std::chrono::system_clock::now();
  if (std::chrono::duration_cast<std::chrono::seconds>(curr_time - start_time)
          .count() >= GlobalVariablesDAGOpt::TIME_LIMIT) {
    std::cout << "\nTime out when running OptimizeOrder. Maximum time is "
              << GlobalVariablesDAGOpt::TIME_LIMIT << " seconds.\n\n";
    return true;
  }
  return false;
}

bool TwoTaskPermutations::AppendJobs(
    const JobCEC& job_curr, const JobCEC& job_match,
    SinglePairPermutation& permutation_current) {
  PermutationInequality perm_new(job_curr, job_match, tasks_info_, type_trait_);
  PermutationInequality perm_merged =
      MergeTwoSinglePermutations(perm_new, permutation_current.inequality_);
  if (perm_merged.IsValid()) {
    // Add bound constraints
    PermutationInequality perm_bound = GenerateBoxPermutationConstraints(
        job_curr.taskId, job_match.taskId, variable_od_range_, type_trait_);
    perm_merged = MergeTwoSinglePermutations(perm_merged, perm_bound);
  }

  if (perm_merged.IsValid()) {
    permutation_current.inequality_ = perm_merged;
    if (!(permutation_current.AddMatchJobPair(job_curr, job_match)))
      return false;
    return true;
  } else
    return false;
}

void TwoTaskPermutations::AppendAllPermutations(
    const JobCEC& job_curr, SinglePairPermutation& permutation_current) {
  if (ifTimeout(start_time_)) return;
  std::vector<JobCEC> jobs_possible_match = GetPossibleReactingJobs(
      job_curr, tasks_info_.GetTask(task_next_id_), superperiod_, tasks_info_);

  for (auto job_match : jobs_possible_match) {
    PermutationInequality perm_ineq_curr = permutation_current.inequality_;
    SinglePairPermutation& permutation_current_copy = permutation_current;
    if (AppendJobs(job_curr, job_match, permutation_current_copy)) {
      if (job_curr.jobId ==
          superperiod_ / GetPeriod(job_curr, tasks_info_) -
              1) {  // reach end, record the current permutations

        // if (single_permutations_.size() > 20 &&
        //     RandRange(0, 1) > GlobalVariablesDAGOpt::SAMPLE_SMALL_TASKS) {
        //   continue;
        // } else {
        permutation_current_copy.index_global_ = perm_count_++;
        permutation_current_copy.index_local_ = perm_count_base_++;
        if (perm_count_ > 1e4) {
          CoutError("Too large perm_count!");
        }
        single_permutations_.push_back(
            std::make_shared<const SinglePairPermutation>(
                permutation_current_copy));
        // }

        if (single_permutations_.size() > 1e5)
          CoutError("Possibly too many permutations!");
      } else {
        JobCEC job_next(job_curr.taskId, job_curr.jobId + 1);
        AppendAllPermutations(job_next, permutation_current_copy);
      }
    } else  // skip this match because it's not feasible or useful
      ;
    permutation_current_copy.inequality_ = perm_ineq_curr;
    permutation_current_copy.PopMatchJobPair(job_curr, job_match);
  }
}

void TwoTaskPermutations::FindAllPermutations() {
  if (type_trait_ == "ReactionTime" || type_trait_ == "ReactionTimeApprox") {
    JobCEC job_curr(task_prev_id_, 0);
    PermutationInequality perm_ineq(task_prev_id_, task_next_id_, type_trait_);
    SinglePairPermutation single_permutation(perm_ineq, tasks_info_,
                                             type_trait_);
    AppendAllPermutations(job_curr, single_permutation);
  } else
    CoutError("Unrecognized type_trait_!");
}
void TwoTaskPermutations::print() const {
  for (uint i = 0; i < single_permutations_.size(); i++) {
    single_permutations_[i]->print();
    std::cout << "\n";
  }
}
}  // namespace DAG_SPACE