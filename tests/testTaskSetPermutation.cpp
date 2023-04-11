#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
#include "sources/Optimization/OptimizeMain.h"
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

    perm01 = TwoTaskPermutations(0, 1, dag_tasks, tasks_info, "ReactionTime");
    perm12 = TwoTaskPermutations(1, 2, dag_tasks, tasks_info, "ReactionTime");
    dag_tasks.chains_[0] = {0, 1, 2};
    // task_sets_perms = TaskSetPermutation(dag_tasks, {task_chain});
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
  // TaskSetPermutation task_sets_perms;
  std::vector<int> task_chain = {0, 1, 2};
};

TEST_F(PermutationTest1, Iteration) {
  TaskSetOptEnumWSkip task_sets_perms(dag_tasks, {task_chain}, "ReactionTime");
  int obj_find =
      task_sets_perms.PerformOptimizationEnumerate<ObjReactionTime>();
  // task_sets_perms.best_yet_chain_[0]->print();
  // task_sets_perms.best_yet_chain_[1]->print();
  EXPECT_THAT(task_sets_perms.iteration_count_, testing::Le(6));
  EXPECT_EQ(20, obj_find);
}
TEST_F(PermutationTest1, Iteration_Sorted) {
  TaskSetOptSorted task_sets_perms(dag_tasks, {task_chain}, "ReactionTime");
  int obj_find = task_sets_perms.PerformOptimizationSort<ObjReactionTime>();
  // task_sets_perms.best_yet_chain_[0]->print();
  // task_sets_perms.best_yet_chain_[1]->print();
  EXPECT_THAT(task_sets_perms.iteration_count_, testing::Le(6));
  EXPECT_EQ(20, obj_find);
}
TEST_F(PermutationTest1, GetUnvisitedFutureEdges) {
  TaskSetOptEnumWSkip task_sets_perms(dag_tasks, {task_chain}, "ReactionTime");
  EXPECT_EQ(1, task_sets_perms.GetUnvisitedFutureEdges(0).size());
}

// TEST_F(PermutationTest1, CompareAndUpdateMinOffsetLB) {
//   dag_tasks.chains_ = {{0, 1, 2}};
//   TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info,"ReactionTime");
//   TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info,"ReactionTime");

//   ChainsPermutation chains_perm;
//   chains_perm.push_back(perm01[0]);
//   chains_perm.push_back(perm12[0]);
//   chains_perm.print();

//   GraphOfChains graph_chains(dag_tasks.chains_);

//   std::vector<int> rta = {1, 3, 6};
//   int min_offset_before = 2;
//   std::cout << "Min offset: "
//             << GetMinOffSet(min_offset_before, dag_tasks, tasks_info,
//                             chains_perm, graph_chains, "ReactionTime", rta)
//             << "\n";

//   ChainsPermutation chains_perm2;
//   chains_perm2.push_back(perm01[0]);
//   chains_perm2.push_back(perm12[1]);
//   chains_perm.print();
//   min_offset_before = 2;
//   std::cout << "Min offset: "
//             << GetMinOffSet(min_offset_before, dag_tasks, tasks_info,
//                             chains_perm2, graph_chains, "ReactionTime", rta)
//             << "\n";

//   TaskSetOptEnumWSkip task_sets_perms =
//       TaskSetPermutation(dag_tasks, {dag_tasks.chains_});
//   ChainsPermutation chains_perm3;
//   chains_perm3.push_back(perm01[0]);
//   chains_perm3.push_back(perm12[1]);
//   min_offset_before =
//       GetMinOffSet(min_offset_before, dag_tasks, tasks_info, chains_perm,
//                    graph_chains, "ReactionTime", rta);

//   EXPECT_TRUE(task_sets_perms.CompareAndUpdateMinOffsetLB<ObjReactionTime>(
//       min_offset_before, chains_perm3, 2));
// }

TEST_F(PermutationTest1, GenerateBoxPermutationConstraints) {
  // RTA 1 3 6
  VariableRange variable_range = FindVariableRange(dag_tasks);
  PermutationInequality perm_ineq =
      GenerateBoxPermutationConstraints(0, 1, variable_range, "ReactionTime");
  EXPECT_EQ(1 - 17 - 1, perm_ineq.lower_bound_);
  EXPECT_EQ(10 - 0, perm_ineq.upper_bound_);

  perm_ineq =
      GenerateBoxPermutationConstraints(1, 2, variable_range, "ReactionTime");
  EXPECT_EQ(3 - 14 - 1, perm_ineq.lower_bound_);
  EXPECT_EQ(20 - 0, perm_ineq.upper_bound_);
}

class PermutationTest2 : public ::testing::Test {
 protected:
  void SetUp() override {
    dag_tasks = ReadDAG_Tasks(
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v14.csv", "RM",
        1);
    tasks = dag_tasks.GetTaskSet();
    tasks_info = TaskSetInfoDerived(tasks);
    task0 = tasks[0];
    task1 = tasks[1];
    task2 = tasks[2];
    job00 = JobCEC(0, 0);
    job01 = JobCEC(0, 1);
    job10 = JobCEC(1, 0);
    job20 = JobCEC(2, 0);

    perm01 = TwoTaskPermutations(0, 1, dag_tasks, tasks_info, "ReactionTime");
    perm12 = TwoTaskPermutations(1, 2, dag_tasks, tasks_info, "ReactionTime");
    // task_sets_perms = TaskSetPermutation(dag_tasks, {task_chain});
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
  // TaskSetPermutation task_sets_perms;
  std::vector<int> task_chain = {4, 3, 0};
};

TEST_F(PermutationTest2, FindVariableRange) {
  // RTA 17 19 53
  VariableRange variable_range = FindVariableRange(dag_tasks);
  EXPECT_EQ(213, variable_range.lower_bound[1].deadline);
  EXPECT_EQ(25, variable_range.lower_bound[2].deadline);
  EXPECT_EQ(27, variable_range.lower_bound[3].deadline);
  EXPECT_EQ(61, variable_range.lower_bound[4].deadline);
}

TEST_F(PermutationTest2, GenerateBoxPermutationConstraints) {
  // RTA 17 19 53
  VariableRange variable_range = FindVariableRange(dag_tasks);
  PermutationInequality perm_ineq =
      GenerateBoxPermutationConstraints(4, 3, variable_range, "ReactionTime");
  EXPECT_EQ(61 - (100 - 27) - 1, perm_ineq.lower_bound_);
  EXPECT_EQ(100 - 0, perm_ineq.upper_bound_);

  perm_ineq =
      GenerateBoxPermutationConstraints(3, 0, variable_range, "ReactionTime");
  EXPECT_EQ(27 - (100 - 17) - 1, perm_ineq.lower_bound_);
  EXPECT_EQ(100 - 0, perm_ineq.upper_bound_);
}

class PermutationTest3 : public ::testing::Test {
 protected:
  void SetUp() override {
    dag_tasks = ReadDAG_Tasks(
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v9.csv", "orig",
        1);
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

TEST_F(PermutationTest1, SinglePairPermutation_constructor) {
  int prev_id = 0, next_id = 1;
  std::unordered_map<JobCEC, JobCEC> job_first_react_matches01;
  for (uint i = 0; i < 2; i++) {
    JobCEC job_curr(prev_id, i);
    job_first_react_matches01[job_curr] = {JobCEC(1, 0)};
  }
  SinglePairPermutation single_perm01(0, 1, job_first_react_matches01,
                                      tasks_info, "ReactionTime");
  EXPECT_EQ(prev_id, single_perm01.inequality_.task_prev_id_);
  EXPECT_EQ(next_id, single_perm01.inequality_.task_next_id_);
  EXPECT_EQ(-20, single_perm01.inequality_.lower_bound_);
  EXPECT_EQ(-10, single_perm01.inequality_.upper_bound_);

  prev_id = 1;
  next_id = 2;

  std::unordered_map<JobCEC, JobCEC> job_first_react_matches12;
  job_first_react_matches12[JobCEC(1, 0)] = {JobCEC(2, 0)};
  SinglePairPermutation single_perm12(1, 2, job_first_react_matches12,
                                      tasks_info, "ReactionTime");
  EXPECT_EQ(prev_id, single_perm12.inequality_.task_prev_id_);
  EXPECT_EQ(next_id, single_perm12.inequality_.task_next_id_);
  EXPECT_EQ(-20, single_perm12.inequality_.lower_bound_);
  EXPECT_EQ(0, single_perm12.inequality_.upper_bound_);
}

TEST_F(PermutationTest1, SinglePairPermutation_valid) {
  int prev_id = 0, next_id = 1;
  std::unordered_map<JobCEC, JobCEC> job_first_react_matches01;
  for (uint i = 0; i < 2; i++) {
    JobCEC job_curr(prev_id, i);
    job_first_react_matches01[job_curr] = {JobCEC(next_id, 0)};
  }
  SinglePairPermutation single_perm01(0, 1, job_first_react_matches01,
                                      tasks_info, "ReactionTime");

  std::unordered_map<JobCEC, JobCEC> job_first_react_matches12;
  job_first_react_matches12[JobCEC(1, 0)] = {JobCEC(2, 0)};
  SinglePairPermutation single_perm12(1, 2, job_first_react_matches12,
                                      tasks_info, "ReactionTime");

  VariableRange variable_range_od = FindVariableRange(dag_tasks);
  ChainsPermutation chains_perm;
  chains_perm.push_back(
      std::make_shared<const SinglePairPermutation>(single_perm01));
  // chains_perm.push_back(single_perm12);

  GraphOfChains graph_chains({task_chain});
  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  EXPECT_TRUE(
      chains_perm.IsValid(variable_range_od, single_perm12, graph_chains, rta));
}

TEST_F(PermutationTest3, SinglePairPermutation_constructor) {
  int prev_id = 0, next_id = 1;
  std::unordered_map<JobCEC, JobCEC> job_first_react_matches01;
  for (uint i = 0; i < 5; i++) {
    JobCEC job_curr(prev_id, i);
    job_first_react_matches01[job_curr] = {JobCEC(1, 0)};
  }
  for (uint i = 5; i < 9; i++) {
    JobCEC job_curr(prev_id, i);
    job_first_react_matches01[job_curr] = {JobCEC(1, 1)};
  }

  SinglePairPermutation single_perm01(0, 1, job_first_react_matches01,
                                      tasks_info, "ReactionTime");
  EXPECT_EQ(prev_id, single_perm01.inequality_.task_prev_id_);
  EXPECT_EQ(next_id, single_perm01.inequality_.task_next_id_);
  EXPECT_EQ(-50, single_perm01.inequality_.lower_bound_);
  EXPECT_EQ(-40, single_perm01.inequality_.upper_bound_);

  prev_id = 1;
  next_id = 2;
  std::unordered_map<JobCEC, JobCEC> job_first_react_matches12;
  job_first_react_matches12[JobCEC(1, 0)] = {JobCEC(2, 0)};
  SinglePairPermutation single_perm12(1, 2, job_first_react_matches12,
                                      tasks_info, "ReactionTime");
  EXPECT_EQ(prev_id, single_perm12.inequality_.task_prev_id_);
  EXPECT_EQ(next_id, single_perm12.inequality_.task_next_id_);
  EXPECT_EQ(-10, single_perm12.inequality_.lower_bound_);
  EXPECT_EQ(0, single_perm12.inequality_.upper_bound_);
}

TEST_F(PermutationTest3, ChainsPermutation_valid_v1) {
  int prev_id = 0, next_id = 1;
  std::unordered_map<JobCEC, JobCEC> job_first_react_matches01;
  for (uint i = 0; i < 5; i++) {
    JobCEC job_curr(prev_id, i);
    job_first_react_matches01[job_curr] = {JobCEC(1, 0)};
  }
  for (uint i = 5; i < 9; i++) {
    JobCEC job_curr(prev_id, i);
    job_first_react_matches01[job_curr] = {JobCEC(1, 1)};
  }
  PermutationInequality perm_ineq01(prev_id, next_id, -50, true, -40, true,
                                    "ReactionTime");
  SinglePairPermutation single_perm01(perm_ineq01, job_first_react_matches01,
                                      "ReactionTime");

  prev_id = 1;
  next_id = 2;
  std::unordered_map<JobCEC, JobCEC> job_first_react_matches12;
  job_first_react_matches12[JobCEC(1, 0)] = {JobCEC(2, 1)};
  PermutationInequality perm_ineq12(prev_id, next_id, 0, true, 10, true,
                                    "ReactionTime");
  SinglePairPermutation single_perm12(perm_ineq12, job_first_react_matches12,
                                      "ReactionTime");

  VariableRange variable_range_od = FindVariableRange(dag_tasks);
  ChainsPermutation chains_perm;
  chains_perm.push_back(
      std::make_shared<const SinglePairPermutation>(single_perm01));
  // EXPECT_TRUE(chains_perm.IsValid(variable_range_od));
  // chains_perm.push_back(single_perm12);
  GraphOfChains graph_chains({{0, 1, 2}});
  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  EXPECT_FALSE(
      chains_perm.IsValid(variable_range_od, single_perm12, graph_chains, rta));
}

class PermutationTest4 : public ::testing::Test {
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

    perm01 = TwoTaskPermutations(0, 1, dag_tasks, tasks_info, "ReactionTime");
    perm12 = TwoTaskPermutations(1, 2, dag_tasks, tasks_info, "ReactionTime");
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
  std::vector<std::vector<int>> task_chains = {
      {0, 1}, {0, 2}};  // from 0 to 1, from 0 to 2
};

TEST_F(PermutationTest4, ChainsPermutation_valid_v1) {
  GraphOfChains graph_chains(task_chains);
  EXPECT_EQ(2, graph_chains.edge_records_.size());
  EXPECT_EQ(2, graph_chains.edge_vec_ordered_.size());
  EXPECT_EQ(0, graph_chains.prev_tasks_[1][0]);
  EXPECT_EQ(0, graph_chains.prev_tasks_[2][0]);
  EXPECT_EQ(1, graph_chains.next_tasks_[0][0]);
  EXPECT_EQ(2, graph_chains.next_tasks_[0][1]);
}

class PermutationTest5 : public ::testing::Test {
 protected:
  void SetUp() override {
    dag_tasks = ReadDAG_Tasks(
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n30_v1.csv",
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
  std::vector<std::vector<int>> task_chains = {
      {0, 1, 6, 7}, {0, 2}, {20, 1}, {25, 6, 7}};
};
TEST_F(PermutationTest5, ChainsPermutation_valid_v1) {
  GraphOfChains graph_chains(task_chains);
  EXPECT_EQ(6, graph_chains.edge_records_.size());
  EXPECT_EQ(6, graph_chains.edge_vec_ordered_.size());
  EXPECT_EQ(0, graph_chains.prev_tasks_[1][0]);
  EXPECT_EQ(20, graph_chains.prev_tasks_[1][1]);
  EXPECT_EQ(0, graph_chains.prev_tasks_[2][0]);

  EXPECT_EQ(1, graph_chains.next_tasks_[0][0]);
  EXPECT_EQ(2, graph_chains.next_tasks_[0][1]);
  EXPECT_EQ(7, graph_chains.next_tasks_[6][0]);
  EXPECT_EQ(1, graph_chains.next_tasks_[6].size());
  EXPECT_EQ(6, graph_chains.next_tasks_[25][0]);
}

class PermutationTest7_n30 : public PermutationTestBase {
  void SetUp() override { SetUpBase("test_n30_v7"); }
};
TEST_F(PermutationTest7_n30, GraphOfChains) {
  // auto task_sets_perms = TaskSetPermutation(dag_tasks, dag_tasks.chains_);
  GraphOfChains graph_of_all_ca_chains_(dag_tasks.chains_);
  EXPECT_EQ(12, graph_of_all_ca_chains_.edge_records_.size());
}

TEST(ReadGlobalVariable, v1) {
  EXPECT_EQ("RM", GlobalVariablesDAGOpt::priorityMode);
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

    perm01 = TwoTaskPermutations(0, 1, dag_tasks, tasks_info, "ReactionTime");
    perm03 = TwoTaskPermutations(0, 3, dag_tasks, tasks_info, "ReactionTime");
    perm34 = TwoTaskPermutations(3, 4, dag_tasks, tasks_info, "ReactionTime");
    perm13 = TwoTaskPermutations(1, 3, dag_tasks, tasks_info, "ReactionTime");

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

TEST_F(PermutationTest_2chain_v1, GetUnvisitedFutureEdges) {
  TaskSetOptEnumWSkip task_sets_perms(dag_tasks, dag_tasks.chains_,
                                      "ReactionTime");
  EXPECT_EQ(2, task_sets_perms.GetUnvisitedFutureEdges(0).size());
  EXPECT_EQ(Edge(3, 4), task_sets_perms.GetUnvisitedFutureEdges(0)[1]);
}
TEST_F(PermutationTest_2chain_v1, TaskSetPermutation) {
  // dag_tasks.chains_[0] = {0, 3, 4};
  // dag_tasks.chains_.push_back({1, 3, 4});
  auto res = PerformTOM_OPT_EnumW_Skip<ObjReactionTime>(dag_tasks);
  EXPECT_THAT(res.obj_, testing::Le(128 + 28));
}
TEST_F(PermutationTest_2chain_v1, TaskSetPermutation_sort) {
  // dag_tasks.chains_[0] = {0, 3, 4};
  // dag_tasks.chains_.push_back({1, 3, 4});
  auto res = PerformTOM_OPT_Sort<ObjReactionTime>(dag_tasks);
  EXPECT_THAT(res.obj_, testing::Le(128 + 28));
}

class PermutationTest5_n5 : public PermutationTestBase {
  void SetUp() override {
    SetUpBase("test_n5_v5");
    type_trait = "ReactionTime";
  }

 public:
  std::string type_trait;
};
TEST_F(PermutationTest5_n5, IsPermConflicted_CheckAllWithSameSink) {
  TwoTaskPermutations perm14(1, 4, dag_tasks, tasks_info, "ReactionTime");
  TwoTaskPermutations perm34(3, 4, dag_tasks, tasks_info, "ReactionTime");
  perm14.print();
  perm34.print();
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm34[0]);
  GraphOfChains graph_of_all_ca_chains(dag_tasks.chains_);
  VariableRange variable_od_range(FindVariableRange(dag_tasks));
  variable_od_range.lower_bound.print();
  variable_od_range.upper_bound.print();
  Interval intv1 =
      GetOffsetRange_RTPerm(variable_od_range, perm14[0]->inequality_);

  EXPECT_EQ(0, intv1.start);
  EXPECT_EQ(0, intv1.start + intv1.length);  // before clap: 369 + 1 + 100

  Interval intv2 =
      GetOffsetRange_RTPerm(variable_od_range, perm34[0]->inequality_);
  EXPECT_EQ(0, intv2.start);
  EXPECT_EQ(0, intv2.start + intv2.length);  // before clap: 300 + 500
  EXPECT_FALSE(chains_perm.IsPermConflicted_CheckAllWithSameSink(
      variable_od_range, *perm14[0], graph_of_all_ca_chains));
}

class PermutationTest6 : public ::testing::Test {
 protected:
  void SetUp() override {
    dag_tasks = ReadDAG_Tasks(
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v8.csv", "orig",
        2);
    tasks = dag_tasks.GetTaskSet();
    tasks_info = TaskSetInfoDerived(tasks);

    perm01 = TwoTaskPermutations(0, 1, dag_tasks, tasks_info, "ReactionTime");
    perm04 = TwoTaskPermutations(0, 4, dag_tasks, tasks_info, "ReactionTime");
    perm34 = TwoTaskPermutations(3, 4, dag_tasks, tasks_info, "ReactionTime");
    perm13 = TwoTaskPermutations(1, 3, dag_tasks, tasks_info, "ReactionTime");

    variable_od = VariableOD(tasks);
    dag_tasks.chains_[0] = {0, 1};
    dag_tasks.chains_.push_back({0, 4});
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
  TwoTaskPermutations perm04;
  TwoTaskPermutations perm34;
  TwoTaskPermutations perm13;
  VariableOD variable_od;
};

TEST_F(PermutationTest6, IsPermConflicted_CheckAllWithSameSource) {
  perm01[0]->print();
  perm04[1]->print();
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm01[0]);
  // chains_perm.push_back(perm04[3]);
  dag_tasks.chains_ = {{0, 1}, {0, 4}};
  GraphOfChains graph_of_all_ca_chains(dag_tasks.chains_);
  VariableRange variable_od_range(FindVariableRange(dag_tasks));
  variable_od_range.lower_bound.print();
  variable_od_range.upper_bound.print();
  Interval intv1 =
      GetDeadlineRange_RTPerm(variable_od_range, perm01[0]->inequality_);
  EXPECT_EQ(10, intv1.start);
  EXPECT_EQ(-300 + 379, intv1.start + intv1.length);

  Interval intv2 =
      GetDeadlineRange_RTPerm(variable_od_range, perm04[1]->inequality_);
  EXPECT_EQ(10, intv2.start);
  EXPECT_EQ(100, intv2.start + intv2.length);
  EXPECT_FALSE(chains_perm.IsPermConflicted_CheckAllWithSameSource(
      variable_od_range, *perm04[1], graph_of_all_ca_chains));
}

TEST_F(PermutationTest6, IsPermConflicted_CheckAllWithSameSource_ret_false) {
  // task 0,1,3's periods are 100,400,100
  TwoTaskPermutations perm10(1, 0, dag_tasks, tasks_info, "ReactionTime");
  TwoTaskPermutations perm13(1, 3, dag_tasks, tasks_info, "ReactionTime");
  perm10[0]->print();
  perm13[4]->print();

  ChainsPermutation chains_perm;
  chains_perm.push_back(perm10[0]);
  // chains_perm.push_back(perm04[3]);
  dag_tasks.chains_ = {{1, 0}, {1, 3}};
  GraphOfChains graph_of_all_ca_chains(dag_tasks.chains_);
  VariableRange variable_od_range(FindVariableRange(dag_tasks));
  variable_od_range.lower_bound.print();
  variable_od_range.upper_bound.print();
  Interval intv1 =
      GetDeadlineRange_RTPerm(variable_od_range, perm10[0]->inequality_);
  EXPECT_EQ(21, intv1.start);
  EXPECT_EQ(0 + 90, intv1.start + intv1.length);

  Interval intv2 =
      GetDeadlineRange_RTPerm(variable_od_range, perm13[4]->inequality_);
  EXPECT_EQ(300, intv2.start);
  EXPECT_EQ(400, intv2.start + intv2.length);
  EXPECT_TRUE(chains_perm.IsPermConflicted_CheckAllWithSameSource(
      variable_od_range, *perm13[4], graph_of_all_ca_chains));
}

TEST_F(PermutationTest6, IsPermConflicted_CheckAllWithSameSink) {
  TwoTaskPermutations perm41(4, 1, dag_tasks, tasks_info, "ReactionTime");
  perm01[0]->print();
  perm41[1]->print();
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm01[0]);
  dag_tasks.chains_ = {{0, 1}, {4, 1}};
  GraphOfChains graph_of_all_ca_chains(dag_tasks.chains_);
  VariableRange variable_od_range(FindVariableRange(dag_tasks));
  Interval intv1 =
      GetOffsetRange_RTPerm(variable_od_range, perm01[0]->inequality_);
  EXPECT_EQ(310, intv1.start);
  EXPECT_EQ(379, intv1.start + intv1.length);  // before clap: 369 + 1 + 100

  Interval intv2 =
      GetOffsetRange_RTPerm(variable_od_range, perm41[1]->inequality_);
  EXPECT_EQ(200 + 60, intv2.start);
  EXPECT_EQ(379, intv2.start + intv2.length);  // before clap: 300 + 500
  EXPECT_FALSE(chains_perm.IsPermConflicted_CheckAllWithSameSink(
      variable_od_range, *perm41[1], graph_of_all_ca_chains));
}

TEST_F(PermutationTest6, IsPermConflicted_CheckAllWithSameSink_ret_false) {
  // task 0,1,3's periods are 100,400,100
  TwoTaskPermutations perm01(0, 1, dag_tasks, tasks_info, "ReactionTime");
  TwoTaskPermutations perm31(3, 1, dag_tasks, tasks_info, "ReactionTime");
  perm01[0]->print();
  perm31[4]->print();

  ChainsPermutation chains_perm;
  chains_perm.push_back(perm01[0]);
  dag_tasks.chains_ = {{0, 1}, {3, 1}};
  GraphOfChains graph_of_all_ca_chains(dag_tasks.chains_);
  VariableRange variable_od_range(FindVariableRange(dag_tasks));
  Interval intv1 =
      GetOffsetRange_RTPerm(variable_od_range, perm01[0]->inequality_);
  EXPECT_EQ(300 + 10, intv1.start);
  EXPECT_EQ(379, intv1.start + intv1.length);  // before clap 369 + 1 + 100

  Interval intv2 =
      GetOffsetRange_RTPerm(variable_od_range, perm31[4]->inequality_);
  EXPECT_EQ(0, intv2.start);  // before clap 10 + 11 + 12 + 13 - 100
  EXPECT_EQ(100, intv2.start + intv2.length);
  EXPECT_TRUE(chains_perm.IsPermConflicted_CheckAllWithSameSink(
      variable_od_range, *perm31[4], graph_of_all_ca_chains));
}
TEST_F(PermutationTest6, IsPermConflicted_CheckAllSerialConnect) {
  // task 0,1,3's periods are 100,400,100
  TwoTaskPermutations perm43(4, 3, dag_tasks, tasks_info, "ReactionTime");
  TwoTaskPermutations perm04(0, 4, dag_tasks, tasks_info, "ReactionTime");
  perm04.print();
  perm43.print();
  std::cout << "****************************\n";
  // perm04[0]->print();
  // perm43[0]->print();
  dag_tasks.chains_ = {{4, 3}, {0, 4}};
  GraphOfChains graph_of_all_ca_chains(dag_tasks.chains_);
  VariableRange variable_od_range(FindVariableRange(dag_tasks));
  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);

  ChainsPermutation chains_perm;
  chains_perm.push_back(perm04[0]);
  EXPECT_TRUE(chains_perm.IsPermConflicted_CheckAllSerialConnect(
      variable_od_range, *perm43[0], graph_of_all_ca_chains, rta));
  EXPECT_TRUE(chains_perm.IsPermConflicted_CheckAllSerialConnect(
      variable_od_range, *perm43[1], graph_of_all_ca_chains, rta));
  EXPECT_TRUE(chains_perm.IsPermConflicted_CheckAllSerialConnect(
      variable_od_range, *perm43[2], graph_of_all_ca_chains, rta));
  EXPECT_TRUE(chains_perm.IsPermConflicted_CheckAllSerialConnect(
      variable_od_range, *perm43[3], graph_of_all_ca_chains, rta));

  chains_perm.clear();
  chains_perm.push_back(perm04[5]);
  EXPECT_FALSE(chains_perm.IsPermConflicted_CheckAllSerialConnect(
      variable_od_range, *perm43[0], graph_of_all_ca_chains, rta));
  EXPECT_FALSE(chains_perm.IsPermConflicted_CheckAllSerialConnect(
      variable_od_range, *perm43[1], graph_of_all_ca_chains, rta));
  EXPECT_FALSE(chains_perm.IsPermConflicted_CheckAllSerialConnect(
      variable_od_range, *perm43[2], graph_of_all_ca_chains, rta));
  EXPECT_FALSE(chains_perm.IsPermConflicted_CheckAllSerialConnect(
      variable_od_range, *perm43[3], graph_of_all_ca_chains, rta));
}

TEST_F(PermutationTest6, IsPermConflicted_CheckAllSerialConnectDA) {
  // task 0,1,3's periods are 100,400,100
  TwoTaskPermutations perm43(4, 3, dag_tasks, tasks_info, "DataAge");
  TwoTaskPermutations perm04(0, 4, dag_tasks, tasks_info, "DataAge");
  perm04.print();
  perm43.print();
  std::cout << "****************************\n";
  // perm04[0]->print();
  // perm43[0]->print();
  dag_tasks.chains_ = {{4, 3}, {0, 4}};
  GraphOfChains graph_of_all_ca_chains(dag_tasks.chains_);
  VariableRange variable_od_range(FindVariableRange(dag_tasks));
  std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
  ChainsPermutation chains_perm;
  chains_perm.push_back(perm04[0]);
  EXPECT_TRUE(chains_perm.IsPermConflicted_CheckAllSerialConnect(
      variable_od_range, *perm43[0], graph_of_all_ca_chains, rta));
  EXPECT_TRUE(chains_perm.IsPermConflicted_CheckAllSerialConnect(
      variable_od_range, *perm43[1], graph_of_all_ca_chains, rta));
  EXPECT_TRUE(chains_perm.IsPermConflicted_CheckAllSerialConnect(
      variable_od_range, *perm43[2], graph_of_all_ca_chains, rta));
  EXPECT_TRUE(chains_perm.IsPermConflicted_CheckAllSerialConnect(
      variable_od_range, *perm43[3], graph_of_all_ca_chains, rta));

  chains_perm.clear();
  chains_perm.push_back(perm04[5]);
  EXPECT_FALSE(chains_perm.IsPermConflicted_CheckAllSerialConnect(
      variable_od_range, *perm43[0], graph_of_all_ca_chains, rta));
  EXPECT_FALSE(chains_perm.IsPermConflicted_CheckAllSerialConnect(
      variable_od_range, *perm43[1], graph_of_all_ca_chains, rta));
  EXPECT_FALSE(chains_perm.IsPermConflicted_CheckAllSerialConnect(
      variable_od_range, *perm43[2], graph_of_all_ca_chains, rta));
  EXPECT_FALSE(chains_perm.IsPermConflicted_CheckAllSerialConnect(
      variable_od_range, *perm43[3], graph_of_all_ca_chains, rta));
}

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}