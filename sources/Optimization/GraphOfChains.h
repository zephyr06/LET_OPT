#pragma once
#include "hashtable.h"
#include "sources/Optimization/Edge.h"
#include "sources/Utils/testMy.h"
#include "unordered_map"
#include "unordered_set"
#include "vector"

namespace DAG_SPACE {
class GraphOfChains {
   public:
    GraphOfChains() {}
    GraphOfChains(const std::vector<std::vector<int>>& chains);

    std::vector<int> GetPrevTasks(int task_id) const;

    std::vector<int> GetNextTasks(int task_id) const;

    // data members
    std::vector<std::vector<int>> chains_;
    std::unordered_set<Edge> edge_records_;
    std::vector<Edge> edge_vec_ordered_;
    std::unordered_map<int, std::vector<int>>
        prev_tasks_;  // saves one task's immediate dependent tasks
    std::unordered_map<int, std::vector<int>>
        next_tasks_;  // saves one task's immediate following tasks
};

}  // namespace DAG_SPACE