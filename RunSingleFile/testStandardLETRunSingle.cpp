

#include "gmock/gmock.h"
#include "sources/Baseline/StandardLET.h"
#include "sources/Optimization/TaskSetPermutation.h"

using namespace DAG_SPACE;
TEST(RunSingle, v1) {
    auto dag_tasks =
        ReadDAG_Tasks(GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/" +
                          GlobalVariablesDAGOpt::testDataSetName + ".csv",
                      GlobalVariablesDAGOpt::priorityMode, 1);
    const TaskSet& tasks = dag_tasks.GetTaskSet();
    TaskSetInfoDerived tasks_info(tasks);

    std::cout << "Cause effect chains:" << std::endl;
    PrintChains(dag_tasks.chains_);
    std::cout << "Schedulable? " << CheckSchedulability(dag_tasks) << "\n";
    int obj_find = PerformLETAnalysis<ObjReactionTime>(dag_tasks).obj_;
    std::cout << "The minimum objective function found is " << obj_find << "\n";
}

int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}