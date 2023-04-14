

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/TaskSetOptSorted.h"
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

PermutationInequality GeneratePermIneqOnlyOffset(
    int task_prev_id, int task_next_id, const std::string& type_trait,
    onst VariableRange& variable_od_range,
    const RegularTaskSystem::TaskSetInfoDerived& tasks_info) {
  PermutationInequality perm_ineq(task_prev_id, task_next_id, type_trait_);
  perm_ineq.lower_bound_ = variable_od_range.lower_bound[task_next_id].offset -
                           tasks_info.GetTask(prev_id).deadline;
  perm_ineq.lower_bound_valid_ = true;
  perm_ineq.upper_bound_ = variable_od_range.upper_bound[task_next_id].offset -
                           tasks_info.GetTask(prev_id).deadline;
  perm_ineq.upper_bound_valid_ = true;
  return perm_ineq;
}

class TwoTaskPermutations_OnlyOffset : public TwoTaskPermutations {
  TwoTaskPermutations_OnlyOffset(
      int task_prev_id, int task_next_id, const DAG_Model& dag_tasks,
      const RegularTaskSystem::TaskSetInfoDerived& tasks_info,
      const std::vector<int>& rta, const std::string& type_trait,
      const VariableRange& variable_od_range, int perm_count_global = 0)
      : TwoTaskPermutations(task_prev_id, task_next_id, dag_tasks, tasks_info,
                            rta, type_trait, perm_count_global),
        variable_range_od_(variable_range_od) {}

  void FindAllPermutations();

  // data members
  VariableRange variable_range_od_;
};

void TwoTaskPermutations_OnlyOffset::FindAllPermutations() {
  assert(type_trait_ == "DataAge");
  JobCEC job_curr(task_next_id_, 0);
  PermutationInequality perm_ineq = GeneratePermIneqOnlyOffset(
      task_prev_id_, task_next_id_, type_trait, variable_od_range_, dag_tasks);

  SinglePairPermutation single_permutation(perm_ineq, tasks_info_, type_trait_);
  AppendAllPermutations(job_curr, single_permutation);
  // reverse the saving order for faster elimination speed;
  std::reverse(single_permutations_.begin(), single_permutations_.end());
}

class TaskSetOptSorted_Offset : public TaskSetOptSorted {
 public:
  TaskSetOptSorted_Offset(const DAG_Model& dag_tasks,
                          const std::vector<std::vector<int>>& chains)
      : start_time_((std::chrono::high_resolution_clock::now())),
        dag_tasks_(dag_tasks),
        tasks_info_(
            RegularTaskSystem::TaskSetInfoDerived(dag_tasks.GetTaskSet())),
        graph_of_all_ca_chains_(chains),
        best_yet_obj_(1e9),
        iteration_count_(0),
        variable_range_od_(FindVariableRange(dag_tasks_)),
        rta_(GetResponseTimeTaskSet(dag_tasks_)),
        best_possible_variable_od_(
            FindBestPossibleVariableOD(dag_tasks_, tasks_info_, rta_)),
        type_trait_(type_trait) {
    adjacent_two_task_permutations_.reserve(
        1e2);  // there are never more than 1e2 edges
    FindPairPermutations();
    feasible_chains_ =
        FeasiblieChainsManagerVec(adjacent_two_task_permutations_.size());
  }

  void FindPairPermutations();
};
void TaskSetOptSorted_Offset::FindPairPermutations() {
  int single_perm_count = 0;
  for (const auto& edge_curr : graph_of_all_ca_chains_.edge_vec_ordered_) {
    if (ifTimeout(start_time_)) break;
    adjacent_two_task_permutations_.push_back(TwoTaskPermutations_OnlyOffset(
        edge_curr.from_id, edge_curr.to_id, dag_tasks_, tasks_info_, rta_,
        type_trait_, variable_range_od_, single_perm_count));
    single_perm_count +=
        adjacent_two_task_permutations_.back().single_permutations_.size();
    std::cout << "Pair permutation #: "
              << adjacent_two_task_permutations_.back().size() << "\n";
  }
}

TEST_F(PermutationTest18_n3, TaskSetOptSorted_Offset) { EXPECT_EQ(21, 21); }

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}