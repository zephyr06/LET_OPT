#pragma once

#include "sources/Optimization/Variable.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/ScheduleResults.h"

namespace DAG_SPACE {

inline int GetTailFluctuationFromVariableRange(
    int task_id, const VariableRange& variable_range) {
  return variable_range.upper_bound.at(task_id).deadline -
         variable_range.lower_bound.at(task_id).deadline;
}

inline int GetHeadFluctuationFromVariableRange(
    int task_id, const VariableRange& variable_range) {
  return variable_range.upper_bound.at(task_id).offset -
         variable_range.lower_bound.at(task_id).offset;
}
inline int GetHeadTailFluctuationFromVariableRange(
    const std::vector<int>& chain, const VariableRange& variable_range) {
  return GetHeadFluctuationFromVariableRange(chain.front(), variable_range) +
         GetTailFluctuationFromVariableRange(chain.back(), variable_range);
}
inline int GetPossibleHeadTailFluctuation(const DAG_Model& dag_tasks,
                                          const std::vector<int>& chain) {
  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  VariableRange variable_range = FindVariableRange(dag_tasks, rta);
  variable_range.upper_bound.print();
  variable_range.lower_bound.print();
  return GetHeadTailFluctuationFromVariableRange(chain, variable_range);
}

inline VariableRange FindVariableRangeImproved(const DAG_Model& dag_tasks,
                                               const std::vector<int>& rta,
                                               const VariableOD& variable_od) {
  VariableRange variable_range = FindVariableRange(dag_tasks, rta);
  for (uint i = 0; i < dag_tasks.GetTaskSet().size(); i++) {
    variable_range.lower_bound[i].offset = variable_od.at(i).offset;
    variable_range.upper_bound[i].deadline = variable_od.at(i).deadline;
  }
  return variable_range;
}

// inline int GetPossibleHeadTailFluctuationImproved(
//     const DAG_Model& dag_tasks, const std::vector<int>& chain,
//     const VariableOD& variable_od) {
//   std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
//   VariableRange variable_range =
//       FindVariableRangeImproved(dag_tasks, rta, variable_od);
//   return GetHeadTailFluctuationFromVariableRange(chain, variable_range);
// }

template <typename ObjectiveFunction>
int GetObjLowerBound(const DAG_Model& dag_tasks,
                     const std::vector<std::vector<int>>& chains) {
  int lb = 0;
  if (ObjectiveFunction::type_trait == "ReactionTime" ||
      ObjectiveFunction::type_trait == "DataAge")
    for (const auto& chain : chains) {
      for (int task_id : chain) {
        lb += dag_tasks.GetTask(task_id).executionTime;
      }
    }
  else
    CoutError("Unrecognized obj trait in GetObjLowerBound!");
  return lb;
}

template <typename ObjectiveFunction>
int GetObjLowerBoundSingleChain(const DAG_Model& dag_tasks,
                                const std::vector<int>& chain) {
  int lb = 0;
  if (ObjectiveFunction::type_trait == "ReactionTime" ||
      ObjectiveFunction::type_trait == "DataAge")
    for (int task_id : chain) {
      lb += dag_tasks.GetTask(task_id).executionTime;
    }
  else
    CoutError("Unrecognized obj trait in GetObjLowerBound!");
  return lb;
}

template <typename ObjectiveFunction>
int GetApproximatedObjBoundSingleChain(const DAG_Model& dag_tasks,
                                       const std::vector<int>& chain,
                                       const int prev_obj) {
  return std::max(
      prev_obj - GetPossibleHeadTailFluctuation(dag_tasks, chain),
      GetObjLowerBoundSingleChain<ObjectiveFunction>(dag_tasks, chain));
}

template <typename ObjectiveFunction>
int GetApproximatedObjBound(const DAG_Model& dag_tasks,
                            const std::vector<std::vector<int>>& chains,
                            const std::vector<double> prev_obj_per_chain) {
  if (prev_obj_per_chain.size() == 0)
    CoutError("Empty prev_obj_per_chain in GetApproximatedObjBound! ");
  int obj_lb = 0;
  for (uint i = 0; i < chains.size(); i++) {
    obj_lb += GetApproximatedObjBoundSingleChain<ObjectiveFunction>(
        dag_tasks, chains[i], prev_obj_per_chain[i]);
  }
  return obj_lb;
}

// template <typename ObjectiveFunction>
// int GetApproximatedObjBoundImproved(const DAG_Model& dag_tasks,
//                                     const std::vector<std::vector<int>>&
//                                     chains, int prev_obj, const VariableOD&
//                                     variable_od) {
//   int obj_lb = prev_obj;
//   for (const auto& chain : chains) {
//     obj_lb -=
//         GetPossibleHeadTailFluctuationImproved(dag_tasks, chain,
//         variable_od);
//   }
//   return std::max(obj_lb,
//                   GetObjLowerBound<ObjectiveFunction>(dag_tasks, chains));
// }

}  // namespace DAG_SPACE
