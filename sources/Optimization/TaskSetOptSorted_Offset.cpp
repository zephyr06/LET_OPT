

#include "sources/Optimization/TaskSetOptSorted_Offset.h"
// This class only optimizes offset. The system model is the same as
// Martinez18TCAD
namespace DAG_SPACE {

PermutationInequality GeneratePermIneqOnlyOffset(
    int task_prev_id, int task_next_id, const std::string& type_trait,
    const VariableRange& variable_od_range,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info) {
  PermutationInequality perm_ineq(task_prev_id, task_next_id, type_trait);
  perm_ineq.lower_bound_ =
      variable_od_range.lower_bound.at(task_next_id).offset -
      (variable_od_range.upper_bound.at(task_prev_id).deadline);
  perm_ineq.lower_bound_valid_ = true;
  perm_ineq.upper_bound_ =
      variable_od_range.upper_bound.at(task_next_id).offset -
      (variable_od_range.lower_bound.at(task_prev_id).deadline) + 1;
  perm_ineq.upper_bound_valid_ = true;
  return perm_ineq;
}

std::vector<JobCEC> GetPossibleReadingJobs_MartModel(
    const JobCEC& job_curr, const Task& task_prev, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasksInfo) {
  int job_min_start_time = GetActivationTime(job_curr, tasksInfo);
  int job_max_start_time =
      job_min_start_time + GetPeriod(job_curr, tasksInfo) - 1;

  int period_prev = tasksInfo.GetTask(task_prev.id).period;
  std::vector<JobCEC> readingJobs;
  readingJobs.reserve(superperiod / tasksInfo.GetTask(job_curr.taskId).period);
  for (int i = std::floor(float(job_min_start_time - (task_prev.period - 1)) /
                          period_prev) -
               1;
       i <= std::ceil(float(job_max_start_time) / period_prev); i++) {
    JobCEC job_prev_i(task_prev.id, i);
    int min_finish_prev = GetActivationTime(job_prev_i, tasksInfo) +
                          GetExecutionTime(job_prev_i, tasksInfo);
    if (min_finish_prev > job_max_start_time) continue;
    readingJobs.push_back(job_prev_i);
  }

  return readingJobs;
}

void TwoTaskPermutations_OnlyOffset::AppendAllPermutations(
    const JobCEC& job_curr, SinglePairPermutation& permutation_current) {
  if (ifTimeout(start_time_)) return;
  std::vector<JobCEC> jobs_possible_match = GetPossibleReadingJobs_MartModel(
      job_curr, tasks_info_.GetTask(task_prev_id_), superperiod_, tasks_info_);

  for (auto job_match : jobs_possible_match) {
    PermutationInequality perm_ineq_curr = permutation_current.inequality_;
    bool append_success = permutation_current.AppendJobs(
        job_curr, job_match, tasks_info_, variable_od_range_);
    if (append_success) {
      if (job_curr.jobId ==
          superperiod_ / GetPeriod(job_curr, tasks_info_) -
              1) {  // reach end, record the current permutations
        InsertNewPermSingle(permutation_current);
      } else {
        JobCEC job_next(job_curr.taskId, job_curr.jobId + 1);
        AppendAllPermutations(job_next, permutation_current);
      }
    } else  // skip this match because it's not feasible or useful
      ;
    permutation_current.inequality_ = perm_ineq_curr;
    permutation_current.PopMatchJobPair(job_curr, job_match);
  }
}

void TwoTaskPermutations_OnlyOffset::FindAllPermutations() {
  assert(type_trait_ == "DataAge");
  JobCEC job_curr(task_next_id_, 0);
  PermutationInequality perm_ineq =
      GeneratePermIneqOnlyOffset(task_prev_id_, task_next_id_, type_trait_,
                                 variable_od_range_, tasks_info_);

  SinglePairPermutation single_permutation(perm_ineq, tasks_info_, type_trait_);
  AppendAllPermutations(job_curr, single_permutation);
  // reverse the saving order for faster elimination speed;
  std::reverse(single_permutations_.begin(), single_permutations_.end());
}

// ************************************************************************************

void TaskSetOptSorted_Offset::FindPairPermutations() {
  int single_perm_count = 0;
  for (const auto& edge_curr : graph_of_all_ca_chains_.edge_vec_ordered_) {
    if (ifTimeout(start_time_)) break;
    adjacent_two_task_permutations_.push_back(TwoTaskPermutations_OnlyOffset(
        edge_curr.from_id, edge_curr.to_id, dag_tasks_, tasks_info_, rta_,
        type_trait_, single_perm_count));
    single_perm_count +=
        adjacent_two_task_permutations_.back().single_permutations_.size();
    std::cout << "Pair permutation #: "
              << adjacent_two_task_permutations_.back().size() << "\n";
  }
}

}  // namespace DAG_SPACE