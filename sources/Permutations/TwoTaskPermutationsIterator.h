
#pragma once

#include <memory>

#include "sources/Permutations/FeasibleChainManager.h"
#include "sources/Permutations/TwoTaskPermutations.h"
namespace DAG_SPACE {

bool IfChainsContainBetterPerm(const ChainsPermutation& chains_perm_partial,
                               const FeasibleChainManager& feasible_chain_man);

bool IfFutureEdgesContainBetterPerm(
    const std::vector<Edge>& unvisited_edges,
    const FeasibleChainManager& feasible_chain_man);

class TwoTaskPermutationsIterator : public TwoTaskPermutations {
   public:
    TwoTaskPermutationsIterator() {}
    // TODO: there is a copy consturctor call, consider adjust it
    TwoTaskPermutationsIterator(const TwoTaskPermutations& two_task_perms)
        : TwoTaskPermutations(two_task_perms) {
        for (const auto& ptr : single_permutations_)
            single_perms_ite_record_.push_back(ptr);
    }

    void RemoveCandidate(const ChainsPermutation& chains_perm_partial,
                         const FeasibleChainManager& feasible_chain_man,
                         const std::vector<Edge>& unvisited_edges);

    void RemoveCandidates(
        const ChainsPermutation& chains_perm_partial,
        const std::vector<FeasibleChainManager>& feasible_chain_man_vec,
        const std::vector<Edge>& unvisited_edges) {
        for (const auto& feasible_chain_man : feasible_chain_man_vec) {
            RemoveCandidate(chains_perm_partial, feasible_chain_man,
                            unvisited_edges);
        }
    }

    void TakeCommonElements(const PermRefSet& per_ptr_set);

    inline const std::shared_ptr<const SinglePairPermutation> pop_front() {
        if (single_perms_ite_record_.size() == 0)
            CoutError("No elements left in TwoTaskPermutationsIterator!");
        auto itr = single_perms_ite_record_.front();
        single_perms_ite_record_.pop_front();
        return itr;
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