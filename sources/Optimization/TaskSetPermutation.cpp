
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
          FindBestPossibleVariableOD(dag_tasks_, tasks_info_, rta_)),
      lp_optimizer_(dag_tasks_, tasks_info_, graph_of_all_ca_chains_, "",
                    rta_) {
  adjacent_two_task_permutations_.reserve(
      1e2);  // there are never more than 1e2 edges
  // lp_optimizer_.AddVariablesOD();
  // lp_optimizer_.AddSchedulabilityConstraints();
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

}  // namespace DAG_SPACE