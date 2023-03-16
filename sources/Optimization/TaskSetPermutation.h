#pragma once

#include "sources/Baseline/StandardLET.h"
#include "sources/Optimization/ChainPermutation.h"
#include "sources/Optimization/GraphOfChains.h"
#include "sources/Optimization/ObjectiveFunction.h"
#include "sources/Utils/BatchUtils.h"
#include "sources/Utils/profilier.h"

namespace DAG_SPACE {

// assume the simple response time analysis
// VariableOD FindODFromPermutation(const DAG_Model& dag_tasks,
//                                  const ChainPermutation& chain_perm,
//                                  std::vector<int> task_chain);

VariableOD FindODFromPermutation(const DAG_Model& dag_tasks,
                                 const ChainPermutation& chain_perm,
                                 const GraphOfChains& graph_of_all_ca_chains);

// currently, as asusme there is only one chain
class TaskSetPermutation {
   public:
    TaskSetPermutation() {}
    TaskSetPermutation(const DAG_Model& dag_tasks,
                       const std::vector<std::vector<int>>& chains)
        : start_time_((std::chrono::high_resolution_clock::now())),
          dag_tasks_(dag_tasks),
          tasks_info_(
              RegularTaskSystem::TaskSetInfoDerived(dag_tasks.GetTaskSet())),
          graph_of_all_ca_chains_(chains),
          best_yet_obj_(1e9),
          iteration_count_(0),
          variable_range_od_(FindVariableRange(dag_tasks_)) {
        adjacent_two_task_permutations_.reserve(
            1e2);  // there are never more than 1e2 edges
        FindPairPermutations();
        chain_permutations_.reserve(1e5);
    }

    void FindPairPermutations() {
        for (const auto& edge_curr :
             graph_of_all_ca_chains_.edge_vec_ordered_) {
            if (ifTimeout(start_time_)) break;
            adjacent_two_task_permutations_.push_back(TwoTaskPermutations(
                edge_curr.from_id, edge_curr.to_id, dag_tasks_, tasks_info_));
            std::cout << "Pair permutation #: "
                      << adjacent_two_task_permutations_.back().size() << "\n";
        }
    }

    int PerformOptimization() {
        ChainPermutation chain_perm;
        IterateAllChainPermutations(0, chain_perm);
        return best_yet_obj_;
    }

    // depth equals the number of edge pais
    void IterateAllChainPermutations(uint position,
                                     ChainPermutation& chain_perm) {
        if (position ==
            graph_of_all_ca_chains_.edge_records_
                .size()) {  // finish iterate all the pair permutations
            iteration_count_++;
            EvaluateChainPermutation(chain_perm);
            return;
        }

        for (uint i = 0; i < adjacent_two_task_permutations_[position].size();
             i++) {
            if (ifTimeout(start_time_)) break;
            if ((!GlobalVariablesDAGOpt::SKIP_PERM) ||
                chain_perm.IsValid(variable_range_od_,
                                   adjacent_two_task_permutations_[position][i],
                                   graph_of_all_ca_chains_)) {
                chain_perm.push_back(
                    adjacent_two_task_permutations_[position][i]);
                IterateAllChainPermutations(position + 1, chain_perm);
                chain_perm.pop(adjacent_two_task_permutations_[position][i]);
            }
        }
    }

    void EvaluateChainPermutation(const ChainPermutation& chain_perm) {
#ifdef PROFILE_CODE
        BeginTimer(__FUNCTION__);
#endif
        // evaluate schedulability
        VariableOD variable_od = FindODFromPermutation(dag_tasks_, chain_perm,
                                                       graph_of_all_ca_chains_);

        if (variable_od.valid_)  // if valid, we'll exam obj; otherwise, we'll
                                 // just move forward
        {
            double obj_curr = ObjReactionTime::Obj(dag_tasks_, tasks_info_,
                                                   chain_perm, variable_od);
            if (obj_curr < best_yet_obj_) {
                best_yet_obj_ = obj_curr;
                best_yet_chain_ = chain_perm;
                best_yet_variable_od_ = variable_od;
            }
        }
#ifdef PROFILE_CODE
        EndTimer(__FUNCTION__);
#endif
    }

    bool ExamSchedulabilityOptSol() const {
        if (best_yet_variable_od_.size() == 0) return false;
        for (int i = 0; i < tasks_info_.N; i++) {
            int offset = best_yet_variable_od_.at(i).offset;
            int deadline = best_yet_variable_od_.at(i).deadline;
            int rta = GetResponseTime(dag_tasks_, i);
            if (rta + offset > deadline ||
                deadline > dag_tasks_.GetTask(i).deadline)
                return false;
        }
        if (GlobalVariablesDAGOpt::debugMode == 1) {
            best_yet_variable_od_.print();
        }
        return true;
    }

    // data members
    TimerType start_time_;
    DAG_Model dag_tasks_;
    RegularTaskSystem::TaskSetInfoDerived tasks_info_;
    GraphOfChains graph_of_all_ca_chains_;
    std::vector<TwoTaskPermutations> adjacent_two_task_permutations_;
    std::vector<ChainPermutation> chain_permutations_;
    ChainPermutation best_yet_chain_;
    int best_yet_obj_;
    int iteration_count_;
    VariableOD best_yet_variable_od_;
    VariableRange variable_range_od_;
};

template <typename ObjectiveFunctionBase>
ScheduleResult PerformTOM_OPT(const DAG_Model& dag_tasks) {
    auto start = std::chrono::high_resolution_clock::now();
    ScheduleResult res;
    TaskSetPermutation task_sets_perms =
        TaskSetPermutation(dag_tasks, dag_tasks.chains_);
    res.obj_ = task_sets_perms.PerformOptimization();
    if (res.obj_ >= 1e8) {
        res.obj_ =
            PerformStandardLETAnalysis<ObjectiveFunctionBase>(dag_tasks).obj_;
    }
    res.schedulable_ = task_sets_perms.ExamSchedulabilityOptSol();
    if (!res.schedulable_) CoutError("Find an unschedulable case!");

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    res.timeTaken_ = double(duration.count()) / 1e6;
    return res;
}
}  // namespace DAG_SPACE
