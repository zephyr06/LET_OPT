#pragma once
#include <list>

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

class TaskSetPermutation {
 public:
  // TaskSetPermutation() {}
  TaskSetPermutation(const DAG_Model& dag_tasks,
                     const std::vector<std::vector<int>>& chains,
                     const std::string& type_trait);

  TaskSetPermutation(const DAG_Model& dag_tasks,
                     const std::vector<std::vector<int>>& chains);

  template <typename ObjectiveFunction>
  void InitializeSolutions() {
    best_yet_variable_od_ = VariableOD(dag_tasks_.GetTaskSet());
    Schedule schedule_stand_let =
        Variable2Schedule(tasks_info_, best_yet_variable_od_);
    best_yet_chain_ = GetChainsPermFromVariable(
        dag_tasks_, tasks_info_, graph_of_all_ca_chains_.chains_,
        ObjectiveFunction::type_trait, schedule_stand_let);
    best_yet_obj_ = ObjectiveFunction::Obj(
        dag_tasks_, tasks_info_, best_yet_chain_, best_yet_variable_od_,
        graph_of_all_ca_chains_.chains_);
    if (GlobalVariablesDAGOpt::OPTIMIZE_JITTER_WEIGHT)
      best_yet_obj_ += ObjectiveFunction::Jitter(
          dag_tasks_, tasks_info_, best_yet_chain_, best_yet_variable_od_,
          graph_of_all_ca_chains_.chains_);
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
  ScheduleResult GetScheduleResult() const {
    ScheduleResult res;
    res.jitter_ = ObjectiveFunction::Jitter(
        dag_tasks_, tasks_info_, best_yet_chain_, best_yet_variable_od_,
        graph_of_all_ca_chains_.chains_);
    // res.obj_ = best_yet_obj_;
    res.obj_ = ObjectiveFunction::Obj(dag_tasks_, tasks_info_, best_yet_chain_,
                                      best_yet_variable_od_,
                                      graph_of_all_ca_chains_.chains_);
    res.variable_opt_ = best_yet_variable_od_;
    res.schedulable_ = ExamSchedulabilityOptSol();
    return res;
  }
  // The following functions more related to optimization

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
};

//   bool WhetherContainInfeasibleSubChains(
//       const ChainsPermutation& chains_perm,
//       const std::vector<std::vector<int>>& sub_chains);

}  // namespace DAG_SPACE
