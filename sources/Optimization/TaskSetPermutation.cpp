
#include "sources/Optimization/TaskSetPermutation.h"

namespace DAG_SPACE {

std::vector<std::vector<int>> GetSubChains(
    const std::vector<std::vector<int>>& chains_full_length,
    const ChainsPermutation& chains_perm) {
    std::vector<Edge> edges = chains_perm.GetEdges();
    std::unordered_set<Edge> edge_record;
    edge_record.reserve(edges.size());
    for (const auto& edge : edges) edge_record.insert(edge);
    std::vector<std::vector<int>> sub_chains;
    sub_chains.reserve(chains_full_length.size());
    for (uint i = 0; i < chains_full_length.size(); i++) {
        const auto& chain = chains_full_length[i];
        for (uint j = 0; j < chain.size() - 1; j++) {
            Edge edge_curr(chain[j], chain[j + 1]);
            if (edge_record.find(edge_curr) == edge_record.end())
                break;
            else {
                if (j == 0) {
                    sub_chains.push_back({chain[j], chain[j + 1]});
                } else {
                    // sub_chains.end()->push_back(chain[j]);
                    sub_chains.back().push_back(chain[j + 1]);
                }
            }
        }
    }

    return sub_chains;
}

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
// void SetVariableHelperSingleChain(
//     int task_id, std::unordered_map<int, bool>& variable_set_record,
//     VariableOD& variable, const ChainsPermutation& chain_perm,
//     const GraphOfChains& graph_of_all_ca_chains, const DAG_Model& dag_tasks,
//     const std::unordered_set<Edge>& sub_chain_edge_record) {
//     if (variable_set_record.find(task_id) == variable_set_record.end() ||
//         variable_set_record[task_id] == false) {
//         if (!variable.valid_) return;
//         int offset_min = 0;
//         if (graph_of_all_ca_chains.prev_tasks_.find(task_id) !=
//             graph_of_all_ca_chains.prev_tasks_.end()) {
//             const std::vector<int>& dependent_tasks =
//                 graph_of_all_ca_chains.prev_tasks_.at(task_id);
//             for (int dependent_task : dependent_tasks) {
//                 SetVariableHelperSingleChain(
//                     dependent_task, variable_set_record, variable,
//                     chain_perm, graph_of_all_ca_chains, dag_tasks,
//                     sub_chain_edge_record);
//                 Edge edge_curr(dependent_task, task_id);
//                 offset_min = std::max(
//                     offset_min,
//                     variable[dependent_task].deadline -
//                         chain_perm[edge_curr].inequality_.upper_bound_);
//             }
//         }
//         variable[task_id].offset = offset_min;
//         variable[task_id].deadline =
//             offset_min + GetResponseTime(dag_tasks, task_id);
//         // exam whether the current offset assignment violates prev tasks'
//         // lower bound constraints
//         if (graph_of_all_ca_chains.prev_tasks_.find(task_id) !=
//             graph_of_all_ca_chains.prev_tasks_.end()) {
//             const std::vector<int>& dependent_tasks =
//                 graph_of_all_ca_chains.prev_tasks_.at(task_id);
//             for (int dependent_task : dependent_tasks) {
//                 SetVariableHelperSingleChain(
//                     dependent_task, variable_set_record, variable,
//                     chain_perm, graph_of_all_ca_chains, dag_tasks,
//                     sub_chain_edge_record);
//                 Edge edge_curr(dependent_task, task_id);
//                 const PermutationInequality& ineq =
//                     chain_perm[edge_curr].inequality_;
//                 if (variable[task_id].offset + ineq.lower_bound_ >=
//                     variable[dependent_task].deadline) {
//                     variable[dependent_task].deadline =
//                         variable[task_id].offset + ineq.lower_bound_ + 1;
//                     if (variable[dependent_task].deadline >
//                         dag_tasks.GetTask(dependent_task).deadline)
//                         variable.valid_ = false;
//                 }
//             }
//         }
//         if (variable[task_id].deadline > dag_tasks.GetTask(task_id).deadline)
//         {
//             variable.valid_ = false;
//         }
//         variable_set_record[task_id] = true;
//     } else
//         return;
// }

bool ExamVariableFeasibility(const VariableOD& variable,
                             const ChainsPermutation& chain_perm,
                             const GraphOfChains& graph_of_all_ca_chains,
                             const DAG_Model& dag_task,
                             const std::vector<int>& chain) {
    for (uint i = 0; i < chain.size() - 1; i++) {
        Edge edge(chain[i], chain[i + 1]);
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
    for (uint i = 0; i < variable.size(); i++) {
        if (variable.at(i).deadline > dag_task.GetTask(i).deadline)
            return false;
    }
    return true;
}

//      o_{task_next_id} + lower_bound < d_{task_prev_id} ;
//      d_{task_prev_id} <= o_{task_next_id} + upper_bound ;
//  const std::vector<int>& rta
void SetVariableHelperSingleEdge(const Edge& edge, VariableOD& variable,
                                 const PermutationInequality& ineq,
                                 const DAG_Model& dag_tasks) {
    //  edge.from_id, edge_to_id
    variable.SetMinDeadline(edge.from_id, dag_tasks);
    variable.SetOffset(edge.to_id,
                       variable[edge.from_id].deadline - ineq.upper_bound_);
    if (variable[edge.from_id].deadline - ineq.upper_bound_ < 0) {
        variable[edge.to_id].offset = 0;
        variable[edge.from_id].deadline =
            std::max(variable[edge.from_id].deadline,
                     variable[edge.to_id].offset + ineq.lower_bound_ + 1);
        if (variable[edge.from_id].deadline >
            dag_tasks.GetTask(edge.from_id).deadline) {
            variable.valid_ = false;
            return;
        }
    }
    if (variable[edge.to_id].offset + GetResponseTime(dag_tasks, edge.to_id) >
        dag_tasks.GetTask(edge.to_id).deadline) {
        variable.valid_ = false;
        return;
    }
}

VariableOD FindODFromSingleChainPermutation(
    const DAG_Model& dag_tasks, const ChainsPermutation& chain_perm,
    const GraphOfChains& graph_of_all_ca_chains,
    const std::vector<int>& chain) {
    const TaskSet& tasks = dag_tasks.GetTaskSet();
    VariableOD variable(tasks);

    std::unordered_set<Edge> sub_chain_edge_record;
    for (uint i = 0; i < chain.size() - 1; i++) {
        Edge edge(chain[i], chain[i + 1]);
        sub_chain_edge_record.insert(edge);
    }

    for (uint i = 0; i < chain.size() - 1; i++) {
        Edge edge(chain[i], chain[i + 1]);
        const SinglePairPermutation& single_perm = chain_perm[edge];
        SetVariableHelperSingleEdge(edge, variable, single_perm.inequality_,
                                    dag_tasks);
        if (!variable.valid_) break;
    }
    if (variable.valid_)
        variable.valid_ = ExamVariableFeasibility(
            variable, chain_perm, graph_of_all_ca_chains, dag_tasks, chain);
    return variable;
}

}  // namespace DAG_SPACE