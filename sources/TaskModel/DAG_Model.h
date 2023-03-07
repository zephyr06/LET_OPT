#pragma once
#include <utility>
#include <boost/config.hpp>
#include <iostream>  // for std::cout
#include <utility>   // for std::pair
#include <algorithm> // for std::for_each
#include <bits/stdc++.h>
#include <boost/utility.hpp> // for boost::tie
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/topological_sort.hpp>

#include <boost/graph/adjacency_list.hpp>       // adjacency_list
#include <boost/graph/topological_sort.hpp>     // find_if
#include <boost/graph/breadth_first_search.hpp> // shortest paths
#include <boost/range/algorithm.hpp>            // range find_if
#include <boost/graph/graphviz.hpp>             // read_graphviz

#include "sources/TaskModel/RegularTasks.h"

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                              boost::property<boost::vertex_name_t, LLint>,
                              boost::property<boost::edge_name_t, LLint>>
    Graph;
// map to access properties of vertex from the graph
typedef boost::property_map<Graph, boost::vertex_name_t>::type vertex_name_map_t;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::property_map<Graph, boost::edge_name_t>::type edge_name_map_t;

typedef std::unordered_map<LLint, Vertex> indexVertexMap;

struct first_name_t
{
    typedef boost::vertex_property_tag kind;
};

// Code from https://stackoverflow.com/questions/52878925/boostgraph-getting-the-path-up-to-the-root
static constexpr Vertex NIL = -1;
std::vector<int> shortest_paths(Vertex root, Vertex target, Graph const &g);

namespace OrderOptDAG_SPACE
{

    void PrintChains(const std::vector<std::vector<int>> &chains);
    // *2, 1 means task 2 depend on task 1, or task 1 must execute before task 2;
    // 1 would be the first in MAP_Prev, while 2 is one task in TaskSet
    // MAP_Prev maps one task to all the tasks it depends on
    typedef std::map<int, RegularTaskSystem::TaskSet> MAP_Prev;
    using namespace RegularTaskSystem;
    class DAG_Model
    {
    private:
        double sfBound_;
        double rtdaBound_;

    public:
        TaskSet tasks;
        MAP_Prev mapPrev;
        Graph graph_;
        indexVertexMap indexesBGL_;
        std::vector<std::vector<int>> chains_;

        DAG_Model() : sfBound_(-1), rtdaBound_(-1) {}
        DAG_Model(TaskSet &tasks, MAP_Prev &mapPrev, int numCauseEffectChain = 1) : tasks(tasks), mapPrev(mapPrev)
        {
            std::tie(graph_, indexesBGL_) = GenerateGraphForTaskSet();
            chains_ = GetRandomChains(numCauseEffectChain);
            sfBound_ = -1;
            rtdaBound_ = -1;
        }

        DAG_Model(TaskSet &tasks, MAP_Prev &mapPrev, double sfBound, double rtdaBound, int numCauseEffectChain = 1)
            : tasks(tasks), mapPrev(mapPrev)
        {
            tasks = tasks;
            mapPrev = mapPrev;
            std::tie(graph_, indexesBGL_) = GenerateGraphForTaskSet();
            chains_ = GetRandomChains(numCauseEffectChain);
            sfBound_ = sfBound;
            rtdaBound_ = rtdaBound;
        }

        std::pair<Graph, indexVertexMap> GenerateGraphForTaskSet() const;

        void addEdge(int prevIndex, int nextIndex)
        {
            mapPrev[nextIndex].push_back(tasks[prevIndex]);
        }

        void print();

        void printChains();

        TaskSet GetTasks() const
        {
            return tasks;
        }

        inline double GetSfBound() { return sfBound_; }
        inline void setSfBound(double sfBound) { sfBound_ = sfBound; }
        inline double GetRtdaBound() { return rtdaBound_; }
        inline void setRtdaBound(double rtdaBound) { rtdaBound_ = rtdaBound; }

        int edgeNumber();

        std::vector<std::vector<int>> GetRandomChains(int numOfChains);
        void SetChains(std::vector<std::vector<int>> &chains)
        {
            chains_ = chains;
        }
        std::vector<int> FindSourceTaskIds() const;

        std::vector<int> FindSinkTaskIds() const;
    };

    DAG_Model ReadDAG_Tasks(std::string path, std::string priorityType = "orig", int chainNum = 1);

}