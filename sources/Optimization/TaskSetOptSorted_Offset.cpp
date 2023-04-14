

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
      tasks_info.GetTask(task_prev_id).deadline;
  perm_ineq.lower_bound_valid_ = true;
  perm_ineq.upper_bound_ =
      variable_od_range.upper_bound.at(task_next_id).offset -
      tasks_info.GetTask(task_prev_id).deadline + 1;
  perm_ineq.upper_bound_valid_ = true;
  return perm_ineq;
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