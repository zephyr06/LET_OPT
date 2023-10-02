

#include "sources/Baseline/StandardLET.h"
#include "sources/OptimizeMain.h"

using namespace DAG_SPACE;
int main(int argc, char** argv) {
  auto dag_tasks =
      ReadDAG_Tasks(GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/" +
                        GlobalVariablesDAGOpt::testDataSetName + ".csv",
                    GlobalVariablesDAGOpt::priorityMode, 4);
  const TaskSet& tasks = dag_tasks.GetTaskSet();
  TaskSetInfoDerived tasks_info(tasks);

  std::cout << "Cause effect chains:" << std::endl;
  PrintChains(dag_tasks.chains_);
  std::cout << "Schedulable? " << CheckSchedulability(dag_tasks) << "\n";
  auto res = PerformStandardLETAnalysis<ObjReactionTime>(dag_tasks);
  int obj_find = res.obj_;
  std::cout << "The minimum objective function found is " << obj_find << "\n";
  std::cout << "The jitter found is " << res.jitter_ << "\n";
}