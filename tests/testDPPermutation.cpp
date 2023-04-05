#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
#include "sources/Optimization/OptimizeMain.h"
#include "sources/Optimization/Variable.h"
#include "sources/Permutations/ChainsPermutation.h"
#include "sources/Permutations/PermutationInequality.h"
#include "sources/Permutations/TwoTaskPermutations.h"
#include "sources/Permutations/TwoTaskPermutationsIterator.h"
#include "sources/RTA/RTA_LL.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Interval.h"
#include "sources/Utils/JobCEC.h"
#include "tests/testEnv.cpp"

using namespace DAG_SPACE;

class PermutationTest18 : public ::testing::Test {
 protected:
  void SetUp() override {
    dag_tasks = ReadDAG_Tasks(
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v18.csv",
        "orig", 1);
    tasks = dag_tasks.GetTaskSet();
    tasks_info = TaskSetInfoDerived(tasks);
    task0 = tasks[0];
    task1 = tasks[1];
    task2 = tasks[2];
    job00 = JobCEC(0, 0);
    job01 = JobCEC(0, 1);
    job10 = JobCEC(1, 0);
    job20 = JobCEC(2, 0);

    perm01 = TwoTaskPermutations(0, 1, dag_tasks, tasks_info);
    perm12 = TwoTaskPermutations(1, 2, dag_tasks, tasks_info);
    dag_tasks.chains_[0] = {0, 1, 2};
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

  TwoTaskPermutations perm01;
  TwoTaskPermutations perm12;
  std::vector<int> task_chain = {0, 1, 2};
};

TEST_F(PermutationTest18, GetFirstReactMaps) {
  TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
  perm01[0]->print();
  perm12[0]->print();

  ChainsPermutation chains_perm;
  chains_perm.push_back(perm01[0]);
  // chains_perm.push_back(perm12[0]);
  std::vector<std::unordered_map<JobCEC, JobCEC>> curr_first_job_maps =
      GetFirstReactMaps(chains_perm, perm12[0], dag_tasks.chains_, dag_tasks,
                        tasks_info);

  EXPECT_EQ(1, curr_first_job_maps.size());  // only 1 chain
  EXPECT_EQ(JobCEC(2, 0), curr_first_job_maps[0][JobCEC(0, 0)]);
  EXPECT_EQ(JobCEC(2, 0), curr_first_job_maps[0][JobCEC(0, 1)]);
}

TEST_F(PermutationTest18, CompareNewPerm) {
  TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
  perm01[0]->print();
  perm12[0]->print();
  ChainsPermutation chains_perm1;
  chains_perm1.push_back(perm01[0]);
  // chains_perm1.push_back(perm12[0]);

  ChainsPermutation chains_perm2;
  chains_perm2.push_back(perm01[0]);
  // chains_perm2.push_back(perm12[1]);
  std::vector<std::unordered_map<JobCEC, JobCEC>> curr_first_job_maps1 =
      GetFirstReactMaps(chains_perm1, perm12[0], dag_tasks.chains_, dag_tasks,
                        tasks_info);

  std::vector<std::unordered_map<JobCEC, JobCEC>> curr_first_job_maps2 =
      GetFirstReactMaps(chains_perm1, perm12[1], dag_tasks.chains_, dag_tasks,
                        tasks_info);
  EXPECT_TRUE(CompareNewPerm(curr_first_job_maps1, curr_first_job_maps2));
  EXPECT_FALSE(CompareNewPerm(curr_first_job_maps2, curr_first_job_maps1));
}

TEST_F(PermutationTest18, perm_count_) {
  TaskSetOptEnumWSkip task_sets_perms =
      TaskSetOptEnumWSkip(dag_tasks, dag_tasks.chains_);
  task_sets_perms.adjacent_two_task_permutations_[0].print();
  EXPECT_EQ(3, task_sets_perms.adjacent_two_task_permutations_[0].perm_count_);
  task_sets_perms.adjacent_two_task_permutations_[1].print();
  EXPECT_EQ(5, task_sets_perms.adjacent_two_task_permutations_[1].perm_count_);
}

class PermutationTest_2chain_v1 : public ::testing::Test {
 protected:
  void SetUp() override {
    dag_tasks = ReadDAG_Tasks(
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v18.csv",
        "orig", 2);
    tasks = dag_tasks.GetTaskSet();
    tasks_info = TaskSetInfoDerived(tasks);
    task0 = tasks[0];
    task1 = tasks[1];
    task2 = tasks[2];
    job00 = JobCEC(0, 0);
    job01 = JobCEC(0, 1);
    job10 = JobCEC(1, 0);
    job20 = JobCEC(2, 0);

    perm01 = TwoTaskPermutations(0, 1, dag_tasks, tasks_info);
    perm03 = TwoTaskPermutations(0, 3, dag_tasks, tasks_info);
    perm34 = TwoTaskPermutations(3, 4, dag_tasks, tasks_info);
    perm13 = TwoTaskPermutations(1, 3, dag_tasks, tasks_info);
    variable_od = VariableOD(tasks);
    dag_tasks.chains_[0] = {0, 3, 4};
    dag_tasks.chains_.push_back({1, 3, 4});
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

  TwoTaskPermutations perm01;
  TwoTaskPermutations perm03;
  TwoTaskPermutations perm34;
  TwoTaskPermutations perm13;
  VariableOD variable_od;
};

TEST_F(PermutationTest_2chain_v1, GetFirstReactMaps) {
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm03[0]);
  chains_perm.push_back(perm34[0]);
  // chains_perm.push_back(perm13[0]);
  perm13[0]->print();
  chains_perm.print();

  std::vector<std::unordered_map<JobCEC, JobCEC>> curr_first_job_maps =
      GetFirstReactMaps(chains_perm, perm13[0], dag_tasks.chains_, dag_tasks,
                        tasks_info);

  EXPECT_EQ(2, curr_first_job_maps.size());
  EXPECT_EQ(JobCEC(4, 0), curr_first_job_maps[0][JobCEC(0, 0)]);
  EXPECT_EQ(JobCEC(4, 0), curr_first_job_maps[0][JobCEC(0, 1)]);
  EXPECT_EQ(3, curr_first_job_maps[1].size());
  EXPECT_EQ(JobCEC(4, 0), curr_first_job_maps[1][JobCEC(1, 0)]);
}

TEST_F(PermutationTest_2chain_v1, GetFirstReactMaps_v2) {
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm03[0]);
  // chains_perm.push_back(perm34[0]);
  // chains_perm.push_back(perm13[0]);
  chains_perm.print();

  std::vector<std::unordered_map<JobCEC, JobCEC>> curr_first_job_maps =
      GetFirstReactMaps(chains_perm, perm13[0], dag_tasks.chains_, dag_tasks,
                        tasks_info);

  EXPECT_EQ(2, curr_first_job_maps.size());
  EXPECT_EQ(JobCEC(3, 0), curr_first_job_maps[0][JobCEC(0, 0)]);
  EXPECT_EQ(JobCEC(3, 0), curr_first_job_maps[0][JobCEC(0, 1)]);
  EXPECT_EQ(JobCEC(3, 0), curr_first_job_maps[1][JobCEC(1, 0)]);
}

TEST_F(PermutationTest_2chain_v1, CompareNewPermv1) {
  ChainsPermutation chains_perm1;
  chains_perm1.push_back(perm03[0]);
  chains_perm1.push_back(perm34[0]);
  // chains_perm1.push_back(perm13[0]);
  chains_perm1.print();

  ChainsPermutation chains_perm2;
  chains_perm2.push_back(perm03[0]);
  chains_perm2.push_back(perm34[0]);
  // chains_perm2.push_back(perm13[0]);
  chains_perm2.print();
  std::vector<std::unordered_map<JobCEC, JobCEC>> curr_first_job_maps1 =
      GetFirstReactMaps(chains_perm1, perm13[0], dag_tasks.chains_, dag_tasks,
                        tasks_info);

  std::vector<std::unordered_map<JobCEC, JobCEC>> curr_first_job_maps2 =
      GetFirstReactMaps(chains_perm1, perm13[1], dag_tasks.chains_, dag_tasks,
                        tasks_info);
  EXPECT_TRUE(CompareNewPerm(curr_first_job_maps1, curr_first_job_maps2));
  EXPECT_FALSE(CompareNewPerm(curr_first_job_maps2, curr_first_job_maps1));
}
TEST_F(PermutationTest_2chain_v1, CompareNewPermv2) {
  ChainsPermutation chains_perm1;
  chains_perm1.push_back(perm03[0]);
  // chains_perm1.push_back(perm34[0]);
  chains_perm1.push_back(perm13[0]);
  chains_perm1.print();

  ChainsPermutation chains_perm2;
  chains_perm2.push_back(perm03[0]);
  // chains_perm2.push_back(perm34[0]);
  chains_perm2.push_back(perm13[0]);
  chains_perm2.print();
  std::vector<std::unordered_map<JobCEC, JobCEC>> curr_first_job_maps1 =
      GetFirstReactMaps(chains_perm1, perm34[0], dag_tasks.chains_, dag_tasks,
                        tasks_info);

  std::vector<std::unordered_map<JobCEC, JobCEC>> curr_first_job_maps2 =
      GetFirstReactMaps(chains_perm1, perm34[1], dag_tasks.chains_, dag_tasks,
                        tasks_info);
  EXPECT_TRUE(CompareNewPerm(curr_first_job_maps1, curr_first_job_maps2));
  EXPECT_FALSE(CompareNewPerm(curr_first_job_maps2, curr_first_job_maps1));
}
class PermutationTest46 : public ::testing::Test {
 protected:
  void SetUp() override {
    dag_tasks = ReadDAG_Tasks(
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v46.csv", "RM",
        2);
    tasks = dag_tasks.GetTaskSet();
    tasks_info = TaskSetInfoDerived(tasks);
    variable_od = VariableOD(tasks);
  };

  DAG_Model dag_tasks;
  TaskSet tasks;
  TaskSetInfoDerived tasks_info;
  VariableOD variable_od;
};
TEST_F(PermutationTest46, CompareNewPermv1) {
  TwoTaskPermutations perm23 = TwoTaskPermutations(2, 3, dag_tasks, tasks_info);
  TwoTaskPermutations perm34 = TwoTaskPermutations(3, 4, dag_tasks, tasks_info);
  ChainsPermutation chains_perm1;
  chains_perm1.push_back(perm23[0]);
  // chains_perm1.push_back(perm34[0]);
  chains_perm1.print();

  ChainsPermutation chains_perm2;
  chains_perm2.push_back(perm23[0]);
  // // chains_perm2.push_back(perm34[0]);
  // chains_perm2.print();
  std::vector<std::unordered_map<JobCEC, JobCEC>> curr_first_job_maps1 =
      GetFirstReactMaps(chains_perm1, perm34[0], dag_tasks.chains_, dag_tasks,
                        tasks_info);

  std::vector<std::unordered_map<JobCEC, JobCEC>> curr_first_job_maps2 =
      GetFirstReactMaps(chains_perm1, perm34[3], dag_tasks.chains_, dag_tasks,
                        tasks_info);
  if (GlobalVariablesDAGOpt::SearchDP_Friendly) {
    EXPECT_TRUE(CompareNewPerm(curr_first_job_maps1, curr_first_job_maps2));
    EXPECT_TRUE(CompareNewPerm(curr_first_job_maps2, curr_first_job_maps1));
  }
}
TEST_F(PermutationTest46, perm_count_) {
  TaskSetOptEnumWSkip task_sets_perms =
      TaskSetOptEnumWSkip(dag_tasks, dag_tasks.chains_);
  task_sets_perms.adjacent_two_task_permutations_[0].print();
  EXPECT_EQ(6, task_sets_perms.adjacent_two_task_permutations_[0].perm_count_);
  task_sets_perms.adjacent_two_task_permutations_[1].print();
  EXPECT_EQ(16, task_sets_perms.adjacent_two_task_permutations_[1].perm_count_);
}

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}