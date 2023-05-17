#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
#include "sources/Optimization/LPSolver_Cplex.h"
#include "sources/Optimization/Variable.h"
#include "sources/OptimizeMain.h"
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
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info, "ReactionTime");
  TwoTaskPermutations perm24(2, 4, dag_tasks, tasks_info, "ReactionTime");
  TwoTaskPermutations perm23(2, 3, dag_tasks, tasks_info, "ReactionTime");
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm12[8]);
  chains_perm.push_back(perm24[11]);
  chains_perm.push_back(perm23[3]);
  chains_perm.print();
  TaskSetOptEnumWSkip task_sets_perms(dag_tasks, dag_tasks.chains_,
                                      "ReactionTime");
  std::pair<VariableOD, int> res =
      FindODWithLP(task_sets_perms.dag_tasks_, task_sets_perms.tasks_info_,
                   chains_perm, task_sets_perms.graph_of_all_ca_chains_,
                   "ReactionTime", task_sets_perms.rta_);

  VariableOD best_possible_variable_od =
      FindBestPossibleVariableOD(dag_tasks, tasks_info, task_sets_perms.rta_);
  std::vector<std::vector<int>> sub_chains =
      GetSubChains(task_sets_perms.dag_tasks_.chains_, chains_perm);
  double obj_curr = ObjReactionTime::Obj(
      task_sets_perms.dag_tasks_, task_sets_perms.tasks_info_, chains_perm,
      best_possible_variable_od, sub_chains);

  ObjReactionTimeIntermediate objRI;
  EXPECT_EQ(1889 - 723,
            objRI.ObjSingleChain(dag_tasks, tasks_info, chains_perm, {1, 2, 4},
                                 best_possible_variable_od));

  EXPECT_EQ(0, objRI.ObjSingleChain(dag_tasks, tasks_info, chains_perm,
                                    {1, 2, 3}, best_possible_variable_od));

  EXPECT_THAT(obj_curr, testing::Le(res.second));
}

TEST_F(PermutationTest1, FindBestPossibleVariableOD) {
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info, "ReactionTime");
  TwoTaskPermutations perm24(2, 4, dag_tasks, tasks_info, "ReactionTime");
  TwoTaskPermutations perm23(2, 3, dag_tasks, tasks_info, "ReactionTime");
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm12[8]);
  chains_perm.push_back(perm24[11]);
  chains_perm.push_back(perm23[3]);
  chains_perm.print();

  TaskSetOptEnumWSkip task_sets_perms(dag_tasks, dag_tasks.chains_,
                                      "ReactionTime");
  VariableRange variable_range = FindPossibleVariableOD(
      dag_tasks, tasks_info, task_sets_perms.rta_, chains_perm, false);
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

  EXPECT_EQ(1, variable_range.lower_bound[4].offset);
  EXPECT_EQ(890, variable_range.lower_bound[4].deadline);
  EXPECT_EQ(19, variable_range.upper_bound[4].offset);  // modified
  EXPECT_EQ(1000, variable_range.upper_bound[4].deadline);

  VariableOD variable_od = FindBestPossibleVariableOD(
      dag_tasks, tasks_info, task_sets_perms.rta_, chains_perm, false);

  EXPECT_EQ(82, variable_od[1].offset);
  EXPECT_EQ(341, variable_od[1].deadline);
  EXPECT_EQ(9, variable_od[2].offset);
  EXPECT_EQ(1, variable_od[2].deadline);
  EXPECT_EQ(169, variable_od[3].offset);
  EXPECT_EQ(165, variable_od[3].deadline);
  EXPECT_EQ(19, variable_od[4].offset);
  EXPECT_EQ(890, variable_od[4].deadline);
}

TEST_F(PermutationTest1, FindBestPossibleVariableOD_DA) {
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info, "DataAge");
  TwoTaskPermutations perm24(2, 4, dag_tasks, tasks_info, "DataAge");
  TwoTaskPermutations perm23(2, 3, dag_tasks, tasks_info, "DataAge");
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm12[8]);
  chains_perm.push_back(perm24[11]);
  chains_perm.push_back(perm23[3]);
  chains_perm.print();

  TaskSetOptEnumWSkip task_sets_perms(dag_tasks, dag_tasks.chains_, "DataAge");
  VariableRange variable_range = FindPossibleVariableOD(
      dag_tasks, tasks_info, task_sets_perms.rta_, chains_perm, false);
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
  EXPECT_EQ(159 + 10, variable_range.upper_bound[3].offset);  // modified
  EXPECT_EQ(165, variable_range.lower_bound[3].deadline);     // true min is 165
  EXPECT_EQ(200, variable_range.upper_bound[3].deadline);

  EXPECT_EQ(1, variable_range.lower_bound[4].offset);
  EXPECT_EQ(19, variable_range.upper_bound[4].offset);  // modified
  EXPECT_EQ(890, variable_range.lower_bound[4].deadline);
  EXPECT_EQ(1000, variable_range.upper_bound[4].deadline);

  VariableOD variable_od = FindBestPossibleVariableOD(
      dag_tasks, tasks_info, task_sets_perms.rta_, chains_perm, false);

  EXPECT_EQ(82, variable_od[1].offset);
  EXPECT_EQ(341, variable_od[1].deadline);
  EXPECT_EQ(9, variable_od[2].offset);
  EXPECT_EQ(1, variable_od[2].deadline);
  EXPECT_EQ(169, variable_od[3].offset);
  EXPECT_EQ(165, variable_od[3].deadline);
  EXPECT_EQ(19, variable_od[4].offset);
  EXPECT_EQ(890, variable_od[4].deadline);
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
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info, "ReactionTime");
  // TwoTaskPermutations perm24(2, 4, dag_tasks, tasks_info,"ReactionTime");
  // TwoTaskPermutations perm23(2, 3, dag_tasks, tasks_info,"ReactionTime");
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm12[0]);
  // chains_perm.push_back(perm24[11]);
  // chains_perm.push_back(perm23[3]);
  chains_perm.print();
  auto rta = GetResponseTimeTaskSet(dag_tasks);

  TaskSetOptEnumWSkip task_sets_perms(dag_tasks, dag_tasks.chains_,
                                      "ReactionTime");
  VariableRange variable_range = FindPossibleVariableOD(
      dag_tasks, tasks_info, task_sets_perms.rta_, chains_perm, false);
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
  TaskSetOptEnumWSkip task_sets_perms(dag_tasks, dag_tasks.chains_,
                                      "ReactionTime");
  auto perm01 = task_sets_perms.adjacent_two_task_permutations_[0];
  auto perm12 = task_sets_perms.adjacent_two_task_permutations_[1];
  perm01.print();
  perm12.print();
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm01[0]);
  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  VariableRange variable_range =
      FindPossibleVariableOD(dag_tasks, tasks_info, rta, chains_perm, false);
  EXPECT_EQ(0, variable_range.lower_bound[0].offset);
  EXPECT_EQ(11 + 7, variable_range.lower_bound[1].deadline);
}

TEST_F(PermutationTest24_n3, select_feasible_perm) {
  TaskSetOptEnumWSkip task_sets_perms(dag_tasks, dag_tasks.chains_,
                                      "ReactionTime");
  auto perm01 = task_sets_perms.adjacent_two_task_permutations_[0];
  auto perm12 = task_sets_perms.adjacent_two_task_permutations_[1];
  perm01.print();
  perm12.print();
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm01[0]);
  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  VariableRange variable_range =
      FindPossibleVariableOD(dag_tasks, tasks_info, rta, chains_perm, false);
  variable_range.lower_bound.print();
  variable_range.upper_bound.print();
  Edge edge_ite(1, 2);
  PermIneqBound_Range edge_range = GetEdgeIneqRangeRT(edge_ite, variable_range);
  EXPECT_EQ(20 - 0, edge_range.lower_bound_s_upper_bound);
  EXPECT_EQ(18 - 10, edge_range.upper_bound_s_lower_bound);
  std::cout << "Valid range for edge(1,2): "
            << edge_range.lower_bound_s_upper_bound << ", "
            << edge_range.upper_bound_s_lower_bound << "\n";
  // std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  EXPECT_FALSE(
      chains_perm.IsValid(task_sets_perms.variable_range_od_, *perm12[0],
                          task_sets_perms.graph_of_all_ca_chains_, rta));

  EXPECT_TRUE(
      chains_perm.IsValid(task_sets_perms.variable_range_od_, *perm12[1],
                          task_sets_perms.graph_of_all_ca_chains_, rta));
  TwoTaskPermutationsIterator iterator(
      task_sets_perms.adjacent_two_task_permutations_[1], edge_range);
  EXPECT_EQ(1, iterator.size());
}

class PermutationTest18_n3 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v18");
    dag_tasks.chains_ = {{0, 1, 2}};
  }
};

TEST_F(PermutationTest18_n3, select_feasible_perm) {
  TaskSetOptEnumWSkip task_sets_perms(dag_tasks, dag_tasks.chains_,
                                      "ReactionTime");
  auto perm01 = task_sets_perms.adjacent_two_task_permutations_[0];
  auto perm12 = task_sets_perms.adjacent_two_task_permutations_[1];
  perm01.print();
  perm12.print();
  ChainsPermutation chains_perm;
  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  VariableRange variable_range =
      FindPossibleVariableOD(dag_tasks, tasks_info, rta, chains_perm, false);
  variable_range.lower_bound.print();
  variable_range.upper_bound.print();
  Edge edge_ite(0, 1);
  PermIneqBound_Range edge_range = GetEdgeIneqRangeRT(edge_ite, variable_range);
  EXPECT_EQ(10 - 0, edge_range.lower_bound_s_upper_bound);
  EXPECT_EQ(1 - 17, edge_range.upper_bound_s_lower_bound);
  TwoTaskPermutationsIterator iterator(
      task_sets_perms.adjacent_two_task_permutations_[0], edge_range);
  EXPECT_EQ(3, iterator.size());
}

TEST_F(PermutationTest18_n3, skip_worse_perm1) {
  TaskSetOptSorted task_sets_perms(dag_tasks, dag_tasks.chains_,
                                   "ReactionTime");
  auto perm01 = task_sets_perms.adjacent_two_task_permutations_[0];
  auto perm12 = task_sets_perms.adjacent_two_task_permutations_[1];
  perm01.print();
  perm12.print();
  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  ChainsPermutation chains_perm;
  VariableRange variable_range =
      FindPossibleVariableOD(dag_tasks, tasks_info, rta, chains_perm, false);
  Edge edge_ite01(0, 1);
  PermIneqBound_Range edge_range =
      GetEdgeIneqRangeRT(edge_ite01, variable_range);
  TwoTaskPermutationsIterator iterator01(
      task_sets_perms.adjacent_two_task_permutations_[0], edge_range);
  EXPECT_EQ(3, iterator01.size());

  std::vector<Edge> unvisited_future_edges =
      task_sets_perms.GetUnvisitedFutureEdges(0);
  chains_perm.push_back(perm01[1]);
  task_sets_perms.feasible_chains_.push_back_incomplete(FeasibleChainManager(
      chains_perm, task_sets_perms.adjacent_two_task_permutations_,
      "ReactionTime"));
  iterator01.RemoveCandidates(
      chains_perm, task_sets_perms.feasible_chains_.chain_man_vec_incomplete_,
      unvisited_future_edges);
  EXPECT_EQ(1, iterator01.size());
}
TEST_F(PermutationTest18_n3, skip_worse_perm2) {
  TaskSetOptSorted task_sets_perms(dag_tasks, dag_tasks.chains_,
                                   "ReactionTime");
  auto perm01 = task_sets_perms.adjacent_two_task_permutations_[0];
  auto perm12 = task_sets_perms.adjacent_two_task_permutations_[1];
  perm01.print();
  perm12.print();
  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  ChainsPermutation chains_perm;
  VariableRange variable_range =
      FindPossibleVariableOD(dag_tasks, tasks_info, rta, chains_perm, false);
  Edge edge_ite01(0, 1);
  PermIneqBound_Range edge_range =
      GetEdgeIneqRangeRT(edge_ite01, variable_range);
  TwoTaskPermutationsIterator iterator01(
      task_sets_perms.adjacent_two_task_permutations_[0], edge_range);
  EXPECT_EQ(3, iterator01.size());

  std::vector<Edge> unvisited_future_edges =
      task_sets_perms.GetUnvisitedFutureEdges(0);
  chains_perm.push_back(perm01[1]);
  chains_perm.push_back(perm12[1]);
  task_sets_perms.feasible_chains_.push_back_incomplete(FeasibleChainManager(
      chains_perm, task_sets_perms.adjacent_two_task_permutations_,
      "ReactionTime"));
  iterator01.RemoveCandidates(
      chains_perm, task_sets_perms.feasible_chains_.chain_man_vec_incomplete_,
      unvisited_future_edges);
  EXPECT_EQ(3, iterator01.size());

  // **************************************************Next-level  iteration
  ChainsPermutation chains_perm_next_lv;
  chains_perm_next_lv.push_back(perm01[2]);
  Edge edge_ite12(1, 2);
  VariableRange variable_range12 = FindPossibleVariableOD(
      dag_tasks, tasks_info, rta, chains_perm_next_lv, false);
  PermIneqBound_Range edge_range12 =
      GetEdgeIneqRangeRT(edge_ite12, variable_range12);
  TwoTaskPermutationsIterator iterator12(
      task_sets_perms.adjacent_two_task_permutations_[1], edge_range12);
  EXPECT_EQ(2, iterator12.size());

  auto unvisited_future_edges12 = task_sets_perms.GetUnvisitedFutureEdges(1);
  iterator12.RemoveCandidates(
      chains_perm_next_lv,
      task_sets_perms.feasible_chains_.chain_man_vec_incomplete_,
      unvisited_future_edges12);
  EXPECT_EQ(1, iterator12.size());
}

class PermutationTestExample : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_PaperExample"); }
};
TEST_F(PermutationTestExample, PerformStandardLETAnalysisDA) {
  // chain is 0 -> 3 -> 4
  ScheduleResult res = PerformTOM_OPT_Sort<ObjReactionTime>(dag_tasks);
  EXPECT_EQ(24, res.obj_);  // NOT 20, because of the pessimistism introduced by
                            // schedulability analysis
}
int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}