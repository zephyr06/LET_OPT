
#include "sources/Permutations/PermutationInequality.h"

namespace DAG_SPACE {

bool ExamConfliction(const PermutationInequality& perm1,
                     const PermutationInequality& perm2) {
  Interval interval1 = perm1.GetInterval();
  Interval interval2 = perm2.GetInterval();
  if (Overlap(interval1, interval2) > 0)
    return false;
  else
    return true;
}

int MergeSmallerThanValue(const PermutationInequality& perm1,
                          const PermutationInequality& perm2) {
  if (perm1.lower_bound_valid_ && perm2.lower_bound_valid_)
    return std::max(perm1.lower_bound_, perm2.lower_bound_);
  else if (perm1.lower_bound_valid_)
    return perm1.lower_bound_;
  else if (perm2.lower_bound_valid_)
    return perm2.lower_bound_;
  else
    return -1e9;
}

int MergeLargerThanValue(const PermutationInequality& perm1,
                         const PermutationInequality& perm2) {
  if (perm1.upper_bound_valid_ && perm2.upper_bound_valid_)
    return std::min(perm1.upper_bound_, perm2.upper_bound_);
  else if (perm1.upper_bound_valid_)
    return perm1.upper_bound_;
  else if (perm2.upper_bound_valid_)
    return perm2.upper_bound_;
  else
    return 1e9;
}

PermutationInequality MergeTwoSinglePermutations(
    const PermutationInequality& perm1, const PermutationInequality& perm2) {
  ASSERT(perm1.type_trait_ == perm2.type_trait_,
         "perm type must be the same in MergeTwoSinglePermutations");
  PermutationInequality merged_perm;

  if (ExamConfliction(perm1, perm2) ||
      perm1.task_prev_id_ != perm2.task_prev_id_ ||
      perm1.task_next_id_ != perm2.task_next_id_) {
    merged_perm.setInvalid();
  } else {
    int smaller_value_merged = MergeSmallerThanValue(perm1, perm2);
    int larger_value_merged = MergeLargerThanValue(perm1, perm2);

    merged_perm = PermutationInequality(
        perm1.task_prev_id_, perm1.task_next_id_, smaller_value_merged,
        perm1.lower_bound_valid_ || perm2.lower_bound_valid_,
        larger_value_merged,
        perm1.upper_bound_valid_ || perm2.upper_bound_valid_,
        perm1.type_trait_);
  }
  return merged_perm;
}

PermutationInequality GenerateBoxPermutationConstraints(
    int task_curr_id, int task_match_id, const VariableRange& variable_range,
    const std::string& type_trait) {
  if (type_trait == "ReactionTime" || type_trait == "ReactionTimeApprox") {
    int task_prev_id = task_curr_id;
    int task_next_id = task_match_id;
    return PermutationInequality(
        task_prev_id, task_next_id,
        variable_range.lower_bound.at(task_prev_id).deadline -
            variable_range.upper_bound.at(task_next_id).offset - 1,
        true,
        variable_range.upper_bound.at(task_prev_id).deadline -
            variable_range.lower_bound.at(task_next_id).offset,
        true, type_trait);
  } else if (type_trait == "DataAge" || type_trait == "DataAgeApprox") {
    int task_prev_id = task_match_id;
    int task_next_id = task_curr_id;
    return PermutationInequality(
        task_prev_id, task_next_id,
        variable_range.lower_bound.at(task_next_id).offset -
            variable_range.upper_bound.at(task_prev_id).deadline,
        true,
        variable_range.upper_bound.at(task_next_id).offset -
            variable_range.lower_bound.at(task_prev_id).deadline + 1,
        true, type_trait);
  } else
    CoutError("Unknown type trait in BatchOptimize!");
  return PermutationInequality();
}

}  // namespace DAG_SPACE