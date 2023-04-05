
#include "sources/Optimization/TaskSetPermutation.h"

namespace DAG_SPACE {

TaskSetPermutation::TaskSetPermutation(
    const DAG_Model& dag_tasks, const std::vector<std::vector<int>>& chains)
    : start_time_((std::chrono::high_resolution_clock::now())),
      dag_tasks_(dag_tasks),
      tasks_info_(
          RegularTaskSystem::TaskSetInfoDerived(dag_tasks.GetTaskSet())),
      graph_of_all_ca_chains_(chains),
      best_yet_obj_(1e9),
      iteration_count_(0),
      variable_range_od_(FindVariableRange(dag_tasks_)),
      rta_(GetResponseTimeTaskSet(dag_tasks_)),
      best_possible_variable_od_(
          FindBestPossibleVariableOD(dag_tasks_, tasks_info_, rta_)) {
  adjacent_two_task_permutations_.reserve(
      1e2);  // there are never more than 1e2 edges
  FindPairPermutations();
  feasible_chains_ =
      FeasiblieChainsManagerVec(adjacent_two_task_permutations_.size());
}

void TaskSetPermutation::FindPairPermutations() {
  int single_perm_count = 0;
  for (const auto& edge_curr : graph_of_all_ca_chains_.edge_vec_ordered_) {
    if (ifTimeout(start_time_)) break;
    adjacent_two_task_permutations_.push_back(
        TwoTaskPermutations(edge_curr.from_id, edge_curr.to_id, dag_tasks_,
                            tasks_info_, rta_, single_perm_count));
    single_perm_count +=
        adjacent_two_task_permutations_.back().single_permutations_.size();
    std::cout << "Pair permutation #: "
              << adjacent_two_task_permutations_.back().size() << "\n";
  }
}
bool TaskSetPermutation::ExamSchedulabilityOptSol() const {
  if (best_yet_variable_od_.size() == 0) return false;
  for (int i = 0; i < tasks_info_.N; i++) {
    int offset = best_yet_variable_od_.at(i).offset;
    int deadline = best_yet_variable_od_.at(i).deadline;
    int rta = GetResponseTime(dag_tasks_, i);
    if (rta + offset > deadline || deadline > dag_tasks_.GetTask(i).deadline)
      return false;
  }
  if (GlobalVariablesDAGOpt::debugMode == 1) {
    best_yet_variable_od_.print();
  }
  return true;
}

void TaskSetPermutation::PrintFeasibleChainsRecord() const {
  std::vector<Edge> edges = GetAllEdges();
  int count = 0;
  for (const auto& feasible_chain_man : feasible_chains_.chain_man_vec_) {
    std::cout << "\n****************************\n";
    std::cout << "Feasible chain index: " << count++ << "\n";
    const ChainsPermutation& chains_perm = feasible_chain_man.feasible_chain_;
    PrintSinglePermIndex(chains_perm, edges);
  }

  std::cout << "\n****************************\n";
}
std::vector<Edge> TaskSetPermutation::GetAllEdges() const {
  std::vector<Edge> edges;
  edges.reserve(adjacent_two_task_permutations_.size());
  for (uint i = 0; i < adjacent_two_task_permutations_.size(); i++)
    edges.push_back(adjacent_two_task_permutations_[i].GetEdge());
  return edges;
}

void TaskSetPermutation::PrintSinglePermIndex(
    const ChainsPermutation& chains_perm) const {
  auto edges = GetAllEdges();
  for (auto edge : edges)
    if (chains_perm.exist(edge))
      std::cout << chains_perm[edge]->index_local_ << ", ";
}

void TaskSetPermutation::PrintSinglePermIndex(
    const ChainsPermutation& chains_perm,
    const std::vector<Edge>& edges) const {
  for (auto edge : edges)
    if (chains_perm.exist(edge))
      std::cout << chains_perm[edge]->index_local_ << ", ";
}

std::vector<Edge> TaskSetPermutation::GetUnvisitedFutureEdges(
    uint position) const {
  std::vector<Edge> edges;
  edges.reserve(adjacent_two_task_permutations_.size());
  for (uint i = position + 1; i < adjacent_two_task_permutations_.size(); i++) {
    edges.push_back(adjacent_two_task_permutations_.at(i).GetEdge());
  }
  return edges;
}
}  // namespace DAG_SPACE