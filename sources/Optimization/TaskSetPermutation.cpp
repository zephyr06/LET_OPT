
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
      variable_range_od_(FindVariableRange(dag_tasks_)) {
    adjacent_two_task_permutations_.reserve(
        1e2);  // there are never more than 1e2 edges
    FindPairPermutations();
    chain_permutations_.reserve(1e5);
}

void TaskSetPermutation::FindPairPermutations() {
    for (const auto& edge_curr : graph_of_all_ca_chains_.edge_vec_ordered_) {
        if (ifTimeout(start_time_)) break;
        adjacent_two_task_permutations_.push_back(TwoTaskPermutations(
            edge_curr.from_id, edge_curr.to_id, dag_tasks_, tasks_info_));
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
        if (rta + offset > deadline ||
            deadline > dag_tasks_.GetTask(i).deadline)
            return false;
    }
    if (GlobalVariablesDAGOpt::debugMode == 1) {
        best_yet_variable_od_.print();
    }
    return true;
}

// set each variable's offset and deadline
// assumes acyclic graph
void SetVariableHelper(int task_id,
                       std::unordered_map<int, bool>& variable_set_record,
                       VariableOD& variable,
                       const ChainsPermutation& chain_perm,
                       const GraphOfChains& graph_of_all_ca_chains,
                       const DAG_Model& dag_tasks) {
    if (variable_set_record.find(task_id) == variable_set_record.end() ||
        variable_set_record[task_id] == false) {
        if (!variable.valid_) return;
        int offset_min = 0;
        if (graph_of_all_ca_chains.prev_tasks_.find(task_id) !=
            graph_of_all_ca_chains.prev_tasks_.end()) {
            const std::vector<int>& dependent_tasks =
                graph_of_all_ca_chains.prev_tasks_.at(task_id);
            for (int dependent_task : dependent_tasks) {
                SetVariableHelper(dependent_task, variable_set_record, variable,
                                  chain_perm, graph_of_all_ca_chains,
                                  dag_tasks);
                Edge edge_curr(dependent_task, task_id);
                offset_min = std::max(
                    offset_min,
                    variable[dependent_task].deadline -
                        chain_perm[edge_curr].inequality_.upper_bound_);
            }
        }
        variable[task_id].offset = offset_min;
        variable[task_id].deadline =
            offset_min + GetResponseTime(dag_tasks, task_id);
        if (variable[task_id].deadline > dag_tasks.GetTask(task_id).deadline) {
            variable.valid_ = false;
        }
        variable_set_record[task_id] = true;
    } else
        return;
}

bool ExamVariableFeasibility(const VariableOD& variable,
                             const ChainsPermutation& chain_perm,
                             const GraphOfChains& graph_of_all_ca_chains) {
    for (const auto& edge : graph_of_all_ca_chains.edge_vec_ordered_) {
        const PermutationInequality& inequality = chain_perm[edge].inequality_;
        int prev_id = inequality.task_prev_id_;
        int next_id = inequality.task_next_id_;
        if (inequality.lower_bound_valid_) {
            if (variable.at(next_id).offset + inequality.lower_bound_ >=
                    variable.at(prev_id).deadline ||
                variable.at(prev_id).deadline >
                    variable.at(next_id).offset + inequality.upper_bound_) {
                if (GlobalVariablesDAGOpt::debugMode) inequality.print();
                return false;
            }
        }
    }
    return true;
}

VariableOD FindODFromPermutation(const DAG_Model& dag_tasks,
                                 const ChainsPermutation& chain_perm,
                                 const GraphOfChains& graph_of_all_ca_chains) {
    const TaskSet& tasks = dag_tasks.GetTaskSet();
    VariableOD variable(dag_tasks.GetTaskSet());
    std::unordered_map<int, bool> variable_set_record(1e2);

    for (const auto& edge : graph_of_all_ca_chains.edge_vec_ordered_) {
        const SinglePairPermutation& single_perm = chain_perm[edge];
        SetVariableHelper(single_perm.GetNextTaskId(), variable_set_record,
                          variable, chain_perm, graph_of_all_ca_chains,
                          dag_tasks);
    }
    if (variable.valid_)
        variable.valid_ = ExamVariableFeasibility(variable, chain_perm,
                                                  graph_of_all_ca_chains);
    return variable;
}

}  // namespace DAG_SPACE