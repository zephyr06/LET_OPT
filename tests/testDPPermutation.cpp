#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
#include "sources/Optimization/ChainsPermutation.h"
#include "sources/Optimization/PermutationInequality.h"
#include "sources/Optimization/TaskSetPermutation.h"
#include "sources/Optimization/TwoTaskPermutations.h"
#include "sources/Optimization/TwoTaskPermutationsIterator.h"
#include "sources/Optimization/Variable.h"
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

TEST_F(PermutationTest18, Iterator_Infeasible) {
  TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
  TwoTaskPermutationsIterator iterator(perm01);
  EXPECT_EQ(perm01[0], iterator.front());
  iterator.Update_InFeasibleFront();
  EXPECT_EQ(2, iterator.single_perms_ite_record_.size());

  EXPECT_EQ(perm01[1], iterator.front());
  iterator.Update_InFeasibleFront();
  EXPECT_EQ(1, iterator.single_perms_ite_record_.size());
  EXPECT_EQ(perm01[2], iterator.front());
  iterator.Update_InFeasibleFront();
  EXPECT_EQ(0, iterator.single_perms_ite_record_.size());
}

TEST_F(PermutationTest18, Iterator_Feasible) {
  TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
  TwoTaskPermutationsIterator iterator(perm01);
  EXPECT_EQ(perm01[0], iterator.front());
  iterator.Update_FeasibleFront<ObjReactionTime>();
  EXPECT_TRUE(iterator.empty());
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

class PermutationTest22 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n3_v22"); }
};

TEST_F(PermutationTest22, CompareSinglePerMRT) {
  TwoTaskPermutations perm10(1, 0, dag_tasks, tasks_info);
  perm10.print();
  EXPECT_TRUE(CompareSinglePerMRT(*perm10[0], *perm10[1]));
  EXPECT_TRUE(CompareSinglePerMRT(*perm10[0], *perm10[2]));
  EXPECT_TRUE(CompareSinglePerMRT(*perm10[0], *perm10[3]));
  EXPECT_TRUE(CompareSinglePerMRT(*perm10[0], *perm10[4]));

  EXPECT_FALSE(CompareSinglePerMRT(*perm10[1], *perm10[0]));
  EXPECT_FALSE(CompareSinglePerMRT(*perm10[2], *perm10[1]));
  EXPECT_FALSE(CompareSinglePerMRT(*perm10[3], *perm10[2]));
  EXPECT_FALSE(CompareSinglePerMRT(*perm10[4], *perm10[3]));
}

TEST_F(PermutationTest22, Iterator_Feasible) {
  TwoTaskPermutations perm10(1, 0, dag_tasks, tasks_info);
  perm10.print();
  TwoTaskPermutationsIterator iterator(perm10);
  EXPECT_EQ(5, iterator.size());
  EXPECT_EQ(perm10[0], iterator.front());
  iterator.Update_FeasibleFront<ObjReactionTime>();
  EXPECT_TRUE(iterator.empty());
}

class PermutationTest23 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n3_v23"); }
};
TEST_F(PermutationTest23, Iterator_Feasible_v1) {
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
  perm12.print();
  TwoTaskPermutationsIterator iterator(perm12);
  EXPECT_EQ(7, iterator.size());
  EXPECT_EQ(perm12[0], iterator.front());
  iterator.Update_FeasibleFront<ObjReactionTime>();
  EXPECT_EQ(7 - 7, iterator.size());
}
TEST_F(PermutationTest23, Iterator_Feasible_v2) {
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
  perm12.print();
  TwoTaskPermutationsIterator iterator(perm12);
  EXPECT_EQ(7, iterator.size());
  EXPECT_EQ(perm12[0], iterator.front());
  iterator.Update_InFeasibleFront();
  EXPECT_EQ(7 - 1, iterator.size());
  iterator.Update_InFeasibleFront();
  EXPECT_EQ(7 - 2, iterator.size());
  iterator.Update_InFeasibleFront();
  EXPECT_EQ(7 - 3, iterator.size());
  iterator.Update_InFeasibleFront();
  EXPECT_EQ(7 - 4, iterator.size());
  iterator.Update_FeasibleFront<ObjReactionTime>();
  EXPECT_EQ(0, iterator.size());
}
TEST_F(PermutationTest23, Iterator_Feasible_v3) {
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
  perm12.print();
  TwoTaskPermutationsIterator iterator(perm12);
  EXPECT_EQ(7, iterator.size());
  auto itr4 = iterator.single_perms_ite_record_.begin();
  std::advance(itr4, 4);
  itr4 = iterator.single_perms_ite_record_.erase(itr4);
  iterator.single_perms_ite_record_.push_front(*itr4);
  iterator.Update_FeasibleFront<ObjReactionTime>();
  EXPECT_EQ(4, iterator.size());
}
int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}