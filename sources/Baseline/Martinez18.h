
#pragma once
#include <algorithm>  // for std::gcd

#include "sources/Baseline/JobCommunications.h"
#include "sources/Baseline/StandardLET.h"
#include "sources/ObjectiveFunction/ObjDataAge.h"
#include "sources/Optimization/LPSolver_Cplex.h"  // int INFEASIBLE_OBJ
#include "sources/Optimization/Variable.h"

namespace DAG_SPACE {
extern const int INFEASIBLE_OBJ;
class Martinez18Perm : public std::vector<int> {
 public:
  Martinez18Perm() {}
  Martinez18Perm(int n) {
    reserve(n);
    for (int i = 0; i < n; i++) push_back(0);
  }
  void print() const {
    for (uint i = 0; i < size(); i++) std::cout << at(i) << ", ";
    std::cout << "\n";
  }
};

VariableOD GetVariable(const Martinez18Perm& mart_perm,
                       const DAG_Model& dag_tasks);

int ObjDataAgeFromVariable(const Martinez18Perm& mart_perm,
                           const DAG_Model& dag_tasks,
                           const TaskSetInfoDerived& tasks_info,
                           const std::vector<int>& chain);

// TODO: add time_out
class Martinez18TaskSetPerms {
 public:
  // TaskSetPermutation() {}
  Martinez18TaskSetPerms(const DAG_Model& dag_tasks,
                         const std::vector<int>& chain)
      : start_time_((std::chrono::high_resolution_clock::now())),
        dag_tasks_(dag_tasks),
        tasks_info_(
            RegularTaskSystem::TaskSetInfoDerived(dag_tasks.GetTaskSet())),
        chain_(chain),
        best_yet_obj_(1e9),
        iteration_count_(0),
        rta_(GetResponseTimeTaskSet(dag_tasks_)) {
    FindPossibleOffsets();
  }

  std::vector<int> GetPossibleOffset(int gcd);

  void FindPossibleOffsets();

  int PerformOptimization();

  // position is the index in the chain, chain[position] is the task_id
  void Iterate(uint position, Martinez18Perm& perms_offset);

  void EvaluateMartPerm(Martinez18Perm& perms_offset);

  bool EvaluateMartSchedulability(Martinez18Perm& perms_offset);

  bool ExamSchedulabilityOptSol() {
    if (best_yet_obj_ == INFEASIBLE_OBJ)
      return false;
    else
      return true;
  }

  // data members
  TimerType start_time_;
  DAG_Model dag_tasks_;
  RegularTaskSystem::TaskSetInfoDerived tasks_info_;
  std::vector<int> chain_;
  int best_yet_obj_;
  int iteration_count_;
  std::vector<int> rta_;

  // task_id -> its possible offsets to try
  std::unordered_map<int, std::vector<int>> possible_offsets_map_;
  Martinez18Perm best_yet_variable_od_;
};

}  // namespace DAG_SPACE