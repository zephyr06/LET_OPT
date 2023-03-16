#pragma once
#include <bits/stdc++.h>

#include <algorithm>  // for std::for_each
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_list.hpp>        // adjacency_list
#include <boost/graph/breadth_first_search.hpp>  // shortest paths
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graphviz.hpp>  // read_graphviz
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/topological_sort.hpp>  // find_if
#include <boost/range/algorithm.hpp>         // range find_if
#include <boost/utility.hpp>                 // for boost::tie
#include <iostream>                          // for std::cout
#include <utility>
#include <utility>  // for std::pair

#include "sources/TaskModel/RegularTasks.h"

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                              boost::property<boost::vertex_name_t, LLint>,
                              boost::property<boost::edge_name_t, LLint>>
    Graph;
// map to access properties of vertex from the graph
typedef boost::property_map<Graph, boost::vertex_name_t>::type
    vertex_name_map_t;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::property_map<Graph, boost::edge_name_t>::type edge_name_map_t;

typedef std::unordered_map<LLint, Vertex> indexVertexMap;

struct first_name_t {
    typedef boost::vertex_property_tag kind;
};

namespace DAG_SPACE {

static constexpr Vertex NIL = -1;
// Code from
// https://stackoverflow.com/questions/52878925/boostgraph-getting-the-path-up-to-the-root
std::vector<int> shortest_paths(Vertex root, Vertex target, Graph const &g);

void PrintChains(const std::vector<std::vector<int>> &chains);
// *2, 1 means task 2 depend on task 1, or task 1 must execute before task 2;
// 1 would be the first in MAP_Prev, while 2 is one task in TaskSet
// MAP_Prev maps one task to all the tasks it depends on
typedef std::map<int, RegularTaskSystem::TaskSet> MAP_Prev;
using namespace RegularTaskSystem;
class DAG_Model {
   public:
    DAG_Model() : sfBound_(-1), rtdaBound_(-1) {}
    DAG_Model(TaskSet &tasks, MAP_Prev &mapPrev, int numCauseEffectChain = 1)
        : tasks(tasks), mapPrev(mapPrev) {
        RecordTaskPosition();
        std::tie(graph_, indexesBGL_) = GenerateGraphForTaskSet();
        chains_ = GetRandomChains(numCauseEffectChain);
        sfBound_ = -1;
        rtdaBound_ = -1;
        CategorizeTaskSet();
    }

    DAG_Model(TaskSet &tasks, MAP_Prev &mapPrev, double sfBound,
              double rtdaBound, int numCauseEffectChain = 1)
        : tasks(tasks), mapPrev(mapPrev) {
        tasks = tasks;
        mapPrev = mapPrev;
        RecordTaskPosition();
        std::tie(graph_, indexesBGL_) = GenerateGraphForTaskSet();
        chains_ = GetRandomChains(numCauseEffectChain);
        sfBound_ = sfBound;
        rtdaBound_ = rtdaBound;
        CategorizeTaskSet();
    }

    std::pair<Graph, indexVertexMap> GenerateGraphForTaskSet() const;

    void addEdge(int prevIndex, int nextIndex) {
        mapPrev[nextIndex].push_back(GetTask(prevIndex));
    }

    void print();

    void printChains();

    TaskSet GetTasks() const { return tasks; }

    inline double GetSfBound() { return sfBound_; }
    inline void setSfBound(double sfBound) { sfBound_ = sfBound; }
    inline double GetRtdaBound() { return rtdaBound_; }
    inline void setRtdaBound(double rtdaBound) { rtdaBound_ = rtdaBound; }

    int edgeNumber();

    std::vector<std::vector<int>> GetRandomChains(int numOfChains);
    void SetChains(std::vector<std::vector<int>> &chains) { chains_ = chains; }
    std::vector<int> FindSourceTaskIds() const;

    std::vector<int> FindSinkTaskIds() const;

    void CategorizeTaskSet();
    void RecordTaskPosition();

    inline TaskSet GetTaskSet() const { return tasks; }

    inline int GetTaskIndex(int task_id) const {
        return task_id2position_.at(task_id);
    }
    inline Task GetTask(int task_id) const {
        return tasks[GetTaskIndex(task_id)];
    }

    // data member
   private:
    double sfBound_;
    double rtdaBound_;
    TaskSet tasks;

   public:
    MAP_Prev mapPrev;
    Graph graph_;
    indexVertexMap indexesBGL_;
    std::vector<std::vector<int>> chains_;
    std::unordered_map<int, TaskSet> processor2taskset_;
    std::unordered_map<int, uint> task_id2task_index_within_processor_;
    std::unordered_map<int, int> task_id2position_;
};

DAG_Model ReadDAG_Tasks(std::string path, std::string priorityType = "orig",
                        int chainNum = 1);

bool WhetherDAGChainsShareNodes(const DAG_Model &dag_tasks);

}  // namespace DAG_SPACE