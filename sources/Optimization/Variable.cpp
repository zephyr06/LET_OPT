
#include "sources/Optimization/Variable.h"
namespace DAG_SPACE {

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