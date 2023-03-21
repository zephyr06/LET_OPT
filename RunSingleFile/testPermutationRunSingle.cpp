

#include "gmock/gmock.h"
#include "sources/Optimization/TaskSetPermutation.h"

using namespace DAG_SPACE;
TEST(RunSingle, v1) {
#ifdef PROFILE_CODE
    BeginTimer("main");
#endif
    auto dag_tasks =
        ReadDAG_Tasks(GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/" +
                          GlobalVariablesDAGOpt::testDataSetName + ".csv",
                      GlobalVariablesDAGOpt::priorityMode,
                      GlobalVariablesDAGOpt::CHAIN_NUMBER);
    const TaskSet& tasks = dag_tasks.GetTaskSet();
    TaskSetInfoDerived tasks_info(tasks);

    std::cout << "Cause effect chains:" << std::endl;
    PrintChains(dag_tasks.chains_);

    std::cout << "Schedulable? " << CheckSchedulability(dag_tasks) << "\n";

    TaskSetPermutation task_sets_perms =
        TaskSetPermutation(dag_tasks, {dag_tasks.chains_});
    int obj_find = task_sets_perms.PerformOptimization<ObjReactionTime>();

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

int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}