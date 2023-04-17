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
  //   variable_range.upper_bound.print();
  //   variable_range.lower_bound.print();
  return GetHeadTailFluctuationFromVariableRange(chain, variable_range);
}

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
int GetApproximatedObjBound(const DAG_Model& dag_tasks,
                            const std::vector<std::vector<int>>& chains,
                            int prev_obj) {
  int obj_lb = prev_obj;
  for (const auto& chain : chains) {
    obj_lb -= GetPossibleHeadTailFluctuation(dag_tasks, chain);
  }
  return std::max(obj_lb,
                  GetObjLowerBound<ObjectiveFunction>(dag_tasks, chains));
}

}  // namespace DAG_SPACE
