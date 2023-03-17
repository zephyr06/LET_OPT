
#include "sources/Optimization/TaskSetPermutation.h"

namespace DAG_SPACE {

// VariableOD FindODFromPermutation(const DAG_Model& dag_tasks,
//                                  const ChainsPermutation& chain_perm,
//                                  std::vector<int> task_id_chain) {
//     const TaskSet& tasks = dag_tasks.GetTaskSet();
//     VariableOD variable(dag_tasks.GetTaskSet());

//     // in each iteration, we consider task i's virtual deadline and task
//     i+1's
//     // virtual offset assignment
//     for (uint chain_index = 0; chain_index < task_id_chain.size() - 1;
//          chain_index++) {
//         int task_id = task_id_chain[chain_index];
//         bool schedulable = variable.SetMinDeadline(task_id, dag_tasks);
//         if (!schedulable) break;
//         if (chain_perm[chain_index].inequality_.upper_bound_valid_)
//             variable.SetOffset(
//                 task_id_chain[chain_index + 1],
//                 variable[task_id].deadline -
//                     chain_perm[chain_index].inequality_.upper_bound_);
//     }
//     variable.SetMinDeadline(task_id_chain.back(), dag_tasks);
//     return variable;
// }

// set each variable's offset and deadline
// assumes acyclic graph
void SetVariableHelper(int task_id,
                       std::unordered_map<int, bool>& variable_set_record,
                       VariableOD& variable, const ChainsPermutation& chain_perm,
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

    return variable;
}
}  // namespace DAG_SPACE