#pragma once
#include "sources/Optimization/TaskSetPermutation.h"
namespace DAG_SPACE {
bool AllChainsNotEmpty(const std::vector<std::vector<int>>& chains);
class TaskSetOptSorted_Maia23 : public TaskSetOptSorted {
 public:
  TaskSetOptSorted_Maia23(const DAG_Model& dag_tasks,
                          const std::vector<std::vector<int>>& chains,
                          const std::string& type_trait)
      : TaskSetOptSorted(dag_tasks, chains, type_trait) {}

  template <typename ObjectiveFunction>
  double EvaluateChainsPermutation(const ChainsPermutation& chains_perm) {
#ifdef PROFILE_CODE
    BeginTimer(__FUNCTION__);
#endif
    // chains_perm.print();
    std::pair<VariableOD, int> res = FindODWithLP(
        dag_tasks_, tasks_info_, chains_perm, graph_of_all_ca_chains_,
        ObjectiveFunction::type_trait, rta_, optimize_offset_only_);

    {
      double obj_maia = ObtainObjAfterMaia<ObjectiveFunction>(
          res.first, dag_tasks_, tasks_info_);
      if (obj_maia < res.second) {
        res.second = obj_maia;
        Schedule schedule_cur =
            SimulateFixedPrioritySched_OD(dag_tasks_, tasks_info_, res.first);
        VariableOD variable_after_Maia =
            GetMaia23VariableOD(dag_tasks_, tasks_info_, schedule_cur);
        res.first = variable_after_Maia;
        if (!CheckSchedulability(dag_tasks_, tasks_info_, schedule_cur,
                                 variable_after_Maia)) {
          CoutWarning("Unschedulable after Maia!");
        }
      }
    }

    if (res.first.valid_)  // if valid, we'll exam obj; otherwise, we'll
                           // just move forward
    {
      feasible_chains_.push_back(
          FeasibleChainManager(chains_perm, adjacent_two_task_permutations_,
                               ObjectiveFunction::type_trait));

      if (res.second < best_yet_obj_) {
        best_yet_obj_ = res.second;
        best_yet_chain_ = chains_perm;
        best_yet_variable_od_ = res.first;
      }
    } else {
      infeasible_iteration_++;
    }

#ifdef PROFILE_CODE
    EndTimer(__FUNCTION__);
#endif
    return res.second;
  }

  // data members
  FeasiblieChainsManagerVec feasible_chains_;
  int decrease_success = 0;
  int decrease_fail = 0;
  bool optimize_offset_only_ = false;
};

}  // namespace DAG_SPACE