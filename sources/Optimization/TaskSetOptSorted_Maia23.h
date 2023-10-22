#pragma once
#include "sources/Optimization/TaskSetOptSorted.h"
#include "sources/Optimization/TaskSetPermutation.h"

namespace DAG_SPACE {
bool AllChainsNotEmpty(const std::vector<std::vector<int>>& chains);

class TaskSetOptSorted_Maia23 : public TaskSetOptSorted {
 public:
  TaskSetOptSorted_Maia23(const DAG_Model& dag_tasks,
                          const std::vector<std::vector<int>>& chains,
                          const std::string& type_trait)
      : TaskSetOptSorted(dag_tasks, chains, type_trait) {
    EnableExtraOptimization();
  }
};

}  // namespace DAG_SPACE