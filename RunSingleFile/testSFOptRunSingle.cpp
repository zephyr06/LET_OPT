

#include "sources/OptimizeMain.h"

using namespace DAG_SPACE;

int main(int argc, char** argv) {
#ifdef PROFILE_CODE
  BeginTimer("main");
#endif
  auto dag_tasks = ReadDAG_Tasks(
      GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/" +
          GlobalVariablesDAGOpt::testDataSetName + ".csv",
      GlobalVariablesDAGOpt::priorityMode, GlobalVariablesDAGOpt::CHAIN_NUMBER);

  dag_tasks.chains_ = GetChainsForSF(dag_tasks);
  const TaskSet& tasks = dag_tasks.GetTaskSet();
  TaskSetInfoDerived tasks_info(tasks);

  std::cout << "Cause effect chains:" << std::endl;
  PrintChains(dag_tasks.chains_);

  std::cout << "Schedulable? " << CheckSchedulability(dag_tasks) << "\n";

  int obj_find = PerformTOM_OPT_EnumW_Skip<ObjSensorFusion>(dag_tasks).obj_;

  // std::cout << "The best permutation is: \n";
  // task_sets_perms.best_yet_chain_.print();
  // std::cout << "The best offset-deadline assignments are: \n";
  // task_sets_perms.best_yet_variable_od_.print();

  // std::cout << "Infeasible iterations: "
  //           << task_sets_perms.infeasible_iteration_ << "\n";
  // print some info

  std::cout << "The minimum objective function found is " << obj_find << "\n";
  // std::cout << "The total number of permutation iterations is: "
  //           << task_sets_perms.iteration_count_ << "\n";

  std::cout << "==================================================\n";
  auto res2 = PerformStandardLETAnalysis<ObjSensorFusion>(dag_tasks);
  int obj_find2 = res2.obj_;
  std::cout << "The minimum objective function in standard LET is " << obj_find2
            << "\n";
  std::cout << "The jitter found is " << res2.jitter_ << "\n";
#ifdef PROFILE_CODE
  EndTimer("main");
  PrintTimer();
#endif
}