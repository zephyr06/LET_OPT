#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/OptimizeMain.h"
#include "sources/Permutations/PermutationInequality.h"
#include "sources/Permutations/TwoTaskPermutations.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Interval.h"
#include "sources/Utils/JobCEC.h"
#include "tests/testEnv.cpp"
using namespace DAG_SPACE;

class PermutationTest2 : public ::testing::Test {
 protected:
  void SetUp() override {
    perm1 = PermutationInequality(0, 1, 0, true, 10, true, "ReactionTime");
    perm2 = PermutationInequality(0, 1, 10, true, 30, true, "ReactionTime");
    perm3 = PermutationInequality(0, 1, 0, true, 5, true, "ReactionTime");
    perm4 = PermutationInequality(0, 1, 11, true, 20, true, "ReactionTime");
    perm5 = PermutationInequality(0, 1, 5, true, 20, true, "ReactionTime");

    perm6 = PermutationInequality(0, 1, 11, false, 20, true, "ReactionTime");
    perm7 = PermutationInequality(0, 1, 5, false, 20, true, "ReactionTime");
    perm8 = PermutationInequality(0, 1, 30, true, 20, false, "ReactionTime");
    perm9 = PermutationInequality(0, 1, 5, true, 15, false, "ReactionTime");
  };

  PermutationInequality perm1;
  PermutationInequality perm2;
  PermutationInequality perm3;
  PermutationInequality perm4;
  PermutationInequality perm5;
  PermutationInequality perm6;
  PermutationInequality perm7;
  PermutationInequality perm8;
  PermutationInequality perm9;
};

TEST_F(PermutationTest2, ExamConfliction_and_WhetherAdjacent) {
  EXPECT_TRUE(ExamConfliction(perm1, perm2));
  EXPECT_TRUE(ExamConfliction(perm2, perm1));
  EXPECT_FALSE(ExamConfliction(perm4, perm2));
  EXPECT_TRUE(ExamConfliction(perm4, perm3));
  EXPECT_TRUE(ExamConfliction(perm1, perm4));

  EXPECT_FALSE(ExamConfliction(perm6, perm7));
  EXPECT_FALSE(ExamConfliction(perm6, perm7));
  EXPECT_TRUE(ExamConfliction(perm8, perm7));
}

TEST_F(PermutationTest2, MergeSingleValue) {
  EXPECT_EQ(10, MergeSmallerThanValue(perm1, perm2));
  EXPECT_EQ(5, MergeSmallerThanValue(perm6, perm9));
  EXPECT_EQ(-1e9, MergeSmallerThanValue(perm6, perm7));

  EXPECT_EQ(10, MergeLargerThanValue(perm1, perm2));
  EXPECT_EQ(20, MergeLargerThanValue(perm6, perm9));
  EXPECT_EQ(1e9, MergeLargerThanValue(perm8, perm9));
}

TEST_F(PermutationTest2, MergeTwoSinglePermutations) {
  PermutationInequality merged_perm = MergeTwoSinglePermutations(perm1, perm2);
  EXPECT_FALSE(merged_perm.IsValid());

  merged_perm = MergeTwoSinglePermutations(perm1, perm3);
  EXPECT_TRUE(merged_perm.IsValid());
  EXPECT_EQ(0, merged_perm.lower_bound_);
  EXPECT_EQ(5, merged_perm.upper_bound_);

  merged_perm = MergeTwoSinglePermutations(perm1, perm4);
  EXPECT_FALSE(merged_perm.IsValid());

  merged_perm = MergeTwoSinglePermutations(perm1, perm5);
  EXPECT_TRUE(merged_perm.IsValid());
  EXPECT_EQ(5, merged_perm.lower_bound_);
  EXPECT_EQ(10, merged_perm.upper_bound_);

  merged_perm = MergeTwoSinglePermutations(perm6, perm7);
  EXPECT_TRUE(merged_perm.IsValid());
  EXPECT_FALSE(merged_perm.lower_bound_valid_);
  // EXPECT_EQ(-1e9, merged_perm.lower_bound_);
  EXPECT_EQ(20, merged_perm.upper_bound_);

  merged_perm = MergeTwoSinglePermutations(perm6, perm8);
  EXPECT_FALSE(merged_perm.IsValid());

  merged_perm = MergeTwoSinglePermutations(perm6, perm9);
  EXPECT_TRUE(merged_perm.IsValid());
  EXPECT_EQ(5, merged_perm.lower_bound_);
  EXPECT_EQ(20, merged_perm.upper_bound_);
}

TEST_F(PermutationTest2, isValid) {
  EXPECT_TRUE(perm1.IsValid());

  PermutationInequality perm10(0, 1, 0, true, 0, true, "ReactionTime");
  EXPECT_FALSE(perm10.IsValid());
  perm10 = PermutationInequality(0, 1, 100, false, 20, true, "ReactionTime");
  EXPECT_TRUE(perm10.IsValid());
  perm10 = PermutationInequality(0, 1, 100, true, 20, false, "ReactionTime");
  EXPECT_TRUE(perm10.IsValid());
  perm10 = PermutationInequality(0, 1, 0, true, -5, true, "ReactionTime");
  EXPECT_FALSE(perm10.IsValid());
  perm10 = PermutationInequality(0, 1, 0, false, -5, false, "ReactionTime");
  EXPECT_TRUE(perm10.IsValid());
}

class PermutationTest1 : public ::testing::Test {
 protected:
  void SetUp() override {
    dag_tasks = ReadDAG_Tasks(
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v18.csv",
        "orig", 1);
    tasks = dag_tasks.GetTaskSet();
    tasks_info = TaskSetInfoDerived(tasks);
    chain1 = {0, 2};
    dag_tasks.chains_[0] = chain1;
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
  std::vector<int> chain1;
  Task task0;
  Task task1;
  Task task2;
  JobCEC job00;
  JobCEC job01;
  JobCEC job10;
  JobCEC job20;
};

TEST_F(PermutationTest1, GetPossibleReactingJobs_same_period) {
  int superperiod = GetSuperPeriod(task1, task2);
  auto reacting_jobs =
      GetPossibleReactingJobs(JobCEC(1, 0), task2, superperiod, tasks_info);
  EXPECT_EQ(2, reacting_jobs.size());
  EXPECT_EQ(0, reacting_jobs[0].jobId);
  EXPECT_EQ(1, reacting_jobs[1].jobId);
}
TEST_F(PermutationTest1, GetPossibleReactingJobs_harmonic_period) {
  int superperiod = GetSuperPeriod(task0, task1);
  auto reacting_jobs =
      GetPossibleReactingJobs(JobCEC(0, 0), task1, superperiod, tasks_info);
  EXPECT_EQ(2, reacting_jobs.size());
  EXPECT_EQ(0, reacting_jobs[0].jobId);
  EXPECT_EQ(1, reacting_jobs[1].jobId);

  reacting_jobs =
      GetPossibleReactingJobs(JobCEC(1, 0), task0, superperiod, tasks_info);
  EXPECT_EQ(3, reacting_jobs.size());
  EXPECT_EQ(0, reacting_jobs[0].jobId);
  EXPECT_EQ(2, reacting_jobs[reacting_jobs.size() - 1].jobId);

  reacting_jobs =
      GetPossibleReactingJobs(JobCEC(0, 1), task1, superperiod, tasks_info);
  EXPECT_EQ(2, reacting_jobs.size());
  EXPECT_EQ(0, reacting_jobs[0].jobId);
  EXPECT_EQ(1, reacting_jobs[1].jobId);
}

TEST_F(PermutationTest1, PermutationInequality_constructor) {
  PermutationInequality perm1(job00, job10, tasks_info, "ReactionTime");
  EXPECT_EQ(0, perm1.upper_bound_);
  EXPECT_TRUE(perm1.upper_bound_valid_);
  EXPECT_EQ(-20, perm1.lower_bound_);
  EXPECT_TRUE(perm1.lower_bound_valid_);

  perm1 = PermutationInequality(job01, job10, tasks_info, "ReactionTime");
  EXPECT_EQ(-10, perm1.upper_bound_);
  EXPECT_TRUE(perm1.upper_bound_valid_);
  EXPECT_EQ(-30, perm1.lower_bound_);
  EXPECT_TRUE(perm1.lower_bound_valid_);
}

class PermutationTest3 : public ::testing::Test {
 protected:
  void SetUp() override {
    dag_tasks = ReadDAG_Tasks(
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v2.csv", "orig",
        1);
    tasks = dag_tasks.GetTaskSet();
    tasks_info = TaskSetInfoDerived(tasks);
    task0 = tasks[0];
    task1 = tasks[1];
    task2 = tasks[2];
  };

  DAG_Model dag_tasks;
  TaskSet tasks;
  TaskSetInfoDerived tasks_info;
  Task task0;
  Task task1;
  Task task2;
};

TEST_F(PermutationTest3, GetPossibleReactingJobs_non_harmonic_period) {
  int superperiod = GetSuperPeriod(task0, task1);
  auto reacting_jobs =
      GetPossibleReactingJobs(JobCEC(0, 0), task1, superperiod, tasks_info);
  EXPECT_EQ(2, reacting_jobs.size());
  EXPECT_EQ(0, reacting_jobs[0].jobId);
  EXPECT_EQ(1, reacting_jobs[1].jobId);

  reacting_jobs =
      GetPossibleReactingJobs(JobCEC(0, 1), task1, superperiod, tasks_info);
  EXPECT_EQ(3, reacting_jobs.size());
  EXPECT_EQ(0, reacting_jobs[0].jobId);
  EXPECT_EQ(2, reacting_jobs.back().jobId);

  reacting_jobs =
      GetPossibleReactingJobs(JobCEC(0, 2), task1, superperiod, tasks_info);
  EXPECT_EQ(2, reacting_jobs.size());
  EXPECT_EQ(1, reacting_jobs[0].jobId);
  EXPECT_EQ(2, reacting_jobs[1].jobId);
}

TEST_F(PermutationTest1, simple_contructor_harmonic) {
  TwoTaskPermutations two_task_permutation(1, 2, dag_tasks, tasks_info,
                                           "ReactionTime");
  EXPECT_EQ(2, two_task_permutation.size());
  // PermutationInequality perm_expected0(1, 2, 0, false, 0, true);
  EXPECT_EQ(0, two_task_permutation[0]->inequality_.upper_bound_);
  // PermutationInequality perm_expected1(1, 2, 0, false, 20, true);
  EXPECT_EQ(20, two_task_permutation[1]->inequality_.upper_bound_);

  two_task_permutation =
      TwoTaskPermutations(0, 2, dag_tasks, tasks_info, "ReactionTime");

  EXPECT_TRUE(
      JobCEC(2, 0) ==
      two_task_permutation[0]->job_first_react_matches_.at(JobCEC(0, 0)));
  EXPECT_TRUE(
      JobCEC(2, 0) ==
      two_task_permutation[0]->job_first_react_matches_.at(JobCEC(0, 1)));
  EXPECT_EQ(3, two_task_permutation.size());
  EXPECT_EQ(-10, two_task_permutation[0]->inequality_.upper_bound_);

  EXPECT_TRUE(
      JobCEC(2, 0) ==
      two_task_permutation[1]->job_first_react_matches_.at(JobCEC(0, 0)));
  EXPECT_TRUE(
      JobCEC(2, 1) ==
      two_task_permutation[1]->job_first_react_matches_.at(JobCEC(0, 1)));
  EXPECT_EQ(-10, two_task_permutation[1]->inequality_.lower_bound_);
  EXPECT_EQ(0, two_task_permutation[1]->inequality_.upper_bound_);

  EXPECT_TRUE(
      JobCEC(2, 1) ==
      two_task_permutation[2]->job_first_react_matches_.at(JobCEC(0, 0)));
  EXPECT_TRUE(
      JobCEC(2, 1) ==
      two_task_permutation[2]->job_first_react_matches_.at(JobCEC(0, 1)));
  EXPECT_EQ(0, two_task_permutation[2]->inequality_.lower_bound_);
  EXPECT_EQ(10, two_task_permutation[2]->inequality_.upper_bound_);
}

TEST_F(PermutationTest1, simple_contructor_harmonic_v2) {
  TwoTaskPermutations two_task_permutation =
      TwoTaskPermutations(2, 0, dag_tasks, tasks_info, "ReactionTime");
  EXPECT_EQ(3, two_task_permutation.size());

  int permutation_index = 0;
  EXPECT_TRUE(JobCEC(0, 0) == two_task_permutation[permutation_index]
                                  ->job_first_react_matches_.at(JobCEC(2, 0)));
  EXPECT_THAT(two_task_permutation[permutation_index]->inequality_.lower_bound_,
              testing::Ge(-10));
  EXPECT_EQ(0,
            two_task_permutation[permutation_index]->inequality_.upper_bound_);

  permutation_index++;
  EXPECT_TRUE(JobCEC(0, 1) == two_task_permutation[permutation_index]
                                  ->job_first_react_matches_.at(JobCEC(2, 0)));
  EXPECT_EQ(0,
            two_task_permutation[permutation_index]->inequality_.lower_bound_);
  EXPECT_EQ(10,
            two_task_permutation[permutation_index]->inequality_.upper_bound_);

  permutation_index++;
  EXPECT_TRUE(JobCEC(0, 2) == two_task_permutation[permutation_index]
                                  ->job_first_react_matches_.at(JobCEC(2, 0)));
  EXPECT_EQ(10,
            two_task_permutation[permutation_index]->inequality_.lower_bound_);
  EXPECT_EQ(20,
            two_task_permutation[permutation_index]->inequality_.upper_bound_);
}

TEST_F(PermutationTest3, simple_contructor_non_harmonic) {
  TwoTaskPermutations two_task_permutation(0, 1, dag_tasks, tasks_info,
                                           "ReactionTime");
  EXPECT_EQ(5, two_task_permutation.size());

  int permutation_index = 0;
  EXPECT_TRUE(
      JobCEC(1, 0) ==
      two_task_permutation[0]->job_first_react_matches_.at(JobCEC(0, 0)));
  EXPECT_TRUE(
      JobCEC(1, 0) ==
      two_task_permutation[0]->job_first_react_matches_.at(JobCEC(0, 1)));
  EXPECT_TRUE(
      JobCEC(1, 1) ==
      two_task_permutation[0]->job_first_react_matches_.at(JobCEC(0, 2)));
  EXPECT_THAT(two_task_permutation[0]->inequality_.lower_bound_,
              testing::Ge(-15));
  // EXPECT_EQ(-15, two_task_permutation[0].inequality_.lower_bound_);
  EXPECT_EQ(-10, two_task_permutation[0]->inequality_.upper_bound_);

  permutation_index++;
  EXPECT_TRUE(JobCEC(1, 0) == two_task_permutation[permutation_index]
                                  ->job_first_react_matches_.at(JobCEC(0, 0)));
  EXPECT_TRUE(JobCEC(1, 1) == two_task_permutation[permutation_index]
                                  ->job_first_react_matches_.at(JobCEC(0, 1)));
  EXPECT_TRUE(JobCEC(1, 1) == two_task_permutation[permutation_index]
                                  ->job_first_react_matches_.at(JobCEC(0, 2)));
  EXPECT_EQ(-10,
            two_task_permutation[permutation_index]->inequality_.lower_bound_);
  EXPECT_EQ(-5,
            two_task_permutation[permutation_index]->inequality_.upper_bound_);

  permutation_index++;
  EXPECT_TRUE(JobCEC(1, 0) == two_task_permutation[permutation_index]
                                  ->job_first_react_matches_.at(JobCEC(0, 0)));
  EXPECT_TRUE(JobCEC(1, 1) == two_task_permutation[permutation_index]
                                  ->job_first_react_matches_.at(JobCEC(0, 1)));
  EXPECT_TRUE(JobCEC(1, 2) == two_task_permutation[permutation_index]
                                  ->job_first_react_matches_.at(JobCEC(0, 2)));
  EXPECT_EQ(-5,
            two_task_permutation[permutation_index]->inequality_.lower_bound_);
  EXPECT_EQ(0,
            two_task_permutation[permutation_index]->inequality_.upper_bound_);

  // **********************************************************
  permutation_index++;
  EXPECT_TRUE(JobCEC(1, 1) == two_task_permutation[permutation_index]
                                  ->job_first_react_matches_.at(JobCEC(0, 0)));
  EXPECT_TRUE(JobCEC(1, 1) == two_task_permutation[permutation_index]
                                  ->job_first_react_matches_.at(JobCEC(0, 1)));
  EXPECT_TRUE(JobCEC(1, 2) == two_task_permutation[permutation_index]
                                  ->job_first_react_matches_.at(JobCEC(0, 2)));
  EXPECT_EQ(0,
            two_task_permutation[permutation_index]->inequality_.lower_bound_);
  EXPECT_EQ(5,
            two_task_permutation[permutation_index]->inequality_.upper_bound_);

  permutation_index++;
  EXPECT_TRUE(JobCEC(1, 1) == two_task_permutation[permutation_index]
                                  ->job_first_react_matches_.at(JobCEC(0, 0)));
  EXPECT_TRUE(JobCEC(1, 2) == two_task_permutation[permutation_index]
                                  ->job_first_react_matches_.at(JobCEC(0, 1)));
  EXPECT_TRUE(JobCEC(1, 2) == two_task_permutation[permutation_index]
                                  ->job_first_react_matches_.at(JobCEC(0, 2)));
  EXPECT_EQ(5,
            two_task_permutation[permutation_index]->inequality_.lower_bound_);
  EXPECT_EQ(10,
            two_task_permutation[permutation_index]->inequality_.upper_bound_);
}

class PermutationTest4 : public ::testing::Test {
 protected:
  void SetUp() override {
    dag_tasks = ReadDAG_Tasks(
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n30_v3.csv",
        "orig", 1);
    tasks = dag_tasks.GetTaskSet();
    tasks_info = TaskSetInfoDerived(tasks);
    task0 = tasks[23];  // period 2000, execution time 36
    task1 = tasks[22];  // period 1000, execution time 11
    task2 = tasks[18];
  };

  DAG_Model dag_tasks;
  TaskSet tasks;
  TaskSetInfoDerived tasks_info;
  Task task0;
  Task task1;
  Task task2;
};

TEST_F(PermutationTest4, GenerateBoxPermutationConstraints) {
  TwoTaskPermutations two_task_permutation(23, 22, dag_tasks, tasks_info,
                                           "ReactionTime");
  PermutationInequality perm_bound = GenerateBoxPermutationConstraints(
      23, 22, two_task_permutation.variable_od_range_, "ReactionTime");
  perm_bound.print();
  EXPECT_FALSE(perm_bound.IsValid());
}

TEST_F(PermutationTest4, simple_contructor_v1) {
  task0.print();
  task1.print();
  std::cout << "RTA-22-old: " << GetResponseTime(tasks, 22) << "\n";
  std::cout << "RTA-22: " << GetResponseTime(dag_tasks, 22) << "\n";
  std::cout << "RTA-23: " << GetResponseTime(dag_tasks, 23) << "\n";
  TwoTaskPermutations two_task_permutation(23, 22, dag_tasks, tasks_info,
                                           "ReactionTime");
  EXPECT_EQ(0, two_task_permutation.size());
}

class PermutationTest5 : public ::testing::Test {
 protected:
  void SetUp() override {
    dag_tasks = ReadDAG_Tasks(
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v16.csv",
        "orig", 1);
    tasks = dag_tasks.GetTaskSet();
    tasks_info = TaskSetInfoDerived(tasks);
    task0 = tasks[3];  // period 10, execution time 1
    task1 = tasks[2];  // period 10000, execution time 547
  };

  DAG_Model dag_tasks;
  TaskSet tasks;
  TaskSetInfoDerived tasks_info;
  Task task0;
  Task task1;
  Task task2;
};

TEST_F(PermutationTest5, GetPossibleReactingJobs) {
  std::vector<JobCEC> react_jobs =
      GetPossibleReactingJobs(JobCEC(3, 946), task1, 1e4, tasks_info);
  EXPECT_EQ(1, react_jobs.size());
  react_jobs = GetPossibleReactingJobs(JobCEC(3, 947), task1, 1e4, tasks_info);
  EXPECT_EQ(1, react_jobs.size());
}

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}