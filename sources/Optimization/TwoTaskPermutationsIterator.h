
#pragma once

#include <memory>

#include "sources/Optimization/TwoTaskPermutations.h"
namespace DAG_SPACE {

class TwoTaskPermutationsIterator : public TwoTaskPermutations {
 public:
  TwoTaskPermutationsIterator() {}
  TwoTaskPermutationsIterator(const TwoTaskPermutations& two_task_perms)
      : TwoTaskPermutations(two_task_perms) {
    for (const auto& ptr : single_permutations_)
      single_perms_ite_record_.push_back(ptr);
    if_harmonic = ifHarmonic();
  }

  inline void Update_InFeasibleFront() {
    single_perms_ite_record_.erase(single_perms_ite_record_.begin());
  }

  template <typename ObjectiveFunctionBase>
  void Update_FeasibleFront() {
    if (if_harmonic)
      single_perms_ite_record_.clear();
    else {
      const auto& perm_ptr_front = single_perms_ite_record_.front();
      for (auto itr = single_perms_ite_record_.begin();
           itr != single_perms_ite_record_.end();) {
        if (CompareSinglePerms(*perm_ptr_front, *(*itr),
                               ObjectiveFunctionBase::type_trait))
          itr = single_perms_ite_record_.erase(itr);
        else
          ++itr;
      }
    }
  }

  inline const std::shared_ptr<const SinglePairPermutation> front() const {
    if (single_perms_ite_record_.size() == 0)
      CoutError("No elements left in TwoTaskPermutationsIterator!");
    return single_perms_ite_record_.front();
  }

  inline bool empty() const { return single_perms_ite_record_.empty(); }

  inline size_t size() const { return single_perms_ite_record_.size(); }

  inline bool ifHarmonic() const {
    int period_prev = tasks_info_.GetTask(task_prev_id_).period;
    int period_next = tasks_info_.GetTask(task_next_id_).period;
    return period_prev % period_next == 0 || period_next % period_prev == 0;
  }

  // data members
  std::list<std::shared_ptr<const SinglePairPermutation>>
      single_perms_ite_record_;
  bool if_harmonic;
};
}  // namespace DAG_SPACE