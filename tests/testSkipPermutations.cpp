#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
#include "sources/Optimization/LPSolver_Cplex.h"
#include "sources/Optimization/TaskSetPermutation.h"
#include "sources/Optimization/Variable.h"
#include "sources/Permutations/ChainsPermutation.h"
#include "sources/Permutations/PermutationInequality.h"
#include "sources/Permutations/TwoTaskPermutations.h"
#include "sources/RTA/RTA_LL.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Interval.h"
#include "sources/Utils/JobCEC.h"
#include "testEnv.cpp"

using namespace DAG_SPACE;

class PermutationTest1 : public ::testing::Test {
 protected:
  void SetUp() override {
    dag_tasks = ReadDAG_Tasks(
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v30.csv", "RM",
        2);
    tasks = dag_tasks.GetTaskSet();
    tasks_info = TaskSetInfoDerived(tasks);
    // task_sets_perms = TaskSetPermutation(dag_tasks, dag_tasks.chains_);
  };

  DAG_Model dag_tasks;
  TaskSet tasks;
  TaskSetInfoDerived tasks_info;
  // TaskSetPermutation task_sets_perms;
};

TEST_F(PermutationTest1, obj_) {
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
  TwoTaskPermutations perm24(2, 4, dag_tasks, tasks_info);
  TwoTaskPermutations perm23(2, 3, dag_tasks, tasks_info);
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm12[8]);
  chains_perm.push_back(perm24[11]);
  chains_perm.push_back(perm23[3]);
  chains_perm.print();
  TaskSetPermutation task_sets_perms(dag_tasks, dag_tasks.chains_);
  std::pair<VariableOD, int> res =
      FindODWithLP(task_sets_perms.dag_tasks_, task_sets_perms.tasks_info_,
                   chains_perm, task_sets_perms.graph_of_all_ca_chains_,
                   "ReactionTime", task_sets_perms.rta_);

  std::vector<std::vector<int>> sub_chains =
      GetSubChains(task_sets_perms.dag_tasks_.chains_, chains_perm);
  double obj_curr = ObjReactionTime::Obj(
      task_sets_perms.dag_tasks_, task_sets_perms.tasks_info_, chains_perm,
      task_sets_perms.best_possible_variable_od_, sub_chains);

  ObjReactionTimeIntermediate objRI;
  EXPECT_EQ(1889 - 723,
            objRI.ObjSingleChain(dag_tasks, tasks_info, chains_perm, {1, 2, 4},
                                 task_sets_perms.best_possible_variable_od_));

  EXPECT_EQ(0,
            objRI.ObjSingleChain(dag_tasks, tasks_info, chains_perm, {1, 2, 3},
                                 task_sets_perms.best_possible_variable_od_));

  EXPECT_THAT(obj_curr, testing::Le(res.second));
}

// TODO: add it to TaskSetPermutation
TEST_F(PermutationTest1, FindBestPossibleVariableOD) {
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
  TwoTaskPermutations perm24(2, 4, dag_tasks, tasks_info);
  TwoTaskPermutations perm23(2, 3, dag_tasks, tasks_info);
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm12[8]);
  chains_perm.push_back(perm24[11]);
  chains_perm.push_back(perm23[3]);
  chains_perm.print();

  TaskSetPermutation task_sets_perms(dag_tasks, dag_tasks.chains_);
  VariableRange variable_range = FindPossibleVariableOD(
      dag_tasks, tasks_info, task_sets_perms.rta_, chains_perm);
  auto rta = GetResponseTimeTaskSet(dag_tasks);

  EXPECT_EQ(0, variable_range.lower_bound[1].offset);
  EXPECT_EQ(341, variable_range.lower_bound[1].deadline);  // modified
  EXPECT_EQ(82, variable_range.upper_bound[1].offset);     // true max is 82
  EXPECT_EQ(359, variable_range.upper_bound[1].deadline);

  EXPECT_EQ(0, variable_range.lower_bound[2].offset);
  EXPECT_EQ(1, variable_range.lower_bound[2].deadline);
  EXPECT_EQ(9, variable_range.upper_bound[2].offset);
  EXPECT_EQ(10, variable_range.upper_bound[2].deadline);

  EXPECT_EQ(151, variable_range.lower_bound[3].offset);       // modified
  EXPECT_EQ(165, variable_range.lower_bound[3].deadline);     // true min is 165
  EXPECT_EQ(159 + 10, variable_range.upper_bound[3].offset);  // modified
  EXPECT_EQ(200, variable_range.upper_bound[3].deadline);

  EXPECT_EQ(0, variable_range.lower_bound[4].offset);
  EXPECT_EQ(889, variable_range.lower_bound[4].deadline);
  EXPECT_EQ(9, variable_range.upper_bound[4].offset);  // modified
  EXPECT_EQ(1000, variable_range.upper_bound[4].deadline);

  VariableOD variable_od = FindBestPossibleVariableOD(
      dag_tasks, tasks_info, task_sets_perms.rta_, chains_perm);

  EXPECT_EQ(82, variable_od[1].offset);
  EXPECT_EQ(341, variable_od[1].deadline);
  EXPECT_EQ(9, variable_od[2].offset);
  EXPECT_EQ(1, variable_od[2].deadline);
  EXPECT_EQ(169, variable_od[3].offset);
  EXPECT_EQ(165, variable_od[3].deadline);
  EXPECT_EQ(9, variable_od[4].offset);
  EXPECT_EQ(889, variable_od[4].deadline);
}

class PermutationTest2 : public ::testing::Test {
 protected:
  void SetUp() override {
    dag_tasks = ReadDAG_Tasks(
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v19.csv", "RM",
        2);
    tasks = dag_tasks.GetTaskSet();
    tasks_info = TaskSetInfoDerived(tasks);
    // task_sets_perms = TaskSetPermutation(dag_tasks, dag_tasks.chains_);
  };

  DAG_Model dag_tasks;
  TaskSet tasks;
  TaskSetInfoDerived tasks_info;
  // TaskSetPermutation task_sets_perms;
};

TEST_F(PermutationTest2, FindBestPossibleVariableOD) {
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
  // TwoTaskPermutations perm24(2, 4, dag_tasks, tasks_info);
  // TwoTaskPermutations perm23(2, 3, dag_tasks, tasks_info);
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm12[0]);
  // chains_perm.push_back(perm24[11]);
  // chains_perm.push_back(perm23[3]);
  chains_perm.print();
  auto rta = GetResponseTimeTaskSet(dag_tasks);

  TaskSetPermutation task_sets_perms(dag_tasks, dag_tasks.chains_);
  VariableRange variable_range = FindPossibleVariableOD(
      dag_tasks, tasks_info, task_sets_perms.rta_, chains_perm);
  EXPECT_EQ(0, variable_range.lower_bound[0].offset);
  EXPECT_EQ(1, variable_range.lower_bound[0].deadline);
  EXPECT_EQ(9, variable_range.upper_bound[0].offset);
  EXPECT_EQ(10, variable_range.upper_bound[0].deadline);

  EXPECT_EQ(0, variable_range.lower_bound[1].offset);
  EXPECT_EQ(3, variable_range.lower_bound[1].deadline);
  EXPECT_EQ(10, variable_range.upper_bound[1].offset);
  EXPECT_EQ(13, variable_range.upper_bound[1].deadline);  // modified

  EXPECT_EQ(3, variable_range.lower_bound[2].offset);  // modified
  EXPECT_EQ(10, variable_range.lower_bound[2].deadline);
  EXPECT_EQ(13, variable_range.upper_bound[2].offset);
  EXPECT_EQ(20, variable_range.upper_bound[2].deadline);
}
class PermutationTest24_n3 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n3_v24"); }
};

TEST_F(PermutationTest24_n3, FindPossibleVariableOD) {
  TaskSetPermutation task_sets_perms(dag_tasks, dag_tasks.chains_);
  auto perm01 = task_sets_perms.adjacent_two_task_permutations_[0];
  auto perm12 = task_sets_perms.adjacent_two_task_permutations_[1];
  perm01.print();
  perm12.print();
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm01[0]);
  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  VariableRange variable_range =
      FindPossibleVariableOD(dag_tasks, tasks_info, rta, chains_perm);
  EXPECT_EQ(0, variable_range.lower_bound[0].offset);
  EXPECT_EQ(11 + 7, variable_range.lower_bound[1].deadline);
}

TEST_F(PermutationTest24_n3, select_feasible_perm) {
  TaskSetPermutation task_sets_perms(dag_tasks, dag_tasks.chains_);
  auto perm01 = task_sets_perms.adjacent_two_task_permutations_[0];
  auto perm12 = task_sets_perms.adjacent_two_task_permutations_[1];
  perm01.print();
  perm12.print();
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm01[0]);
  TwoTaskPermutationsIterator iterator(
      task_sets_perms.adjacent_two_task_permutations_[1]);
  // iterator.RemoveCandidatesBasedOnAvailableChains(chains_perm);
  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  VariableRange variable_range =
      FindPossibleVariableOD(dag_tasks, tasks_info, rta, chains_perm);
  variable_range.lower_bound.print();
  variable_range.upper_bound.print();
  Edge edge_ite(1, 2);
  Interval edge_range = GetEdgeIneqRange(edge_ite, variable_range);
  EXPECT_EQ(18 - 10, edge_range.start);
  EXPECT_EQ(20 - 0, edge_range.getFinish());
  std::cout << "Valid range for edge(1,2): " << edge_range.start << ", "
            << edge_range.getFinish() << "\n";
}

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}