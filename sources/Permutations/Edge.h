#pragma once
#include "hashtable.h"
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