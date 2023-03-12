
#include "sources/Optimization/Variable.h"
namespace DAG_SPACE {

VariableRange FindVariableRange(const DAG_Model& dag_tasks) {
    const TaskSet& tasks = dag_tasks.GetTaskSet();
    VariableOD upper_bound(tasks);
    VariableOD lower_bound(tasks);
    for (uint i = 0; i < tasks.size(); i++) {
        int rta =
            GetResponseTime(dag_tasks,
                            tasks[i].id);  // TODO: exam schedulability there
        lower_bound.variables_[i].deadline = rta;
        upper_bound.variables_[i].offset = tasks[i].deadline - rta;
    }
    return VariableRange{lower_bound, upper_bound};
}

}  // namespace DAG_SPACE