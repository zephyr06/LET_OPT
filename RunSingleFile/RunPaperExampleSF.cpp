

#include "sources/OptimizeMain.h"

using namespace DAG_SPACE;
int main(int argc, char** argv) {
#ifdef PROFILE_CODE
  BeginTimer("main");
#endif
  auto dag_tasks =
      ReadDAG_Tasks(GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/" +
                        "test_PaperExample2Chain_v2" + ".csv",
                    GlobalVariablesDAGOpt::priorityMode, 2);
  const TaskSet& tasks = dag_tasks.GetTaskSet();
  TaskSetInfoDerived tasks_info(tasks);
  dag_tasks.chains_ = GetChainsForSF(dag_tasks);

  GlobalVariablesDAGOpt::OPTIMIZE_JITTER_WEIGHT = 1;

  std::cout << "Cause effect chains:" << std::endl;
  PrintChains(dag_tasks.chains_);
  std::cout << "Schedulable? " << CheckSchedulability(dag_tasks) << "\n";

  ScheduleResult res = PerformTOM_OPT_BF<ObjSensorFusion>(dag_tasks);

  std::cout << "The best offset-deadline assignments are: \n";
  res.variable_opt_.print();

  std::cout << "The SF obj after optimization is " << res.obj_ << "\n";
  std::cout << "The SF jitter after optimization is " << res.jitter_ << "\n";

#ifdef PROFILE_CODE
  EndTimer("main");
  PrintTimer();
#endif
}