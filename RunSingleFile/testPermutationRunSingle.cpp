

#include "gmock/gmock.h"
#include "sources/Optimization/TaskSetPermutation.h"

using namespace DAG_SPACE;
TEST(RunSingle, v1) {
    auto dag_tasks =
        ReadDAG_Tasks(GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/" +
                          GlobalVariablesDAGOpt::testDataSetName + ".csv",
                      "orig", 1);
    TaskSet& tasks = dag_tasks.tasks;
    TaskSetInfoDerived tasks_info(tasks);

    std::cout << "Cause effect chains:" << std::endl;

    TaskSetPermutation task_sets_perms =
        TaskSetPermutation(dag_tasks, dag_tasks.chains_[0]);
    int obj_find = task_sets_perms.PerformOptimization();

    // print some info
    PrintChains(dag_tasks.chains_);

    std::cout << "The minimum objective function found is " << obj_find << "\n";
    std::cout << "The total number of permutation iterations is: "
              << task_sets_perms.iteration_count_ << "\n";
}

int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}