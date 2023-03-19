#pragma once

#include "sources/Baseline/StandardLET.h"
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
#include "sources/Optimization/ChainsPermutation.h"
#include "sources/Optimization/GraphOfChains.h"
#include "sources/Optimization/LPSolver_Cplex.h"
#include "sources/Utils/BatchUtils.h"
#include "sources/Utils/profilier.h"
namespace DAG_SPACE {

// assume the simple response time analysis
// VariableOD FindODFromPermutation(const DAG_Model& dag_tasks,
//                                  const ChainsPermutation& chain_perm,
//                                  std::vector<int> task_chain);

bool ExamVariableFeasibility(const VariableOD& variable,
                             const ChainsPermutation& chain_perm,
                             const GraphOfChains& graph_of_all_ca_chains);

VariableOD FindODFromPermutation(const DAG_Model& dag_tasks,
                                 const ChainsPermutation& chain_perm,
                                 const GraphOfChains& graph_of_all_ca_chains);

// currently, as asusme there is only one chain
class TaskSetPermutation {
   public:
    TaskSetPermutation() {}
    TaskSetPermutation(const DAG_Model& dag_tasks,
                       const std::vector<std::vector<int>>& chains);

    void FindPairPermutations();
    bool ExamSchedulabilityOptSol() const;

    template <typename ObjectiveFunctionBase>
    int PerformOptimization() {
        ChainsPermutation chain_perm;
        IterateAllChainsPermutations<ObjectiveFunctionBase>(0, chain_perm);
        return best_yet_obj_;
    }

    // depth equals the number of edge pais

    template <typename ObjectiveFunctionBase>
    void IterateAllChainsPermutations(uint position,
                                      ChainsPermutation& chain_perm) {
        if (position ==
            graph_of_all_ca_chains_.edge_records_
                .size()) {  // finish iterate all the pair permutations
            iteration_count_++;
            EvaluateChainsPermutation<ObjectiveFunctionBase>(chain_perm);
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
                IterateAllChainsPermutations<ObjectiveFunctionBase>(
                    position + 1, chain_perm);
                chain_perm.pop(adjacent_two_task_permutations_[position][i]);
            }
        }
    }

    template <typename ObjectiveFunctionBase>
    void EvaluateChainsPermutation(const ChainsPermutation& chain_perm) {
#ifdef PROFILE_CODE
        BeginTimer(__FUNCTION__);
#endif

        std::unordered_map<JobCEC, JobCEC> react_chain_map;
        std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks_);

        std::pair<VariableOD, int> res = FindODWithLP(
            dag_tasks_, tasks_info_, chain_perm, graph_of_all_ca_chains_,
            ObjectiveFunctionBase::type_trait, react_chain_map, rta);

        if (res.first.valid_)  // if valid, we'll exam obj; otherwise, we'll
                               // just move forward
        {
            if (res.second < best_yet_obj_) {
                best_yet_obj_ = res.second;
                best_yet_chain_ = chain_perm;
                best_yet_variable_od_ = res.first;
            }
        }

#ifdef PROFILE_CODE
        EndTimer(__FUNCTION__);
#endif
    }

    // data members
    TimerType start_time_;
    DAG_Model dag_tasks_;
    RegularTaskSystem::TaskSetInfoDerived tasks_info_;
    GraphOfChains graph_of_all_ca_chains_;
    std::vector<TwoTaskPermutations> adjacent_two_task_permutations_;
    std::vector<ChainsPermutation> chain_permutations_;
    ChainsPermutation best_yet_chain_;
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
    res.obj_ = task_sets_perms.PerformOptimization<ObjectiveFunctionBase>();
    if (res.obj_ >= 1e8) {
        res.obj_ =
            PerformStandardLETAnalysis<ObjectiveFunctionBase>(dag_tasks).obj_;
    }
    res.schedulable_ = task_sets_perms.ExamSchedulabilityOptSol();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    res.timeTaken_ = double(duration.count()) / 1e6;

    if (!res.schedulable_ &&
        res.timeTaken_ < GlobalVariablesDAGOpt::TIME_LIMIT - 5)
        CoutError("Find an unschedulable case!");
    return res;
}
}  // namespace DAG_SPACE
