#pragma once
#include "hashtable.h"
#include "unordered_map"
#include "unordered_set"
#include "vector"

namespace DAG_SPACE {
struct Edge {
    Edge() {}
    Edge(int f, int t) : from_id(f), to_id(t) {}
    int from_id;
    int to_id;

    bool operator==(const Edge& other) const {
        return from_id == other.from_id && to_id == other.to_id;
    }
    bool operator!=(const Edge& other) const { return !(*this == other); }
};

}  // namespace DAG_SPACE

template <>
struct std::hash<DAG_SPACE::Edge> {
    std::size_t operator()(const DAG_SPACE::Edge& edge) const {
        return edge.from_id * 1e4 + edge.to_id;
    }
};

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