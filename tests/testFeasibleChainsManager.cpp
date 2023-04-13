#include "sources/OptimizeMain.h"
#include "sources/Permutations/FeasibleChainManager.h"
#include "testEnv.cpp"

class PermutationTest22 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v22");
    // dag_tasks.chains_ = {{0, 1, 2}};
  }
};

TEST_F(PermutationTest22, IfSkipAnotherPermRT) {
  TwoTaskPermutations perm10(1, 0, dag_tasks, tasks_info, "ReactionTimeApprox");
  perm10.print();
  EXPECT_TRUE(IfSkipAnotherPermRT(*perm10[0], *perm10[1]));
  EXPECT_TRUE(IfSkipAnotherPermRT(*perm10[0], *perm10[2]));
  EXPECT_TRUE(IfSkipAnotherPermRT(*perm10[0], *perm10[3]));
  EXPECT_TRUE(IfSkipAnotherPermRT(*perm10[0], *perm10[4]));

  EXPECT_FALSE(IfSkipAnotherPermRT(*perm10[1], *perm10[0]));
  EXPECT_FALSE(IfSkipAnotherPermRT(*perm10[2], *perm10[1]));
  EXPECT_FALSE(IfSkipAnotherPermRT(*perm10[3], *perm10[2]));
  EXPECT_FALSE(IfSkipAnotherPermRT(*perm10[4], *perm10[3]));
}

TEST_F(PermutationTest22, IfSkipAnotherPermDA) {
  TwoTaskPermutations perm10(1, 0, dag_tasks, tasks_info, "ReactionTimeApprox");
  perm10.print();
  EXPECT_FALSE(IfSkipAnotherPermDA(*perm10[0], *perm10[1]));
  EXPECT_FALSE(IfSkipAnotherPermDA(*perm10[0], *perm10[2]));
  EXPECT_FALSE(IfSkipAnotherPermDA(*perm10[0], *perm10[3]));
  EXPECT_FALSE(IfSkipAnotherPermDA(*perm10[0], *perm10[4]));

  EXPECT_TRUE(IfSkipAnotherPermDA(*perm10[1], *perm10[0]));
  EXPECT_TRUE(IfSkipAnotherPermDA(*perm10[2], *perm10[1]));
  EXPECT_TRUE(IfSkipAnotherPermDA(*perm10[3], *perm10[2]));
  EXPECT_TRUE(IfSkipAnotherPermDA(*perm10[4], *perm10[3]));
}

class PermutationTest18 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v18");
    dag_tasks.chains_ = {{0, 1, 2}};
  }
};

TEST_F(PermutationTest18, FeasibleChainManager) {
  TaskSetOptEnumWSkip task_set_perms(dag_tasks, dag_tasks.chains_,
                                     "ReactionTimeApprox");
  TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info, "ReactionTimeApprox");
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info, "ReactionTimeApprox");
  perm01.print();
  perm12.print();
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm01[0]);
  chains_perm.push_back(perm12[0]);
  FeasibleChainManager fea_chain_man(
      chains_perm, task_set_perms.adjacent_two_task_permutations_,
      "ReactionTimeApprox");
  EXPECT_EQ(2, fea_chain_man.better_perm_per_chain_per_edge_.size());
  EXPECT_EQ(0,
            fea_chain_man.better_perm_per_chain_per_edge_[Edge(0, 1)].size());
  EXPECT_EQ(0,
            fea_chain_man.better_perm_per_chain_per_edge_[Edge(1, 2)].size());
}

TEST_F(PermutationTest18, IfChainsContainBetterPerm) {
  TaskSetOptEnumWSkip task_set_perms(dag_tasks, dag_tasks.chains_,
                                     "ReactionTimeApprox");
  TwoTaskPermutations perm01 =
      task_set_perms.adjacent_two_task_permutations_[0];
  TwoTaskPermutations perm12 =
      task_set_perms.adjacent_two_task_permutations_[1];
  perm01.print();
  perm12.print();
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm01[0]);
  chains_perm.push_back(perm12[0]);
  FeasibleChainManager fea_chain_man(
      chains_perm, task_set_perms.adjacent_two_task_permutations_,
      "ReactionTimeApprox");
  ChainsPermutation chains_perm_partial;
  chains_perm_partial.push_back(perm01[0]);
  EXPECT_FALSE(IfChainsContainBetterPerm(chains_perm_partial, fea_chain_man));
  chains_perm_partial.push_back(perm12[0]);
  EXPECT_FALSE(IfChainsContainBetterPerm(chains_perm_partial, fea_chain_man));

  chains_perm_partial.clear();
  chains_perm_partial.push_back(perm01[1]);
  EXPECT_FALSE(IfChainsContainBetterPerm(chains_perm_partial, fea_chain_man));
  chains_perm_partial.push_back(perm12[0]);
  EXPECT_FALSE(IfChainsContainBetterPerm(chains_perm_partial, fea_chain_man));

  ChainsPermutation chains_perm2;
  chains_perm2.push_back(perm01[0]);
  chains_perm2.push_back(perm12[1]);
  FeasibleChainManager fea_chain_man2(
      chains_perm2, task_set_perms.adjacent_two_task_permutations_,
      "ReactionTimeApprox");

  chains_perm_partial.clear();
  chains_perm_partial.push_back(perm01[0]);
  EXPECT_FALSE(IfChainsContainBetterPerm(chains_perm_partial, fea_chain_man2));
  chains_perm_partial.push_back(perm12[0]);
  EXPECT_TRUE(IfChainsContainBetterPerm(chains_perm_partial, fea_chain_man2));
}

TEST_F(PermutationTest18, IfFutureEdgesContainBetterPerm) {
  TaskSetOptEnumWSkip task_set_perms(dag_tasks, dag_tasks.chains_,
                                     "ReactionTimeApprox");
  TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info, "ReactionTimeApprox");
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info, "ReactionTimeApprox");
  perm01.print();
  perm12.print();
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm01[0]);
  chains_perm.push_back(perm12[0]);
  FeasibleChainManager fea_chain_man(
      chains_perm, task_set_perms.adjacent_two_task_permutations_,
      "ReactionTimeApprox");
  std::vector<Edge> unvisited_edges = {Edge(1, 2)};
  EXPECT_FALSE(IfFutureEdgesContainBetterPerm(unvisited_edges, fea_chain_man));
  // unvisited_edges.clear();
  EXPECT_FALSE(IfFutureEdgesContainBetterPerm({}, fea_chain_man));

  ChainsPermutation chains_perm2;
  chains_perm2.push_back(perm01[0]);
  chains_perm2.push_back(perm12[1]);
  FeasibleChainManager fea_chain_man2(
      chains_perm2, task_set_perms.adjacent_two_task_permutations_,
      "ReactionTimeApprox");

  EXPECT_TRUE(IfFutureEdgesContainBetterPerm(unvisited_edges, fea_chain_man2));
  EXPECT_FALSE(IfFutureEdgesContainBetterPerm({}, fea_chain_man2));
}

TEST_F(PermutationTest18, TakeCommonElements) {
  TaskSetOptEnumWSkip task_set_perms(dag_tasks, dag_tasks.chains_,
                                     "ReactionTime");
  TwoTaskPermutations perm01 =
      task_set_perms.adjacent_two_task_permutations_[0];
  TwoTaskPermutations perm12 =
      task_set_perms.adjacent_two_task_permutations_[1];
  TwoTaskPermutationsIterator iterator01(perm01);
  PermRefSet perm_set;
  perm_set.insert(*perm01[0]);
  iterator01.TakeCommonElements(perm_set);
  EXPECT_EQ(1, iterator01.size());
  perm_set.clear();
  iterator01.TakeCommonElements(perm_set);
  EXPECT_EQ(0, iterator01.size());
}

class PermutationTest23 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v23");
    dag_tasks.chains_ = {{0, 1, 2}};
  }
};
TEST_F(PermutationTest23, FeasibleChainManager) {
  TaskSetOptEnumWSkip task_set_perms(dag_tasks, dag_tasks.chains_,
                                     "ReactionTimeApprox");
  TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info, "ReactionTimeApprox");
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info, "ReactionTimeApprox");
  perm01.print();
  std::cout << "***********************************\n";
  perm12.print();
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm01[1]);
  chains_perm.push_back(perm12[2]);  // J(1,0)->J(2,0),
                                     // J(1,1)->J(2,1)
                                     // J(1,2)->J(2,1)
                                     // J(1,3)->J(2,2)
  FeasibleChainManager fea_chain_man(
      chains_perm, task_set_perms.adjacent_two_task_permutations_,
      "ReactionTimeApprox");
  EXPECT_EQ(2, fea_chain_man.better_perm_per_chain_per_edge_.size());
  EXPECT_EQ(1,
            fea_chain_man.better_perm_per_chain_per_edge_[Edge(0, 1)].size());
  EXPECT_EQ(2,
            fea_chain_man.better_perm_per_chain_per_edge_[Edge(1, 2)].size());
  auto prm_ref_set = fea_chain_man.better_perm_per_chain_per_edge_[Edge(1, 2)];
  for (auto perm : prm_ref_set) {
    std::unordered_map<JobCEC, JobCEC> job_react = perm.job_matches_;
    EXPECT_THAT(0, ::testing::Ge(job_react[JobCEC(1, 0)].jobId));
    EXPECT_THAT(1, ::testing::Ge(job_react[JobCEC(1, 1)].jobId));
    EXPECT_THAT(1, ::testing::Ge(job_react[JobCEC(1, 2)].jobId));
    EXPECT_THAT(2, ::testing::Ge(job_react[JobCEC(1, 3)].jobId));
  }
}

TEST_F(PermutationTest23, FeasibleChainManager_incomplete) {
  TaskSetOptEnumWSkip task_set_perms(dag_tasks, dag_tasks.chains_,
                                     "ReactionTimeApprox");
  TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info, "ReactionTimeApprox");
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info, "ReactionTimeApprox");
  perm01.print();
  std::cout << "***********************************\n";
  perm12.print();
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm01[1]);
  FeasibleChainManager fea_chain_man(
      chains_perm, task_set_perms.adjacent_two_task_permutations_,
      "ReactionTimeApprox");
  EXPECT_EQ(2, fea_chain_man.better_perm_per_chain_per_edge_.size());
  EXPECT_EQ(1,
            fea_chain_man.better_perm_per_chain_per_edge_[Edge(0, 1)].size());
  EXPECT_EQ(0,
            fea_chain_man.better_perm_per_chain_per_edge_[Edge(1, 2)].size());
}

TEST_F(PermutationTest23, IfChainsContainBetterPerm) {
  TaskSetOptEnumWSkip task_set_perms(dag_tasks, dag_tasks.chains_,
                                     "ReactionTimeApprox");
  TwoTaskPermutations perm01 =
      task_set_perms.adjacent_two_task_permutations_[0];
  TwoTaskPermutations perm12 =
      task_set_perms.adjacent_two_task_permutations_[1];
  perm01.print();
  std::cout << "***********************************\n";
  perm12.print();
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm01[1]);
  chains_perm.push_back(perm12[2]);  // J(1,0)->J(2,0),
                                     // J(1,1)->J(2,1)
                                     // J(1,2)->J(2,1)
                                     // J(1,3)->J(2,2)
  FeasibleChainManager fea_chain_man(
      chains_perm, task_set_perms.adjacent_two_task_permutations_,
      "ReactionTimeApprox");

  ChainsPermutation chains_perm_partial;
  chains_perm_partial.push_back(perm01[1]);
  EXPECT_FALSE(IfChainsContainBetterPerm(chains_perm_partial, fea_chain_man));
  chains_perm_partial.push_back(perm12[0]);
  EXPECT_TRUE(IfChainsContainBetterPerm(chains_perm_partial, fea_chain_man));
}

TEST_F(PermutationTest23, TakeCommonElements) {
  TaskSetOptEnumWSkip task_set_perms(dag_tasks, dag_tasks.chains_,
                                     "ReactionTime");
  TwoTaskPermutations perm12 =
      task_set_perms.adjacent_two_task_permutations_[1];
  TwoTaskPermutationsIterator iterator12(perm12);
  PermRefSet perm_set;
  perm_set.insert(*perm12[3]);
  perm_set.insert(*perm12[4]);
  iterator12.TakeCommonElements(perm_set);
  EXPECT_EQ(2, iterator12.size());
  perm_set.clear();
  iterator12.TakeCommonElements(perm_set);
  EXPECT_EQ(0, iterator12.size());
}

class PermutationTest22_n5 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n5_v22"); }
};

TEST_F(PermutationTest22_n5, Feasible_Chain_should_skip_perms) {
  TaskSetOptSorted task_set_perms(dag_tasks, dag_tasks.chains_, "ReactionTime");
  auto perm41 = task_set_perms.adjacent_two_task_permutations_[0];
  auto perm32 = task_set_perms.adjacent_two_task_permutations_[1];
  auto perm10 = task_set_perms.adjacent_two_task_permutations_[2];
  auto perm21 = task_set_perms.adjacent_two_task_permutations_[3];
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm41[0]);
  chains_perm.push_back(perm32[0]);
  chains_perm.push_back(perm10[1]);
  ChainsPermutation chains_perm2 = chains_perm;
  chains_perm.push_back(perm21[80]);
  task_set_perms.feasible_chains_.push_back(FeasibleChainManager(
      chains_perm, task_set_perms.adjacent_two_task_permutations_,
      "ReactionTimeApprox"));
  task_set_perms.PrintFeasibleChainsRecord();
  uint position = 3;
  TwoTaskPermutationsIterator iterator21(
      task_set_perms.adjacent_two_task_permutations_[position]);
  std::vector<Edge> unvisited_future_edges =
      task_set_perms.GetUnvisitedFutureEdges(position);
  iterator21.RemoveCandidates(chains_perm2,
                              task_set_perms.feasible_chains_.chain_man_vec_,
                              unvisited_future_edges);
  EXPECT_EQ(80, iterator21.size());
}

TEST_F(PermutationTest22_n5, skip_with_incomplete_chain_perms) {
  TaskSetOptSorted task_set_perms(dag_tasks, dag_tasks.chains_, "ReactionTime");
  auto perm41 = task_set_perms.adjacent_two_task_permutations_[0];
  auto perm32 = task_set_perms.adjacent_two_task_permutations_[1];
  auto perm10 = task_set_perms.adjacent_two_task_permutations_[2];
  auto perm21 = task_set_perms.adjacent_two_task_permutations_[3];
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm41[1]);
  task_set_perms.feasible_chains_.push_back_incomplete(FeasibleChainManager(
      chains_perm, task_set_perms.adjacent_two_task_permutations_,
      "ReactionTimeApprox"));
  task_set_perms.PrintFeasibleChainsRecord();
  uint position = 1;
  TwoTaskPermutationsIterator iterator32(
      task_set_perms.adjacent_two_task_permutations_[position]);
  std::vector<Edge> unvisited_future_edges =
      task_set_perms.GetUnvisitedFutureEdges(position);
  iterator32.RemoveCandidates(
      chains_perm, task_set_perms.feasible_chains_.chain_man_vec_incomplete_,
      unvisited_future_edges);
  EXPECT_EQ(0, iterator32.size());
}

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}