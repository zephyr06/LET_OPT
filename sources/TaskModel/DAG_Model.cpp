#include "sources/TaskModel/DAG_Model.h"

namespace DAG_SPACE {

std::vector<int> shortest_paths(Vertex root, Vertex target, Graph const& g) {
  std::vector<int> path;
  // find shortest paths from the root
  std::vector<Vertex> predecessors(boost::num_vertices(g), NIL);
  auto recorder =
      boost::record_predecessors(predecessors.data(), boost::on_examine_edge());
  boost::breadth_first_search(
      g, root, boost::visitor(boost::make_bfs_visitor(recorder)));

  for (auto pred = predecessors[target]; pred != NIL;
       pred = predecessors[pred]) {
    path.push_back(pred);
  }
  if (path.size() != 0) {
    std::reverse(path.begin(), path.end());
    path.push_back(target);
  }
  return path;
}

void PrintChains(const std::vector<std::vector<int>>& chains) {
  std::cout << "Chains:" << std::endl;
  for (size_t i = 0; i < size(chains); i++) {
    for (size_t j = 0; j < size(chains[i]); j++) {
      std::cout << chains[i][j] << ", ";
    }
    std::cout << std::endl;
  }
}

// *2, 1 means task 2 depend on task 1, or task 1 must execute before task 2;
// 1 would be the first in MAP_Prev, while 2 is one task in TaskSet
// MAP_Prev maps one task to all the tasks it depends on
using namespace RegularTaskSystem;

std::pair<Graph, indexVertexMap> DAG_Model::GenerateGraphForTaskSet() const {
  Graph g;
  // map to access properties of vertex from the graph
  vertex_name_map_t vertex2indexBig = get(boost::vertex_name, g);

  // map to access vertex from its global index
  indexVertexMap indexesBGL;
  for (uint i = 0; i < tasks.size(); i++) {
    indexVertexMap::iterator pos;
    bool inserted;
    Vertex u;
    boost::tie(pos, inserted) = indexesBGL.insert(std::make_pair(i, Vertex()));
    if (inserted) {
      u = add_vertex(g);
      vertex2indexBig[u] = i;
      pos->second = u;
    } else {
      CoutError("Error building indexVertexMap!");
    }
  }

  // add edges

  for (auto itr = mapPrev.begin(); itr != mapPrev.end(); itr++) {
    const TaskSet& tasksPrev = itr->second;  // task ids that must finish before
                                             // the task whose id is indexNext
    size_t indexNext = itr->first;
    for (size_t i = 0; i < tasksPrev.size(); i++) {
      boost::add_edge(tasksPrev[i].id, GetTask(indexNext).id, g);
    }
  }
  return std::make_pair(g, indexesBGL);
}

void DAG_Model::print() {
  for (auto& task : tasks) task.print();
  for (auto itr = mapPrev.begin(); itr != mapPrev.end(); itr++) {
    for (uint i = 0; i < (itr->second).size(); i++)
      std::cout << "Edge: " << ((itr->second)[i].id) << "-->" << (itr->first)
                << std::endl;
  }
}

void DAG_Model::printChains() {
  for (size_t i = 0; i < chains_.size(); i++) {
    std::cout << "Chain #" << i << ": ";
    for (auto task : chains_[i]) {
      std::cout << task << ", ";
    }
    std::cout << std::endl;
  }
}

int DAG_Model::edgeNumber() {
  int count = 0;
  for (auto itr = mapPrev.begin(); itr != mapPrev.end(); itr++) {
    count += (itr->second).size();
  }
  return count;
}

std::vector<int> DAG_Model::FindSourceTaskIds() const {
  std::set<int> originTasks;
  for (uint i = 0; i < tasks.size(); i++) {
    originTasks.insert(i);
  }

  for (auto itr = mapPrev.begin(); itr != mapPrev.end(); itr++) {
    size_t indexNext = itr->first;
    originTasks.erase(indexNext);
  }
  std::vector<int> res(originTasks.size());
  std::copy(originTasks.begin(), originTasks.end(), res.begin());
  return res;
}
std::vector<int> DAG_Model::FindSinkTaskIds() const {
  std::set<int> originTasks;
  for (uint i = 0; i < tasks.size(); i++) {
    originTasks.insert(i);
  }

  for (auto itr = mapPrev.begin(); itr != mapPrev.end(); itr++) {
    auto parents = itr->second;
    for (auto p : parents) {
      originTasks.erase(p.id);
    }
  }
  std::vector<int> res(originTasks.size());
  std::copy(originTasks.begin(), originTasks.end(), res.begin());
  return res;
}

std::vector<std::vector<int>> DAG_Model::GetRandomChains(int numOfChains,
                                                         int chain_length) {
  std::vector<std::vector<int>> chains;
  if (numOfChains <= 0) return chains;
  chains.reserve(numOfChains);
  int chainCount = 0;
  auto rng = std::default_random_engine{};
  std::vector<int> sourceIds = FindSourceTaskIds();
  std::vector<int> sinkIds = FindSinkTaskIds();

  std::shuffle(std::begin(sourceIds), std::end(sourceIds), rng);
  std::shuffle(std::begin(sinkIds), std::end(sinkIds), rng);

  for (int sourceId : sourceIds) {
    for (int sinkId : sinkIds) {
      if (chainCount >= numOfChains) break;
      auto path = shortest_paths(sourceId, sinkId, graph_);
      if (path.size() > 1) {
        if (chain_length == 0 ||
            (chain_length > 0 && path.size() >= chain_length))
          chains.push_back(path);
        chainCount++;
      }
    }
  }
  return chains;
}

void DAG_Model::CategorizeTaskSet() {
  for (uint i = 0; i < tasks.size(); i++) {
    int task_id = tasks[i].id;
    int p_id = tasks[i].processorId;
    auto itr = processor2taskset_.find(p_id);
    if (itr == processor2taskset_.end()) {
      processor2taskset_[p_id] = {tasks[i]};
    } else {
      processor2taskset_[p_id].push_back(tasks[i]);
    }
  }
  SortTaskSetForEachProcessor();
}
void DAG_Model::SortTaskSetForEachProcessor() {
  for (auto itr = processor2taskset_.begin(); itr != processor2taskset_.end();
       itr++) {
    TaskSet& tasks = itr->second;
    std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
      return a.priority() > b.priority();
    });
    for (uint i = 0; i < tasks.size(); i++) {
      task_id2task_index_within_processor_[tasks[i].id] = i;
    }
  }
}

void DAG_Model::RecordTaskPosition() {
  for (int i = 0; i < static_cast<int>(tasks.size()); i++) {
    task_id2position_[tasks[i].id] = i;
  }
}

std::vector<SF_Fork> DAG_Model::GetRandomForks(int num_fork,
                                               int fork_sensor_num_min,
                                               int fork_sensor_num_max) {
  std::vector<SF_Fork> res;
  if (num_fork <= 0) return res;
  res.reserve(mapPrev.size());
  for (const auto& [sink, source_tasks] : mapPrev) {
    if (source_tasks.size() > 1 && source_tasks.size() >= fork_sensor_num_min &&
        source_tasks.size() <= fork_sensor_num_max) {
      res.push_back(SF_Fork(GetIDVec(source_tasks), sink));
    }
  }
  auto rng = std::default_random_engine{};
  std::shuffle(std::begin(res), std::end(res), rng);
  if (num_fork < res.size()) res.resize(num_fork);
  return res;
}

int DAG_Model::getSF_Fork_InstanceCount(
    const TaskSetInfoDerived& tasks_info) const {
  int count = 0;
  for (uint i = 0; i < sf_forks_.size(); i++) {
    count +=
        tasks_info.hyper_period / tasks_info.GetTask(sf_forks_[i].sink).period;
  }
  return count;
}
int DAG_Model::getSFInstance(int fork_index, int sink_job_index,
                             const TaskSetInfoDerived& tasks_info) const {
  int count = 0;
  for (uint i = 0; i < fork_index; i++) {
    count +=
        tasks_info.hyper_period / tasks_info.GetTask(sf_forks_[i].sink).period;
  }
  return count + sink_job_index;
}

// transform a string to a vectof of int
std::vector<int> Str2VecInt(const std::string& str) {
  std::vector<int> vect;
  std::stringstream ss(str);
  for (int i; ss >> i;) {
    vect.push_back(i);
    if (ss.peek() == ',') ss.ignore();
  }
  return vect;
}

DAG_Model ReadDAG_Tasks(std::string path, std::string priorityType,
                        int chainNum) {  // , int fork_num
  using namespace std;
  TaskSet tasks = ReadTaskSet(path, priorityType);

  std::unordered_map<int, int> task_id2position;
  for (int i = 0; i < static_cast<int>(tasks.size()); i++) {
    task_id2position[tasks[i].id] = i;
  }
  // some default parameters in this function
  std::string delimiter = ",";
  std::string token;
  std::string line;
  size_t pos = 0;

  MAP_Prev mapPrev;

  std::vector<std::vector<int>> chains;
  std::vector<SF_Fork> sf_forks;
  SF_Fork fork_temp;  // for read purpose

  fstream file;
  file.open(path, ios::in);
  if (file.is_open()) {
    std::string line;
    while (getline(file, line)) {
      if (line[0] != '*' && line[0] != '@') continue;
      if (line[0] == '*') {
        line = line.substr(1, int(line.size()) - 1);
        std::vector<int> dataInLine;
        while ((pos = line.find(delimiter)) != std::string::npos) {
          token = line.substr(0, pos);
          int temp = atoi(token.c_str());
          dataInLine.push_back(temp);
          line.erase(0, pos + delimiter.length());
        }
        dataInLine.push_back(atoi(line.c_str()));
        mapPrev[dataInLine[1]].push_back(
            tasks[task_id2position[dataInLine[0]]]);
      } else if (line[0] == '@') {
        if (line.find("Chain:") != std::string::npos) {
          int name_length = 7;
          chains.push_back(Str2VecInt(
              line.substr(name_length, line.size() - name_length - 1)));
        } else if (line.find("Fork_source:") != std::string::npos) {
          int name_length = 13;
          fork_temp.source = Str2VecInt(
              line.substr(name_length, line.size() - name_length - 1));
        } else if (line.find("Fork_sink:") != std::string::npos) {
          int name_length = 11;
          fork_temp.sink = Str2VecInt(
              line.substr(name_length, line.size() - name_length - 1))[0];
          sf_forks.push_back(fork_temp);
        }
      }
    }

    DAG_Model ttt(tasks, mapPrev, chainNum);
    if (chains.size() > 0) ttt.chains_ = chains;
    if (chainNum < chains.size()) ttt.chains_.resize(chainNum);
    ttt.sf_forks_ = sf_forks;
    return ttt;
  } else {
    std::cout << Color::red << "The path does not exist in ReadTaskSet!"
              << std::endl
              << path << Color::def << std::endl;
    throw;
  }
}

bool WhetherDAGChainsShareNodes(const DAG_Model& dag_tasks) {
  const std::vector<std::vector<int>>& chains = dag_tasks.chains_;
  for (const auto& chain_curr : chains) {
    std::unordered_set<int> record_curr;
    record_curr.reserve(chain_curr.size());
    for (int x : chain_curr) record_curr.insert(x);
    for (const auto& chain_compare : chains) {
      if (chain_curr != chain_compare) {
        for (int y : chain_compare)
          if (record_curr.find(y) != record_curr.end()) return true;
      }
    }
  }
  return false;
}

}  // namespace DAG_SPACE