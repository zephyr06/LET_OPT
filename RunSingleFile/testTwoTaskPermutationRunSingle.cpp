
#include "gmock/gmock.h"
#include "sources/Optimization/TwoTaskPermutation.h"

using namespace DAG_SPACE;
TEST(RunSingle, v1) {
    auto dagTasks =
        ReadDAG_Tasks(GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/" +
                          GlobalVariablesDAGOpt::testDataSetName + ".csv",
                      "orig", 1);
    TaskSet& tasks = dagTasks.tasks;
    TaskSetInfoDerived tasks_info(tasks);
    std::cout << "Cause effect chains:" << std::endl;
    PrintChains(dagTasks.chains_);
    for (uint i = 0; i < dagTasks.chains_[0].size() - 1; i++) {
        int task_id_curr = dagTasks.chains_[0][i];
        int task_id_next = dagTasks.chains_[0][i + 1];
        TwoTaskPermutation two_task_permutation(
            tasks[task_id_curr], tasks[task_id_next], tasks_info);
        std::cout << "The number of permutations of two adjacent tasks "
                  << task_id_curr << " and " << task_id_next << " is "
                  << two_task_permutation.size() << "\n";
    }
}
int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}