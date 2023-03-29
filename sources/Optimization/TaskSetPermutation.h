#pragma once
#include <list>

#include "sources/Baseline/StandardLET.h"
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
#include "sources/Optimization/GraphOfChains.h"
#include "sources/Optimization/LPSolver_Cplex.h"
#include "sources/Permutations/ChainsPermutation.h"
#include "sources/Permutations/TwoTaskPermutationsIterator.h"
#include "sources/Utils/BatchUtils.h"
#include "sources/Utils/profilier.h"
namespace DAG_SPACE {

bool ExamVariableFeasibility(const VariableOD& variable,
                             const ChainsPermutation& chains_perm,
                             const GraphOfChains& graph_of_all_ca_chains,
                             const DAG_Model& dag_task,
                             const std::vector<int>& chain);

VariableOD FindODFromSingleChainPermutation(
    const DAG_Model& dag_tasks, const ChainsPermutation& chains_perm,
    const GraphOfChains& graph_of_all_ca_chains, const std::vector<int>& chain,
    const std::vector<int>& rta);

std::vector<std::vector<int>> GetSubChains(
    const std::vector<std::vector<int>>& chains_full_length,
    const ChainsPermutation& chains_perm);
// Note: this function doesn't change chains_perm
std::vector<std::unordered_map<JobCEC, JobCEC>> GetFirstReactMaps(
    ChainsPermutation& chains_perm,
    const std::shared_ptr<const SinglePairPermutation> single_perm,
    const std::vector<std::vector<int>>& chains, const DAG_Model& dag_tasks,
    const TaskSetInfoDerived& tasks_info);

std::vector<std::unordered_map<JobCEC, JobCEC>> GetFirstReactMaps(
    const ChainsPermutation& chains_perm,
    const std::vector<std::vector<int>>& chains, const DAG_Model& dag_tasks,
    const TaskSetInfoDerived& tasks_info);

// return true if it's possible for curr_first_job_maps to achieve better
// performance than curr_best_first_job_maps
bool CompareNewPerm(
    const std::vector<std::unordered_map<JobCEC, JobCEC>>& curr_first_job_maps,
    const std::vector<std::unordered_map<JobCEC, JobCEC>>&
        curr_best_first_job_maps);

// currently, as asusme there is only one chain
// TODO: what's the usage of chains in arguments
class TaskSetPermutation {
 public:
  // TaskSetPermutation() {}
  TaskSetPermutation(const DAG_Model& dag_tasks,
                     const std::vector<std::vector<int>>& chains);

  void FindPairPermutations();
  bool ExamSchedulabilityOptSol() const;

  template <typename ObjectiveFunction>
  int PerformOptimizationEnumerate() {
    ChainsPermutation chains_perm;
    IterateAllChainsPermutations<ObjectiveFunction>(0, chains_perm);
    lp_optimizer_
        .ClearCplexMemory();  // TODO: consider trying to optimize
                              // performance by directly set coefficient
                              // rather than remove/add constraints
    return best_yet_obj_;
  }

  // chains_perm already pushed the new perm_single
  template <typename ObjectiveFunction>
  bool WhetherSkipToNextPerm(const ChainsPermutation& chains_perm) {
#ifdef PROFILE_CODE
    BeginTimer(__FUNCTION__);
#endif

    std::vector<std::vector<int>> sub_chains =
        GetSubChains(dag_tasks_.chains_, chains_perm);
    for (const auto& sub_chain : sub_chains) {
      if (sub_chain.size() == 0) continue;
      if (GlobalVariablesDAGOpt::SKIP_PERM >= 2 &&
          !FindODFromSingleChainPermutation(dag_tasks_, chains_perm,
                                            graph_of_all_ca_chains_, sub_chain,
                                            rta_)
               .valid_) {
#ifdef PROFILE_CODE
        EndTimer(__FUNCTION__);
#endif
        return true;
      }
    }

    VariableOD best_possible_variable_od =
        FindBestPossibleVariableOD(dag_tasks_, tasks_info_, rta_, chains_perm);
    double obj_curr =
        ObjectiveFunction::Obj(dag_tasks_, tasks_info_, chains_perm,
                               best_possible_variable_od, sub_chains);
#ifdef PROFILE_CODE
    EndTimer(__FUNCTION__);
#endif

    if (obj_curr > best_yet_obj_) {
      return true;
    }
    return false;
  }

  // depth equals the number of edge pais
  template <typename ObjectiveFunction>
  void IterateAllChainsPermutations(uint position,
                                    ChainsPermutation& chains_perm) {
    if (position == graph_of_all_ca_chains_.edge_records_
                        .size()) {  // finish iterate all the pair permutations
      iteration_count_++;
      EvaluateChainsPermutation<ObjectiveFunction>(chains_perm);
      return;
    }

    for (uint i = 0; i < adjacent_two_task_permutations_[position].size();
         i++) {
      if (ifTimeout(start_time_)) break;
      if (chains_perm.IsValid(variable_range_od_,
                              *adjacent_two_task_permutations_[position][i],
                              graph_of_all_ca_chains_)) {
        chains_perm.push_back(adjacent_two_task_permutations_[position][i]);

        // try to skip some permutations
        if (!WhetherSkipToNextPerm<ObjectiveFunction>(chains_perm)) {
          IterateAllChainsPermutations<ObjectiveFunction>(position + 1,
                                                          chains_perm);
        }
        chains_perm.pop(*adjacent_two_task_permutations_[position][i]);
      }
    }
  }

  // optimize with Dynamic Programming
  // *********************************************
  template <typename ObjectiveFunction>
  int PerformOptimizationSort() {
    ChainsPermutation chains_perm;
    IterateSortedPerms<ObjectiveFunction>(0, chains_perm);
    lp_optimizer_.ClearCplexMemory();
    return best_yet_obj_;
  }

  template <typename ObjectiveFunction>
  double IterateSortedPerms(uint position, ChainsPermutation& chains_perm) {
    if (position == graph_of_all_ca_chains_.edge_records_
                        .size()) {  // finish iterate all the pair permutations
      iteration_count_++;
      return EvaluateChainsPermutation<ObjectiveFunction>(chains_perm);
    }

    double best_obj_this_level = 1e9;
    for (uint i = 0; i < adjacent_two_task_permutations_[position].size();
         i++) {
      if (ifTimeout(start_time_)) break;
      const auto& perm_sing_curr = adjacent_two_task_permutations_[position][i];

      if (chains_perm.IsValid(variable_range_od_, *perm_sing_curr,
                              graph_of_all_ca_chains_)) {
        chains_perm.push_back(perm_sing_curr);

        if (!WhetherSkipToNextPerm<ObjectiveFunction>(chains_perm)) {
          double curr_obj =
              IterateSortedPerms<ObjectiveFunction>(position + 1, chains_perm);
          if (curr_obj < best_obj_this_level) {
            best_obj_this_level = curr_obj;
          }
        }

        chains_perm.pop(*perm_sing_curr);
      }
    }
    return best_obj_this_level;
  }

  template <typename ObjectiveFunction>
  double EvaluateChainsPermutation(const ChainsPermutation& chains_perm) {
#ifdef PROFILE_CODE
    BeginTimer(__FUNCTION__);
#endif

    std::pair<VariableOD, int> res = FindODWithLP(
        dag_tasks_, tasks_info_, chains_perm, graph_of_all_ca_chains_,
        ObjectiveFunction::type_trait, rta_);
    // LPOptimizer lp_optimizer(dag_tasks_, tasks_info_,
    //                          graph_of_all_ca_chains_,
    //                          ObjectiveFunction::type_trait, rta_);
    // std::pair<VariableOD, int> res;
    // if (lp_optimizer_.name2ilo_const_.size() != 0) {
    //     res = lp_optimizer_.IncrementOptimize(chains_perm);
    // } else {
    //     lp_optimizer_.obj_trait_ = ObjectiveFunction::type_trait;
    //     res = lp_optimizer_.OptimizeWithoutClear(chains_perm);
    // }
    // if (GlobalVariablesDAGOpt::debugMode == 1)
    //     lp_optimizer_.WriteModelToFile();

    if (res.first.valid_)  // if valid, we'll exam obj; otherwise, we'll
                           // just move forward
    {
      if (res.second < best_yet_obj_) {
        best_yet_obj_ = res.second;
        best_yet_chain_ = chains_perm;
        best_yet_variable_od_ = res.first;
        // if (res.second == 1459) {
        //     int a = 1;
        // }
        std::vector<std::vector<int>> sub_chains =
            GetSubChains(dag_tasks_.chains_, chains_perm);
        double obj_curr =
            ObjectiveFunction::Obj(dag_tasks_, tasks_info_, chains_perm,
                                   best_possible_variable_od_, sub_chains);
        if (obj_curr > best_yet_obj_)
          CoutError("Something's wrong with sub-chain obj evaluation");

#ifdef PROFILE_CODE
        EndTimer(__FUNCTION__);
#endif
        return res.second;
      }
    } else {
      infeasible_iteration_++;
    }

#ifdef PROFILE_CODE
    EndTimer(__FUNCTION__);
#endif
    return 1e9;
  }

  // data members
  TimerType start_time_;
  DAG_Model dag_tasks_;
  RegularTaskSystem::TaskSetInfoDerived tasks_info_;
  GraphOfChains graph_of_all_ca_chains_;
  std::vector<TwoTaskPermutations> adjacent_two_task_permutations_;
  ChainsPermutation best_yet_chain_;
  int best_yet_obj_;
  int iteration_count_;
  VariableOD best_yet_variable_od_;
  std::vector<int> rta_;
  VariableOD best_possible_variable_od_;
  VariableRange variable_range_od_;
  int infeasible_iteration_ = 0;
  LPOptimizer lp_optimizer_;
};

template <typename ObjectiveFunction>
ScheduleResult PerformTOM_OPT(const DAG_Model& dag_tasks,
                              std::string method = "Enumerate") {
  auto start = std::chrono::high_resolution_clock::now();
  ScheduleResult res;
  TaskSetPermutation task_sets_perms =
      TaskSetPermutation(dag_tasks, dag_tasks.chains_);
  if (method == "Enumerate")
    res.obj_ =
        task_sets_perms.PerformOptimizationEnumerate<ObjectiveFunction>();
  else if (method == "Sort")
    res.obj_ = task_sets_perms.PerformOptimizationSort<ObjectiveFunction>();
  else
    CoutError("Unrecognized method in PerformTOM_OPT");
  if (res.obj_ >= 1e8) {
    res.obj_ = PerformStandardLETAnalysis<ObjectiveFunction>(dag_tasks).obj_;
  }
  res.schedulable_ = task_sets_perms.ExamSchedulabilityOptSol();

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  res.timeTaken_ = double(duration.count()) / 1e6;
  std::cout << "The total number of permutation iterations is: "
            << task_sets_perms.iteration_count_ << "\n";
  if (GlobalVariablesDAGOpt::debugMode == 1) {
    std::cout << "The best permutation is: \n";
    task_sets_perms.best_yet_chain_.print();
  }
  if (!res.schedulable_ &&
      res.timeTaken_ < GlobalVariablesDAGOpt::TIME_LIMIT - 5)
    CoutError("Find an unschedulable case!");
  return res;
}

}  // namespace DAG_SPACE
