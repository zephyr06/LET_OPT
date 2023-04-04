#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
#include "sources/Optimization/TaskSetPermutation.h"
#include "sources/Optimization/Variable.h"
#include "sources/Permutations/ChainsPermutation.h"
#include "sources/Permutations/PermutationInequality.h"
#include "sources/Permutations/TwoTaskPermutations.h"
#include "sources/RTA/RTA_LL.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Interval.h"
#include "sources/Utils/JobCEC.h"

using namespace DAG_SPACE;

class PermutationTestBase : public ::testing::Test {
 protected:
  void SetUp() override { SetUpBase("test_n3_v18"); }
  void SetUpBase(std::string test_case_name) {
    dag_tasks = ReadDAG_Tasks(GlobalVariablesDAGOpt::PROJECT_PATH +
                                  "TaskData/" + test_case_name + ".csv",
                              "RM", 2);
    tasks = dag_tasks.GetTaskSet();
    tasks_info = TaskSetInfoDerived(tasks);
    task0 = tasks[0];
    task1 = tasks[1];
    task2 = tasks[2];
    job00 = JobCEC(0, 0);
    job01 = JobCEC(0, 1);
    job10 = JobCEC(1, 0);
    job20 = JobCEC(2, 0);
  };

  DAG_Model dag_tasks;
  TaskSet tasks;
  TaskSetInfoDerived tasks_info;
  Task task0;
  Task task1;
  Task task2;
  JobCEC job00;
  JobCEC job01;
  JobCEC job10;
  JobCEC job20;
};

// TEST(root_dir, V1) {
//   std::cout << "Project root directory: " << PROJECT_ROOT_DIR << std::endl;
//   const std::string PROJECT_PATH = PROJECT_ROOT_DIR;
// }

// int main(int argc, char** argv) {
//   // ::testing::InitGoogleTest(&argc, argv);
//   ::testing::InitGoogleMock(&argc, argv);
//   return RUN_ALL_TESTS();
// }