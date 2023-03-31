#include "sources/Optimization/TaskSetPermutation.h"
#include "sources/Permutations/FeasibleChainManager.h"
#include "testEnv.cpp"

class PermutationTest22 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v22");
    // dag_tasks.chains_ = {{0, 1, 2}};
  }
};

TEST_F(PermutationTest22, IfSkipAnotherPermRT) {
  TwoTaskPermutations perm10(1, 0, dag_tasks, tasks_info);
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

void TwoTaskPermutationsIterator::TakeCommonElements(
    std::list<std::shared_ptr<const SinglePairPermutation>> perms) {
  ;
}

// first generate candidates to select at this level, then take intersection
// with current available candidates;
// TwoTaskPermutationsIterator::
void RemoveCandidate(const ChainsPermutation& chains_perm,
                     const FeasibleChainManager& feasible_ref_chains) {
  // const std::vector<Edge>& edges_inserted = chains_perm.GetEdges();
  // exam whether inserted perms contain better perms than chains_perm, if so,
  // return without removing;
  // for (const auto& edge : edges_inserted) {
  //   ;
  // }
  // exam whether un-inserted perms contain better perms than chains_perm, if
  // so, return;

  // in this case, the possible elements to iterate must include only those
  // possibly better perms
}

// void TwoTaskPermutationsIterator::UpdateCandidates(
//     const FeasiblieChainsManagerVec& feasible_ref_chains,
//     const ChainsPermutation& chains_perm) {
//   // TODO: add index check, skip examming chains if feasible_ref_chains
//   remain
//   // the same
//   // for (const auto& chain_ref : feasible_ref_chains) {
//   //   RemoveCandidate(chains_perm, feasible_ref_chains);
//   // }
// }

class PermutationTest18 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v18");
    dag_tasks.chains_ = {{0, 1, 2}};
  }
};

TEST_F(PermutationTest18, FeasibleChainManager) {
  TaskSetPermutation task_set_perms(dag_tasks, dag_tasks.chains_);
  TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
  perm01.print();
  perm12.print();
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm01[0]);
  chains_perm.push_back(perm12[0]);
  FeasibleChainManager fea_chain_man(
      chains_perm, task_set_perms.adjacent_two_task_permutations_,
      "ReactionTime");
  EXPECT_EQ(2, fea_chain_man.better_perm_per_chain_per_edge_.size());
  EXPECT_EQ(0,
            fea_chain_man.better_perm_per_chain_per_edge_[Edge(0, 1)].size());
  EXPECT_EQ(0,
            fea_chain_man.better_perm_per_chain_per_edge_[Edge(1, 2)].size());
}

class PermutationTest23 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n3_v23");
    dag_tasks.chains_ = {{0, 1, 2}};
  }
};
TEST_F(PermutationTest23, FeasibleChainManager) {
  TaskSetPermutation task_set_perms(dag_tasks, dag_tasks.chains_);
  TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info);
  TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
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
      "ReactionTime");
  EXPECT_EQ(2, fea_chain_man.better_perm_per_chain_per_edge_.size());
  EXPECT_EQ(1,
            fea_chain_man.better_perm_per_chain_per_edge_[Edge(0, 1)].size());
  EXPECT_EQ(2,
            fea_chain_man.better_perm_per_chain_per_edge_[Edge(1, 2)].size());
  auto prm_ptr_set = fea_chain_man.better_perm_per_chain_per_edge_[Edge(1, 2)];
  for (auto perm_ptr : prm_ptr_set) {
    std::unordered_map<JobCEC, std::vector<JobCEC>> job_react =
        perm_ptr->job_first_react_matches_;
    EXPECT_THAT(0, ::testing::Ge(job_react[JobCEC(1, 0)][0].jobId));
    EXPECT_THAT(1, ::testing::Ge(job_react[JobCEC(1, 1)][0].jobId));
    EXPECT_THAT(1, ::testing::Ge(job_react[JobCEC(1, 2)][0].jobId));
    EXPECT_THAT(2, ::testing::Ge(job_react[JobCEC(1, 3)][0].jobId));
  }
}
int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}