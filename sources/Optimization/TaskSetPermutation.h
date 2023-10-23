#pragma once
#include <list>

#include "sources/Baseline/Maia23.h"
#include "sources/Baseline/StandardLET.h"
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
#include "sources/Optimization/LPSolver_Cplex.h"
#include "sources/Optimization/TaskSetPermutationHelper.h"
#include "sources/Permutations/ChainsPermutation.h"
#include "sources/Permutations/GraphOfChains.h"
#include "sources/Permutations/TwoTaskPermutationsIterator.h"
#include "sources/Utils/BatchUtils.h"
#include "sources/Utils/ScheduleResults.h"
#include "sources/Utils/profilier.h"
namespace DAG_SPACE {

bool ExamSchedulabilityVariable(const VariableOD& variable_od,
                                const DAG_Model& dag_tasks,
                                const TaskSetInfoDerived& tasks_info);

class TaskSetPermutation {
 public:
  // TaskSetPermutation() {}
  TaskSetPermutation(const DAG_Model& dag_tasks,
                     const std::vector<std::vector<int>>& chains,
                     const std::string& type_trait);

  TaskSetPermutation(const DAG_Model& dag_tasks,
                     const std::vector<std::vector<int>>& chains);

  enum InitialMethod { DefaultLET, Maia23, Both };

  template <typename ObjectiveFunction>
  void InitializeSolutions(InitialMethod initial = DefaultLET) {
    VariableOD variable_to_try;

    if (initial == DefaultLET)
      variable_to_try = VariableOD(dag_tasks_.GetTaskSet());
    else if (initial == Maia23)
      variable_to_try = GetMaia23VariableOD(dag_tasks_, tasks_info_);
    else {
      CoutError("Unknown initial  method!");
    }

    Schedule schedule_to_try = Variable2Schedule(tasks_info_, variable_to_try);
    ChainsPermutation chain_to_try = GetChainsPermFromVariable(
        dag_tasks_, tasks_info_, graph_of_all_ca_chains_.chains_,
        ObjectiveFunction::type_trait, schedule_to_try);
    TryUpdateBestYetResults<ObjectiveFunction>(chain_to_try, variable_to_try);
  }

  template <typename ObjectiveFunction>
  void TryUpdateBestYetResults(const ChainsPermutation& chain_to_try,
                               const VariableOD& variable_to_try) {
    double obj_to_try = ObjectiveFunction::Obj(dag_tasks_, tasks_info_,
                                               chain_to_try, variable_to_try,
                                               graph_of_all_ca_chains_.chains_);
    if (IfSF_Trait(ObjectiveFunction::type_trait) &&
        GlobalVariablesDAGOpt::OPTIMIZE_JITTER_WEIGHT)
      obj_to_try += GlobalVariablesDAGOpt::OPTIMIZE_JITTER_WEIGHT *
                    ObjectiveFunction::Jitter(dag_tasks_, tasks_info_,
                                              chain_to_try, variable_to_try,
                                              graph_of_all_ca_chains_.chains_);
    if (obj_to_try < best_yet_obj_) {
      best_yet_obj_ = obj_to_try;
      best_yet_variable_od_ = variable_to_try;
      best_yet_chain_ = chain_to_try;
    }
  }

  void FindPairPermutations();
  bool ExamSchedulabilityOptSol() const;
  std::vector<Edge> GetAllEdges() const;
  std::vector<Edge> GetUnvisitedFutureEdges(uint position) const;
  // print optimization trajectory
  void PrintSinglePermIndex(const ChainsPermutation& chains_perm) const;
  void PrintSinglePermIndex(const ChainsPermutation& chains_perm,
                            const std::vector<Edge>& edges) const;

  template <typename ObjectiveFunction>
  inline std::vector<double> GetOptObjPerChain() const {
    return ObjectiveFunction::ObjAllChains(
        dag_tasks_, tasks_info_, best_yet_chain_, best_yet_variable_od_,
        graph_of_all_ca_chains_.chains_);
  }
  template <typename ObjectiveFunction>
  ScheduleResult GetScheduleResultExceptSchedulability() const {
    ScheduleResult res;
    res.jitter_ = ObjectiveFunction::Jitter(
        dag_tasks_, tasks_info_, best_yet_chain_, best_yet_variable_od_,
        graph_of_all_ca_chains_.chains_);
    res.obj_ = ObjectiveFunction::Obj(dag_tasks_, tasks_info_, best_yet_chain_,
                                      best_yet_variable_od_,
                                      graph_of_all_ca_chains_.chains_);
    res.variable_opt_ = best_yet_variable_od_;
    // res.schedulable_ = ExamSchedulabilityOptSol();
    return res;
  }

  template <typename ObjectiveFunction>
  ScheduleResult GetScheduleResult() const {
    ScheduleResult res =
        GetScheduleResultExceptSchedulability<ObjectiveFunction>();
    res.schedulable_ = ExamSchedulabilityOptSol();
    return res;
  }

  // The following functions more related to optimization

  void ReOrderTwoTaskPermutations();

  void EnableExtraOptimization() { enableExtraOpt_ = true; }

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
  VariableRange variable_range_od_;
  std::string type_trait_;
  int infeasible_iteration_ = 0;
  bool found_optimal_ = false;
  bool enableExtraOpt_ = false;
};

// For ordering perms when optimizing SF *****************
inline int CountOverlapScore(int perm_id,
                             const std::vector<TwoTaskPermutations>& perms,
                             const std::vector<int>& task_id_count) {
  return task_id_count[perms[perm_id].task_next_id_] +
         task_id_count[perms[perm_id].task_prev_id_];
}
inline void UpdateTaskIdCount(int perm_id,
                              const std::vector<TwoTaskPermutations>& perms,
                              std::vector<int>& task_id_count) {
  task_id_count[perms[perm_id].task_prev_id_]++;
  task_id_count[perms[perm_id].task_next_id_]++;
}

void RemoveValue(std::vector<int>& perm_to_add, int val);
int SelectPermWithMostOverlap(const std::vector<TwoTaskPermutations>& perms,
                              const std::vector<int>& perm_to_add,
                              const std::vector<int>& task_id_count,
                              int prev_sink_id);
//   bool WhetherContainInfeasibleSubChains(
//       const ChainsPermutation& chains_perm,
//       const std::vector<std::vector<int>>& sub_chains);

}  // namespace DAG_SPACE
