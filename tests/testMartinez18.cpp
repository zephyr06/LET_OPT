
#include <algorithm>  // for std::gcd

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

class Martinez18Perm : public std::vector<int> {
 public:
  Martinez18Perm() {}
};

class Martinez18TaskSetPerms {
 public:
  // TaskSetPermutation() {}
  Martinez18TaskSetPerms(const DAG_Model& dag_tasks,
                         const std::vector<int>& chain)
      : start_time_((std::chrono::high_resolution_clock::now())),
        dag_tasks_(dag_tasks),
        tasks_info_(
            RegularTaskSystem::TaskSetInfoDerived(dag_tasks.GetTaskSet())),
        chain_(chain),
        best_yet_obj_(1e9),
        iteration_count_(0),
        rta_(GetResponseTimeTaskSet(dag_tasks_)) {
    FindPossibleOffsets();
  }

  std::vector<int> GetPossibleOffset(int gcd) {
    std::vector<int> offsets;
    offsets.reserve(gcd);
    for (int i = 0; i < gcd; i++) offsets.push_back(i);
    return offsets;
  }

  void FindPossibleOffsets() {
    possible_offsets_map_.reserve(chain_.size());
    possible_offsets_map_[chain_[0]] = {0};
    int hp_yet = dag_tasks_.GetTask(chain_[0]).period;
    for (uint i = 1; i < chain_.size(); i++) {
      auto itr = possible_offsets_map_.find(chain_[i]);
      assert(itr == possible_offsets_map_.end());
      int period_curr = dag_tasks_.GetTask(chain_[i]).period;
      int gcd_max = std::gcd(hp_yet, period_curr);
      possible_offsets_map_[chain_[i]] = GetPossibleOffset(gcd_max);
      hp_yet = std::lcm(hp_yet, period_curr);
    }
  }

  // data members
  TimerType start_time_;
  DAG_Model dag_tasks_;
  RegularTaskSystem::TaskSetInfoDerived tasks_info_;
  std::vector<int> chain_;
  int best_yet_obj_;
  int iteration_count_;
  std::vector<int> rta_;

  // task_id -> its possible offsets to try
  std::unordered_map<int, std::vector<int>> possible_offsets_map_;
  VariableOD best_yet_variable_od_;
};

TEST_F(PermutationTest18_n3, FindPossibleOffsets) {
  Martinez18TaskSetPerms mart_task_perms(dag_tasks, dag_tasks.chains_[0]);
  EXPECT_EQ(3, mart_task_perms.possible_offsets_map_.size());
  EXPECT_EQ(1, mart_task_perms.possible_offsets_map_[0].size());
  EXPECT_EQ(0, mart_task_perms.possible_offsets_map_[0][0]);
  EXPECT_EQ(10, mart_task_perms.possible_offsets_map_[1].size());
  EXPECT_EQ(9, mart_task_perms.possible_offsets_map_[1][9]);
  EXPECT_EQ(20, mart_task_perms.possible_offsets_map_[2].size());
}
TEST_F(PermutationTest18_n3, Martinez18Perm) {
  Martinez18Perm mart_perm;
  mart_perm.push_back(0);
  mart_perm.push_back(1);
  mart_perm.push_back(3);
  EXPECT_EQ(3, mart_perm.size());
  EXPECT_EQ(1, mart_perm[1]);
  EXPECT_EQ(3, mart_perm[2]);
}

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}