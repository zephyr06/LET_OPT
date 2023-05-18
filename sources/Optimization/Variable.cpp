
#include "sources/Optimization/Variable.h"
namespace DAG_SPACE {

VariableRange FindVariableRange(const DAG_Model& dag_tasks) {
  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  return FindVariableRange(dag_tasks, rta);
}

VariableRange FindVariableRange(const DAG_Model& dag_tasks,
                                const std::vector<int>& rta) {
  const TaskSet& tasks = dag_tasks.GetTaskSet();
  VariableOD upper_bound(tasks);
  VariableOD lower_bound(tasks);
  for (uint i = 0; i < tasks.size(); i++) {
    lower_bound[tasks[i].id].deadline = rta[tasks[i].id];
    upper_bound[tasks[i].id].offset = tasks[i].deadline - rta[tasks[i].id];
  }
  return VariableRange{lower_bound, upper_bound};
}
VariableRange FindVariableRangeWithRTA(const DAG_Model& dag_tasks) {
  const TaskSet& tasks = dag_tasks.GetTaskSet();
  VariableOD upper_bound(tasks);
  VariableOD lower_bound(tasks);
  for (uint i = 0; i < tasks.size(); i++) {
    int period_curr = dag_tasks.GetTask(tasks[i].id).period;
    lower_bound[tasks[i].id].offset = 0;
    lower_bound[tasks[i].id].deadline =
        lower_bound[tasks[i].id].offset + period_curr;

    upper_bound[tasks[i].id].offset = period_curr - 1;
    upper_bound[tasks[i].id].deadline =
        upper_bound[tasks[i].id].offset + period_curr;
  }
  return VariableRange{lower_bound, upper_bound};
}

void VariableOD::print() const {
  for (int i = 0; i < variables_.size(); i++) {
    std::cout << std::setfill(' ') << std::setw(5);
    std::cout << "Task " << i << " offset: " << variables_.at(i).offset
              << " deadline: " << variables_.at(i).deadline << "\n";
  }
}

VariableOD FindBestPossibleVariableOD(const DAG_Model& dag_tasks,
                                      const TaskSetInfoDerived& tasks_info,
                                      const std::vector<int>& rta) {
  VariableOD variable(dag_tasks.GetTaskSet());
  for (int i = 0; i < tasks_info.N; i++) {
    variable.SetOffset(i, dag_tasks.GetTask(i).deadline - rta[i]);
    variable.SetDeadline(i, rta[i]);
  }
  return variable;
}
}  // namespace DAG_SPACE