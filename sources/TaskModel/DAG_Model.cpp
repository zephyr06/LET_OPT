#include "sources/TaskModel/DAG_Model.h"

namespace OrderOptDAG_SPACE
{

    std::vector<int> shortest_paths(Vertex root, Vertex target, Graph const &g)
    {
        std::vector<int> path;
        // find shortest paths from the root
        std::vector<Vertex> predecessors(boost::num_vertices(g), NIL);
        auto recorder = boost::record_predecessors(predecessors.data(), boost::on_examine_edge());
        boost::breadth_first_search(g, root, boost::visitor(boost::make_bfs_visitor(recorder)));

        for (auto pred = predecessors[target]; pred != NIL; pred = predecessors[pred])
        {
            path.push_back(pred);
        }
        if (path.size() != 0)
        {
            std::reverse(path.begin(), path.end());
            path.push_back(target);
        }
        return path;
    }

    void PrintChains(const std::vector<std::vector<int>> &chains)
    {
        std::cout << "Chains:" << std::endl;
        for (size_t i = 0; i < size(chains); i++)
        {
            for (size_t j = 0; j < size(chains[i]); j++)
            {
                std::cout << chains[i][j] << ", ";
            }
            std::cout << std::endl;
        }
    }

    // *2, 1 means task 2 depend on task 1, or task 1 must execute before task 2;
    // 1 would be the first in MAP_Prev, while 2 is one task in TaskSet
    // MAP_Prev maps one task to all the tasks it depends on
    using namespace RegularTaskSystem;

    std::pair<Graph, indexVertexMap> DAG_Model::GenerateGraphForTaskSet() const
    {

        Graph g;
        // map to access properties of vertex from the graph
        vertex_name_map_t vertex2indexBig = get(boost::vertex_name, g);

        // map to access vertex from its global index
        indexVertexMap indexesBGL;
        for (uint i = 0; i < tasks.size(); i++)
        {
            indexVertexMap::iterator pos;
            bool inserted;
            Vertex u;
            boost::tie(pos, inserted) = indexesBGL.insert(std::make_pair(i, Vertex()));
            if (inserted)
            {
                u = add_vertex(g);
                vertex2indexBig[u] = i;
                pos->second = u;
            }
            else
            {
                CoutError("Error building indexVertexMap!");
            }
        }

        // add edges

        for (auto itr = mapPrev.begin(); itr != mapPrev.end(); itr++)
        {
            const TaskSet &tasksPrev = itr->second;
            size_t indexNext = itr->first;
            for (size_t i = 0; i < tasksPrev.size(); i++)
            {
                boost::add_edge(tasksPrev[i].id, tasks[indexNext].id, g);
            }
        }
        return std::make_pair(g, indexesBGL);
    }

    void DAG_Model::print()
    {
        for (auto &task : tasks)
            task.print();
        for (auto itr = mapPrev.begin(); itr != mapPrev.end(); itr++)
        {
            for (uint i = 0; i < (itr->second).size(); i++)
                std::cout << "Edge: " << ((itr->second)[i].id) << "-->" << (itr->first) << std::endl;
        }
    }

    void DAG_Model::printChains()
    {
        for (size_t i = 0; i < chains_.size(); i++)
        {
            std::cout << "Chain #" << i << ": ";
            for (auto task : chains_[i])
            {
                std::cout << task << ", ";
            }
            std::cout << std::endl;
        }
    }

    int DAG_Model::edgeNumber()
    {
        int count = 0;
        for (auto itr = mapPrev.begin(); itr != mapPrev.end(); itr++)
        {
            count += (itr->second).size();
        }
        return count;
    }

    std::vector<int> DAG_Model::FindSourceTaskIds() const
    {
        std::set<int> originTasks;
        for (uint i = 0; i < tasks.size(); i++)
        {
            originTasks.insert(i);
        }

        for (auto itr = mapPrev.begin(); itr != mapPrev.end(); itr++)
        {
            size_t indexNext = itr->first;
            originTasks.erase(indexNext);
        }
        std::vector<int> res(originTasks.size());
        std::copy(originTasks.begin(), originTasks.end(), res.begin());
        return res;
    }
    std::vector<int> DAG_Model::FindSinkTaskIds() const
    {
        std::set<int> originTasks;
        for (uint i = 0; i < tasks.size(); i++)
        {
            originTasks.insert(i);
        }

        for (auto itr = mapPrev.begin(); itr != mapPrev.end(); itr++)
        {
            auto parents = itr->second;
            for (auto p : parents)
            {
                originTasks.erase(p.id);
            }
        }
        std::vector<int> res(originTasks.size());
        std::copy(originTasks.begin(), originTasks.end(), res.begin());
        return res;
    }

    std::vector<std::vector<int>> DAG_Model::GetRandomChains(int numOfChains)
    {
        std::vector<std::vector<int>> chains;
        chains.reserve(numOfChains);
        int chainCount = 0;
        std::vector<int> sourceIds = FindSourceTaskIds();
        std::vector<int> sinkIds = FindSinkTaskIds();

        for (int sourceId : sourceIds)
        {
            for (int sinkId : sinkIds)
            {
                auto path = shortest_paths(sourceId, sinkId, graph_);
                if (path.size() > 1)
                {
                    chains.push_back(path);
                    chainCount++;
                }
            }
        }
        if (chainCount > numOfChains)
        {
            if (GlobalVariablesDAGOpt::whether_shuffle_CE_chain)
                std::shuffle(chains.begin(), chains.end(), std::default_random_engine{});
            chains.resize(numOfChains);
        }
        return chains;
    }

    DAG_Model ReadDAG_Tasks(std::string path, std::string priorityType, int chainNum)
    {
        using namespace std;
        TaskSet tasks = ReadTaskSet(path, priorityType);
        // some default parameters in this function
        std::string delimiter = ",";
        std::string token;
        std::string line;
        size_t pos = 0;

        MAP_Prev mapPrev;

        fstream file;
        file.open(path, ios::in);
        if (file.is_open())
        {
            std::string line;
            double sf_bound = 0;
            double rtda_bound = 0;
            while (getline(file, line))
            {
                if (line[0] != '*' && line[0] != '@')
                    continue;
                if (line[0] == '*')
                {
                    line = line.substr(1, int(line.size()) - 1);
                    std::vector<int> dataInLine;
                    while ((pos = line.find(delimiter)) != std::string::npos)
                    {
                        token = line.substr(0, pos);
                        int temp = atoi(token.c_str());
                        dataInLine.push_back(temp);
                        line.erase(0, pos + delimiter.length());
                    }
                    dataInLine.push_back(atoi(line.c_str()));
                    mapPrev[dataInLine[1]].push_back(tasks[dataInLine[0]]);
                }
                else if (line[0] == '@')
                {
                    if ((pos = line.find(":")) != std::string::npos)
                    {
                        token = line.substr(pos + 1);
                        if (line.find("SF_Bound") != std::string::npos)
                        {
                            sf_bound = atoi(token.c_str());
                        }
                        else if (line.find("RTDA_Bound") != std::string::npos)
                        {
                            rtda_bound = atoi(token.c_str());
                        }
                    }
                }
            }

            DAG_Model ttt(tasks, mapPrev, sf_bound, rtda_bound, chainNum);
            return ttt;
        }
        else
        {
            std::cout << Color::red << "The path does not exist in ReadTaskSet!" << std::endl
                      << path
                      << Color::def << std::endl;
            throw;
        }
    }
}