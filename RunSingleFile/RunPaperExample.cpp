

#include "sources/OptimizeMain.h"

using namespace DAG_SPACE;
int main(int argc, char** argv) {
#ifdef PROFILE_CODE
  BeginTimer("main");
#endif
  auto dag_tasks =
      ReadDAG_Tasks(GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/" +
                        "test_PaperExample2Chain" + ".csv",
                    GlobalVariablesDAGOpt::priorityMode, 2);
  const TaskSet& tasks = dag_tasks.GetTaskSet();
  TaskSetInfoDerived tasks_info(tasks);

  std::cout << "Cause effect chains:" << std::endl;
  PrintChains(dag_tasks.chains_);

  std::cout << "Schedulable? " << CheckSchedulability(dag_tasks) << "\n";

  TaskSetOptSorted task_sets_perms =
      TaskSetOptSorted(dag_tasks, {dag_tasks.chains_}, "DataAge");

  std::cout << "*******************All the edge last-reading "
               "patterns***********************\n";
  task_sets_perms.adjacent_two_task_permutations_[0].print();
  task_sets_perms.adjacent_two_task_permutations_[1].print();
  task_sets_perms.adjacent_two_task_permutations_[2].print();
  std::cout << "*******************All the edge last-reading "
               "patterns***********************\n";
  int obj_find = task_sets_perms.PerformOptimizationSort<ObjDataAge>();

  std::cout << "The best permutation is: \n";
  task_sets_perms.best_yet_chain_.print();
  std::cout << "The best offset-deadline assignments are: \n";
  task_sets_perms.best_yet_variable_od_.print();

  std::cout << "Infeasible iterations: "
            << task_sets_perms.infeasible_iteration_ << "\n";
  // print some info

  std::cout << "The minimum objective function found is " << obj_find << "\n";
  std::cout << "The total number of permutation iterations is: "
            << task_sets_perms.iteration_count_ << "\n";
#ifdef PROFILE_CODE
  EndTimer("main");
  PrintTimer();
#endif
}