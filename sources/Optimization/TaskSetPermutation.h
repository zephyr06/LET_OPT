#pragma once

#include "sources/Optimization/ChainPermutation.h"
#include "sources/Optimization/ObjectiveFunction.h"
namespace DAG_SPACE {

// assume the simple response time analysis
VariableOD FindODFromPermutation(const TaskSetInfoDerived& tasks_info,
                                 const ChainPermutation& chain_perm,
                                 std::vector<int> task_chain);

// currently, as asusme there is only one chain
class TaskSetPermutation {
   public:
    TaskSetPermutation() {}
    TaskSetPermutation(const DAG_Model& dag_tasks,
                       const std::vector<int>& chain)
        : dag_tasks_(dag_tasks),
          tasks_info_(RegularTaskSystem::TaskSetInfoDerived(dag_tasks.tasks)),
          task_chain_(chain),
          best_yet_obj_(1e9),
          iteration_count_(0) {
        adjacent_two_task_permutations_.reserve(chain.size() - 1);
        FindPairPermutations();
        chain_permutations_.reserve(1e5);
    }

    void FindPairPermutations() {
        for (uint i = 0; i < task_chain_.size() - 1; i++) {
            adjacent_two_task_permutations_.push_back(TwoTaskPermutations(
                task_chain_[i], task_chain_[i + 1], dag_tasks_, tasks_info_));
        }
    }

    int PerformOptimization() {
        ChainPermutation chain_perm;
        IterateAllChainPermutations(0, chain_perm);
        return best_yet_obj_;
    }

    void IterateAllChainPermutations(uint position,
                                     ChainPermutation& chain_perm) {
        if (position == task_chain_.size() - 1) {
            iteration_count_++;
            EvaluateChainPermutation(chain_perm);
            return;
        }

        for (uint i = 0; i < adjacent_two_task_permutations_[position].size();
             i++) {
            // TODO: consider improving efficiency
            chain_perm.push_back(adjacent_two_task_permutations_[position][i]);
            IterateAllChainPermutations(position + 1, chain_perm);
            chain_perm.pop_back();
        }
    }

    void EvaluateChainPermutation(const ChainPermutation& chain_perm) {
        // evaluate schedulability
        VariableOD variable_od =
            FindODFromPermutation(tasks_info_, chain_perm, task_chain_);
        if (!variable_od.valid_)
            return;
        else {
            double obj_curr = ObjReactionTime::Obj(dag_tasks_, tasks_info_,
                                                   chain_perm, variable_od);
            if (obj_curr < best_yet_obj_) {
                best_yet_obj_ = obj_curr;
                best_yet_chain_ = chain_perm;
            }
        }

        // chain_permutations_.push_back(chain_perm);
    }

    // data members
    DAG_Model dag_tasks_;
    RegularTaskSystem::TaskSetInfoDerived tasks_info_;
    std::vector<int> task_chain_;
    std::vector<TwoTaskPermutations> adjacent_two_task_permutations_;
    std::vector<ChainPermutation> chain_permutations_;
    ChainPermutation best_yet_chain_;
    int best_yet_obj_;
    int iteration_count_;
};

}  // namespace DAG_SPACE