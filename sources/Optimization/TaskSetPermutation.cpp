
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
      rta_(GetResponseTimeTaskSet(dag_tasks_)) {
  adjacent_two_task_permutations_.reserve(
      1e2);  // there are never more than 1e2 edges
}

TaskSetPermutation::TaskSetPermutation(
    const DAG_Model& dag_tasks, const std::vector<std::vector<int>>& chains,
    const std::string& type_trait)
    : TaskSetPermutation(dag_tasks, chains) {
  type_trait_ = type_trait;
  FindPairPermutations();
}

void TaskSetPermutation::FindPairPermutations() {
  int single_perm_count = 0;
  for (const auto& edge_curr : graph_of_all_ca_chains_.edge_vec_ordered_) {
    if (ifTimeout(start_time_)) break;
    adjacent_two_task_permutations_.push_back(
        TwoTaskPermutations(edge_curr.from_id, edge_curr.to_id, dag_tasks_,
                            tasks_info_, rta_, type_trait_, single_perm_count));
    single_perm_count +=
        adjacent_two_task_permutations_.back().single_permutations_.size();
    std::cout << "Pair permutation #: "
              << adjacent_two_task_permutations_.back().size() << "\n";
  }
}

bool ExamSchedulabilityVariable(const VariableOD& variable_od,
                                const DAG_Model& dag_tasks,
                                const TaskSetInfoDerived& tasks_info) {
  if (variable_od.size() == 0) return false;
  for (int i = 0; i < tasks_info.N; i++) {
    int offset = variable_od.at(i).offset;
    int deadline = variable_od.at(i).deadline;
    int rta = GetResponseTime(dag_tasks, i);
    if (rta + offset > deadline || deadline > dag_tasks.GetTask(i).deadline)
      return false;
  }
  if (GlobalVariablesDAGOpt::debugMode == 1) {
    variable_od.print();
  }
  return true;
}

bool TaskSetPermutation::ExamSchedulabilityOptSol() const {
  return ExamSchedulabilityVariable(best_yet_variable_od_, dag_tasks_,
                                    tasks_info_);
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

// bool TaskSetPermutation::WhetherContainInfeasibleSubChains(
//     const ChainsPermutation& chains_perm,
//     const std::vector<std::vector<int>>& sub_chains) {
//   for (const auto& sub_chain : sub_chains) {
//     if (sub_chain.size() == 0) continue;
//     if (GlobalVariablesDAGOpt::SKIP_PERM >= 2 &&
//         !FindODFromSingleChainPermutation(
//              dag_tasks_, chains_perm, graph_of_all_ca_chains_, sub_chain,
//              rta_) .valid_) {
//       if (GlobalVariablesDAGOpt::debugMode) {
//         std::cout << "Early break at level " << chains_perm.size() << ": ";
//         PrintSinglePermIndex(chains_perm);
//         std::cout
//             << " due to being conflicted permutations from sub-chains while "
//                "exploring the "
//             // << adjacent_two_task_permutations_[position].size() -
//             //        iterator.size()
//             << " permutations\n";
//         std::cout << "\n";
//       }
//       return true;
//     }
//   }
//   return false;
// }
}  // namespace DAG_SPACE