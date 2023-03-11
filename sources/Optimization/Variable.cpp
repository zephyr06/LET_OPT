
#include "sources/Optimization/Variable.h"
namespace DAG_SPACE {

VariableOD FindODFromPermutation(const TaskSetInfoDerived& tasks_info,
                                 const ChainPermutation& chain_perm,
                                 std::vector<int> task_chain) {
    const TaskSet& tasks = tasks_info.tasks;
    VariableOD variable(tasks_info.tasks);
    std::vector<int> rta_vec =
        GetResponseTimeOfChain(tasks_info.tasks, task_chain);

    // in each iteration, we consider task i's virtual deadline and task i+1's
    // virtual offset assignment
    for (uint chain_index = 0; chain_index < task_chain.size() - 1;
         chain_index++) {
        int task_index = task_chain[chain_index];
        bool schedulable = variable.SetMinDeadline(task_index, tasks);
        if (!schedulable) break;

        if (chain_perm[chain_index].inequality_.upper_bound_valid_)
            variable.SetOffset(
                task_chain[chain_index + 1],
                variable[task_index].deadline -
                    chain_perm[chain_index].inequality_.upper_bound_);
    }
    variable.SetMinDeadline(task_chain.back(), tasks);
    return variable;
}

VariableRange FindVariableRange(const TaskSet& tasks) {
    VariableOD upper_bound(tasks);
    VariableOD lower_bound(tasks);
    for (uint i = 0; i < tasks.size(); i++) {
        int rta = GetResponseTime(tasks, i);
        lower_bound.variables_[i].deadline = rta;
        upper_bound.variables_[i].offset = tasks[i].deadline - rta;
    }
    return VariableRange{lower_bound, upper_bound};
}

}  // namespace DAG_SPACE