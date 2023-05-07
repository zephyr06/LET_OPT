
#include "sources/Baseline/Bradatsch16.h"

#include "sources/RTA/RTA_LL.h"

namespace DAG_SPACE {
VariableOD GetVariableOD_Bradatsch16(const DAG_Model& dag_tasks) {
  VariableOD variable(dag_tasks.GetTaskSet());
  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  for (uint task_id = 0; task_id < dag_tasks.GetTaskSet().size(); task_id++) {
    variable[task_id].deadline = rta[task_id];
  }
  return variable;
}
}  // namespace DAG_SPACE