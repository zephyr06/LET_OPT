#pragma once
#include "hashtable.h"
#include "sources/Optimization/Edge.h"
#include "unordered_map"
#include "unordered_set"
#include "vector"

namespace DAG_SPACE {
class GraphOfChains {
   public:
    GraphOfChains() {}
    GraphOfChains(const std::vector<std::vector<int>>& chains);

    // data members
    std::vector<std::vector<int>> chains_;
    std::unordered_set<Edge> edge_records_;
    std::vector<Edge> edge_vec_ordered_;
    std::unordered_map<int, std::vector<int>>
        prev_tasks_;  // saves one task's immediate dependent tasks
};

}  // namespace DAG_SPACE