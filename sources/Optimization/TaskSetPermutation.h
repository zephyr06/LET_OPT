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
    return ObjectiveFunction::ObjPerChain(
        dag_tasks_, tasks_info_, best_yet_chain_, best_yet_variable_od_,
        graph_of_all_ca_chains_.chains_);
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
  VariableOD best_possible_variable_od_;
  VariableRange variable_range_od_;
  std::string type_trait_;
  int infeasible_iteration_ = 0;
};

//   bool WhetherContainInfeasibleSubChains(
//       const ChainsPermutation& chains_perm,
//       const std::vector<std::vector<int>>& sub_chains);

}  // namespace DAG_SPACE
