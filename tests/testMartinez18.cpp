
#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/Variable.h"
#include "testEnv.cpp"
using namespace DAG_SPACE;

class PermutationTest18_n3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v18");
    dag_tasks.chains_ = {{0, 1, 2}};
    type_trait = "DataAge";
  }

 public:
  std::string type_trait;
};

class Martinez18Perms {
 public:
  // TaskSetPermutation() {}
  Martinez18Perms(const DAG_Model& dag_tasks,
                  const std::vector<std::vector<int>>& chains,
                  const std::string& type_trait)
      : start_time_((std::chrono::high_resolution_clock::now())),
        dag_tasks_(dag_tasks),
        tasks_info_(
            RegularTaskSystem::TaskSetInfoDerived(dag_tasks.GetTaskSet())),
        graph_of_all_ca_chains_(chains),
        best_yet_obj_(1e9),
        iteration_count_(0),
        rta_(GetResponseTimeTaskSet(dag_tasks_)) {
    FindPairPermutations();
  }

  void FindPairPermutations();

  // data members
  TimerType start_time_;
  DAG_Model dag_tasks_;
  RegularTaskSystem::TaskSetInfoDerived tasks_info_;
  GraphOfChains graph_of_all_ca_chains_;
  int best_yet_obj_;
  int iteration_count_;
  VariableOD best_yet_variable_od_;
  std::vector<int> rta_;
  VariableRange variable_range_od_;
  std::string type_trait_;
  int infeasible_iteration_ = 0;
};

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}