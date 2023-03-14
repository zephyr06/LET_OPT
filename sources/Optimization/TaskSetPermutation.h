#pragma once

#include "sources/Baseline/StandardLET.h"
#include "sources/Optimization/ChainPermutation.h"
#include "sources/Optimization/ObjectiveFunction.h"
#include "sources/Utils/BatchUtils.h"
#include "sources/Utils/profilier.h"
namespace DAG_SPACE {

// assume the simple response time analysis
VariableOD FindODFromPermutation(const DAG_Model& dag_tasks,
                                 const ChainPermutation& chain_perm,
                                 std::vector<int> task_chain);

// currently, as asusme there is only one chain
class TaskSetPermutation {
   public:
    TaskSetPermutation() {}
    TaskSetPermutation(const DAG_Model& dag_tasks,
                       const std::vector<int>& chain)
        : start_time_((std::chrono::high_resolution_clock::now())),
          dag_tasks_(dag_tasks),
          tasks_info_(
              RegularTaskSystem::TaskSetInfoDerived(dag_tasks.GetTaskSet())),
          task_chain_(chain),
          best_yet_obj_(1e9),
          iteration_count_(0),
          variable_range_od_(FindVariableRange(dag_tasks_)) {
        adjacent_two_task_permutations_.reserve(chain.size() - 1);
        FindPairPermutations();
        chain_permutations_.reserve(1e5);
    }

    void FindPairPermutations() {
        for (uint i = 0; i < task_chain_.size() - 1; i++) {
            if (ifTimeout(start_time_)) break;
            adjacent_two_task_permutations_.push_back(TwoTaskPermutations(
                task_chain_[i], task_chain_[i + 1], dag_tasks_, tasks_info_));
            std::cout << "Pair permutation #: "
                      << adjacent_two_task_permutations_.back().size() << "\n";
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
            if (ifTimeout(start_time_)) break;
            chain_perm.push_back(adjacent_two_task_permutations_[position][i]);
            if (GlobalVariablesDAGOpt::SKIP_PERM) {
                if (!chain_perm.IsValid(variable_range_od_)) {
                    chain_perm.pop_back();
                    continue;
                }
            }
            IterateAllChainPermutations(position + 1, chain_perm);
            chain_perm.pop_back();
        }
    }

    void EvaluateChainPermutation(const ChainPermutation& chain_perm) {
#ifdef PROFILE_CODE
        BeginTimer(__FUNCTION__);
#endif
        // evaluate schedulability
        VariableOD variable_od =
            FindODFromPermutation(dag_tasks_, chain_perm, task_chain_);

        if (variable_od.valid_)  // if valid, we'll exam obj; otherwise, we'll
                                 // just move forward
        {
            double obj_curr = ObjReactionTime::Obj(dag_tasks_, tasks_info_,
                                                   chain_perm, variable_od);
            if (obj_curr < best_yet_obj_) {
                best_yet_obj_ = obj_curr;
                best_yet_chain_ = chain_perm;
            }
        }
#ifdef PROFILE_CODE
        EndTimer(__FUNCTION__);
#endif
    }

    bool ExamSchedulabilityOptSol() const {
        return true;
    }  // TODO: finish this function

    // data members
    TimerType start_time_;
    DAG_Model dag_tasks_;
    RegularTaskSystem::TaskSetInfoDerived tasks_info_;
    std::vector<int> task_chain_;
    std::vector<TwoTaskPermutations> adjacent_two_task_permutations_;
    std::vector<ChainPermutation> chain_permutations_;
    ChainPermutation best_yet_chain_;
    int best_yet_obj_;
    int iteration_count_;
    VariableRange variable_range_od_;
};

template <typename ObjectiveFunctionBase>
ScheduleResult PerformTOM_OPT(const DAG_Model& dag_tasks) {
    auto start = std::chrono::high_resolution_clock::now();
    ScheduleResult res;
    TaskSetPermutation task_sets_perms =
        TaskSetPermutation(dag_tasks, dag_tasks.chains_[0]);
    res.obj_ = task_sets_perms.PerformOptimization();
    if (res.obj_ >= 1e8) {
        res.obj_ = PerformLETAnalysis<ObjectiveFunctionBase>(dag_tasks).obj_;
    }
    res.schedulable_ = task_sets_perms.ExamSchedulabilityOptSol();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    res.timeTaken_ = double(duration.count()) / 1e6;
    return res;
}
}  // namespace DAG_SPACE