
#include "sources/Optimization/TaskSetPermutation.h"

namespace DAG_SPACE {

VariableOD FindODFromPermutation(const DAG_Model& dag_tasks,
                                 const ChainPermutation& chain_perm,
                                 std::vector<int> task_id_chain) {
    const TaskSet& tasks = dag_tasks.GetTaskSet();
    VariableOD variable(dag_tasks.GetTaskSet());

    // in each iteration, we consider task i's virtual deadline and task i+1's
    // virtual offset assignment
    for (uint chain_index = 0; chain_index < task_id_chain.size() - 1;
         chain_index++) {
        int task_id = task_id_chain[chain_index];
        bool schedulable = variable.SetMinDeadline(task_id, dag_tasks);
        if (!schedulable) break;
        if (chain_perm[chain_index].inequality_.upper_bound_valid_)
            variable.SetOffset(
                task_id_chain[chain_index + 1],
                variable[task_id].deadline -
                    chain_perm[chain_index].inequality_.upper_bound_);
    }
    variable.SetMinDeadline(task_id_chain.back(), dag_tasks);
    return variable;
}
}  // namespace DAG_SPACE