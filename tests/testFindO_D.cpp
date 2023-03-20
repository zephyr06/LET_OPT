#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/ChainsPermutation.h"
#include "sources/Optimization/TaskSetPermutation.h"
#include "sources/Optimization/TwoTaskPermutations.h"
#include "sources/Optimization/Variable.h"
#include "sources/RTA/RTA_LL.h"
#include "sources/TaskModel/DAG_Model.h"
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

        perm01 = TwoTaskPermutations(0, 1, dag_tasks, tasks_info);
        perm12 = TwoTaskPermutations(1, 2, dag_tasks, tasks_info);
        perm02 = TwoTaskPermutations(0, 2, dag_tasks, tasks_info);

        // perm01[0].print();
        // perm01[1].print();
        // perm01[2].print();
        // perm12[0].print();
        // perm12[1].print();
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
    TwoTaskPermutations perm02;
};

TEST_F(PermutationTest1, FindODFromSingleChainPermutation) {
    std::vector<int> task_chain = {0, 1, 2};
    // RTA: 1, 3, 6
    ChainsPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    perm01[0].print();
    chain_perm.push_back(perm12[0]);
    perm12[0].print();

    GraphOfChains graph_chains({task_chain});
    VariableOD variable =
        FindODFromSingleChainPermutation(dag_tasks, chain_perm, graph_chains);
    EXPECT_TRUE(variable.valid_);
    EXPECT_EQ(0, variable[0].offset);
    EXPECT_EQ(1, variable[0].deadline);
    EXPECT_EQ(11, variable[1].offset);
    EXPECT_EQ(14, variable[1].deadline);
    EXPECT_EQ(14, variable[2].offset);
    EXPECT_EQ(20, variable[2].deadline);
}

TEST_F(PermutationTest1, FindODFromSingleChainPermutation_v2) {
    std::vector<int> task_chain = {0, 1, 2};
    // RTA: 1, 3, 6
    ChainsPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    perm01[0].print();
    chain_perm.push_back(perm12[0]);
    perm12[0].print();

    GraphOfChains graph_chains({task_chain});
    VariableOD variable =
        FindODFromSingleChainPermutation(dag_tasks, chain_perm, graph_chains);
    EXPECT_TRUE(variable.valid_);
    EXPECT_EQ(0, variable[0].offset);
    EXPECT_EQ(1, variable[0].deadline);
    EXPECT_EQ(11, variable[1].offset);
    EXPECT_EQ(14, variable[1].deadline);
    EXPECT_EQ(14, variable[2].offset);
    EXPECT_EQ(20, variable[2].deadline);
}
TEST_F(PermutationTest1, FindODFromSingleChainPermutation_graph_v1) {
    // RTA: 1, 3, 6
    std::vector<std::vector<int>> task_chains = {{0, 1}, {0, 2}};
    GraphOfChains graph_chains(task_chains);
    ChainsPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    perm01[0].print();
    chain_perm.push_back(perm02[0]);
    perm02[0].print();

    VariableOD variable =
        FindODFromSingleChainPermutation(dag_tasks, chain_perm, graph_chains);
    EXPECT_TRUE(variable.valid_);
    EXPECT_EQ(0, variable[0].offset);
    EXPECT_EQ(1, variable[0].deadline);
    EXPECT_EQ(11, variable[1].offset);
    EXPECT_EQ(14, variable[1].deadline);
    EXPECT_EQ(11, variable[2].offset);
    EXPECT_EQ(17, variable[2].deadline);
}

TEST_F(PermutationTest1, FindODFromSingleChainPermutation_graph_v2) {
    // RTA: 1, 3, 6
    std::vector<std::vector<int>> task_chains = {{0, 2}, {1, 2}};
    GraphOfChains graph_chains(task_chains);
    ChainsPermutation chain_perm;
    chain_perm.push_back(perm12[0]);
    perm12[0].print();
    chain_perm.push_back(perm02[0]);
    perm02[0].print();

    VariableOD variable =
        FindODFromSingleChainPermutation(dag_tasks, chain_perm, graph_chains);
    EXPECT_TRUE(variable.valid_);
    EXPECT_EQ(0, variable[0].offset);
    EXPECT_EQ(1, variable[0].deadline);
    EXPECT_EQ(0, variable[1].offset);
    EXPECT_EQ(3, variable[1].deadline);
    EXPECT_EQ(11, variable[2].offset);
    EXPECT_EQ(17, variable[2].deadline);
}

class PermutationTest2 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v6.csv",
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

        // perm01[0].print();
        // perm01[1].print();
        // perm01[2].print();
        // perm12[0].print();
        // perm12[1].print();
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
};

TEST_F(PermutationTest2, FindODFromSingleChainPermutation) {
    std::vector<int> task_chain = {0, 1, 2};
    // RTA: 1, 3, 6
    ChainsPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    perm01[0].print();
    chain_perm.push_back(perm12[0]);
    perm12[0].print();

    GraphOfChains graph_chains({task_chain});
    VariableOD variable =
        FindODFromSingleChainPermutation(dag_tasks, chain_perm, graph_chains);
    EXPECT_FALSE(variable.valid_);
    EXPECT_EQ(0, variable[0].offset);
    EXPECT_EQ(1, variable[0].deadline);
    EXPECT_EQ(11, variable[1].offset);
    EXPECT_EQ(14, variable[1].deadline);
    EXPECT_EQ(14, variable[2].offset);
    EXPECT_EQ(21, variable[2].deadline);
}

TEST_F(PermutationTest2, FindVariableRange) {
    // RTA: 1, 3, 6
    VariableRange range = FindVariableRange(dag_tasks);

    EXPECT_EQ(0, range.lower_bound[0].offset);
    EXPECT_EQ(9, range.upper_bound[0].offset);
    EXPECT_EQ(1, range.lower_bound[0].deadline);
    EXPECT_EQ(10, range.upper_bound[0].deadline);

    EXPECT_EQ(0, range.lower_bound[1].offset);
    EXPECT_EQ(17, range.upper_bound[1].offset);
    EXPECT_EQ(3, range.lower_bound[1].deadline);
    EXPECT_EQ(20, range.upper_bound[1].deadline);

    EXPECT_EQ(0, range.lower_bound[2].offset);
    EXPECT_EQ(13, range.upper_bound[2].offset);
    EXPECT_EQ(7, range.lower_bound[2].deadline);
    EXPECT_EQ(20, range.upper_bound[2].deadline);
}

class PermutationTest_Non_Har : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v2.csv",
            "orig", 1);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
        task0 = tasks[0];
        task1 = tasks[1];
        task2 = tasks[2];
        perm01 = TwoTaskPermutations(0, 1, dag_tasks, tasks_info);
        perm12 = TwoTaskPermutations(1, 2, dag_tasks, tasks_info);
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
    Task task0;
    Task task1;
    Task task2;
    TwoTaskPermutations perm01;
    TwoTaskPermutations perm12;
};

TEST_F(PermutationTest_Non_Har, FindODFromSingleChainPermutation_invalid) {
    std::vector<int> task_chain = {0, 1, 2};
    // RTA: 1, 3, 6
    ChainsPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    perm01[0].print();
    chain_perm.push_back(perm12[0]);
    perm12[0].print();

    GraphOfChains graph_chains({task_chain});
    VariableOD variable =
        FindODFromSingleChainPermutation(dag_tasks, chain_perm, graph_chains);
    EXPECT_FALSE(variable.valid_);
    EXPECT_EQ(0, variable[0].offset);
    EXPECT_EQ(1, variable[0].deadline);
    EXPECT_EQ(11, variable[1].offset);
    EXPECT_EQ(14, variable[1].deadline);
    EXPECT_EQ(14, variable[2].offset);
    EXPECT_EQ(20, variable[2].deadline);
}

TEST_F(PermutationTest_Non_Har, FindODFromSingleChainPermutation_valid) {
    std::vector<int> task_chain = {0, 1, 2};
    // RTA: 1, 3, 6
    ChainsPermutation chain_perm;
    chain_perm.push_back(perm01[1]);
    perm01[1].print();
    chain_perm.push_back(perm12[0]);
    perm12[0].print();

    GraphOfChains graph_chains({task_chain});
    VariableOD variable =
        FindODFromSingleChainPermutation(dag_tasks, chain_perm, graph_chains);
    EXPECT_TRUE(variable.valid_);
    EXPECT_EQ(0, variable[0].offset);
    EXPECT_EQ(1, variable[0].deadline);
    EXPECT_EQ(6, variable[1].offset);
    EXPECT_EQ(9, variable[1].deadline);
    EXPECT_EQ(9, variable[2].offset);
    EXPECT_EQ(15, variable[2].deadline);
}

class PermutationTest3 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v26.csv",
            "RM", 2);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
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

TEST_F(PermutationTest3, Find_OD) {
    TaskSetPermutation task_set_perms(dag_tasks, dag_tasks.chains_);
    ChainsPermutation chains_perm;
    chains_perm.push_back(task_set_perms.adjacent_two_task_permutations_[0][0]);
    chains_perm.push_back(task_set_perms.adjacent_two_task_permutations_[1][0]);
    chains_perm.push_back(task_set_perms.adjacent_two_task_permutations_[2][1]);
    chains_perm.push_back(task_set_perms.adjacent_two_task_permutations_[3][1]);
    task_set_perms.adjacent_two_task_permutations_[0][0].print();
    task_set_perms.adjacent_two_task_permutations_[1][0].print();
    task_set_perms.adjacent_two_task_permutations_[2][1].print();
    task_set_perms.adjacent_two_task_permutations_[3][1].print();

    VariableOD variable_od = FindODFromSingleChainPermutation(
        dag_tasks, chains_perm, task_set_perms.graph_of_all_ca_chains_);
    variable_od.print();

    std::unordered_map<JobCEC, JobCEC> react_chain_map;
    std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
    int index = 0;
    for (int x : rta) {
        std::cout << "RTA of task " << index++ << ": " << x << "\n";
    }
    auto res =
        FindODWithLP(task_set_perms.dag_tasks_, task_set_perms.tasks_info_,
                     chains_perm, task_set_perms.graph_of_all_ca_chains_,
                     "ReactionTime", react_chain_map, rta);
    EXPECT_EQ(res.first.valid_, variable_od.valid_);
}

TEST_F(PermutationTest3, Find_OD_v2) {
    TaskSetPermutation task_set_perms(dag_tasks, dag_tasks.chains_);
    ChainsPermutation chains_perm;
    chains_perm.push_back(task_set_perms.adjacent_two_task_permutations_[0][1]);
    chains_perm.push_back(task_set_perms.adjacent_two_task_permutations_[1][0]);
    chains_perm.push_back(task_set_perms.adjacent_two_task_permutations_[2][1]);
    chains_perm.push_back(task_set_perms.adjacent_two_task_permutations_[3][1]);
    task_set_perms.adjacent_two_task_permutations_[0][1].print();
    task_set_perms.adjacent_two_task_permutations_[1][0].print();
    task_set_perms.adjacent_two_task_permutations_[2][1].print();
    task_set_perms.adjacent_two_task_permutations_[3][1].print();

    VariableOD variable_od = FindODFromSingleChainPermutation(
        dag_tasks, chains_perm, task_set_perms.graph_of_all_ca_chains_);

    std::unordered_map<JobCEC, JobCEC> react_chain_map;
    std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
    auto res =
        FindODWithLP(task_set_perms.dag_tasks_, task_set_perms.tasks_info_,
                     chains_perm, task_set_perms.graph_of_all_ca_chains_,
                     "ReactionTime", react_chain_map, rta);
    // this is just the limitation of FindODFromSingleChainPermutation
    EXPECT_FALSE(res.first.valid_ != variable_od.valid_);
}

class PermutationTest4 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v28.csv",
            "RM", 2);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
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
TEST_F(PermutationTest4, Find_OD_v2) {
    TaskSetPermutation task_set_perms(dag_tasks, dag_tasks.chains_);
    ChainsPermutation chains_perm;
    chains_perm.push_back(task_set_perms.adjacent_two_task_permutations_[0][0]);
    chains_perm.push_back(task_set_perms.adjacent_two_task_permutations_[1][1]);
    chains_perm.push_back(task_set_perms.adjacent_two_task_permutations_[2][0]);
    chains_perm.push_back(task_set_perms.adjacent_two_task_permutations_[3][3]);
    chains_perm.print();

    VariableOD variable_od = FindODFromSingleChainPermutation(
        dag_tasks, chains_perm, task_set_perms.graph_of_all_ca_chains_);

    std::unordered_map<JobCEC, JobCEC> react_chain_map;
    std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
    int index = 0;
    for (int x : rta) {
        std::cout << "RTA of task " << index++ << ": " << x << "\n";
    }
    auto res =
        FindODWithLP(task_set_perms.dag_tasks_, task_set_perms.tasks_info_,
                     chains_perm, task_set_perms.graph_of_all_ca_chains_,
                     "ReactionTime", react_chain_map, rta);
    // this is just the limitation of FindODFromSingleChainPermutation
    EXPECT_EQ(res.first.valid_, variable_od.valid_);
    if (res.first.valid_ != variable_od.valid_ && res.first.valid_)
        res.first.print();
}

class PermutationTest5 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v29.csv",
            "RM", 2);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
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
TEST_F(PermutationTest5, Find_OD) {
    TaskSetPermutation task_set_perms(dag_tasks, dag_tasks.chains_);
    ChainsPermutation chains_perm;
    chains_perm.push_back(task_set_perms.adjacent_two_task_permutations_[0][1]);
    chains_perm.push_back(task_set_perms.adjacent_two_task_permutations_[1][2]);
    chains_perm.print();

    VariableOD variable_od = FindODFromSingleChainPermutation(
        dag_tasks, chains_perm, task_set_perms.graph_of_all_ca_chains_);

    std::unordered_map<JobCEC, JobCEC> react_chain_map;
    std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
    int index = 0;
    for (int x : rta) {
        std::cout << "RTA of task " << index++ << ": " << x << "\n";
    }
    auto res =
        FindODWithLP(task_set_perms.dag_tasks_, task_set_perms.tasks_info_,
                     chains_perm, task_set_perms.graph_of_all_ca_chains_,
                     "ReactionTime", react_chain_map, rta);
    // TOOD: pass this test
    // EXPECT_EQ(res.first.valid_, variable_od.valid_);
    // if (res.first.valid_ != variable_od.valid_ && res.first.valid_)
    //     res.first.print();
}

TEST_F(PermutationTest1, Find_OD) {
    std::vector<int> rta = GetResponseTimeTaskSet(dag_tasks);
    int index = 0;
    for (int x : rta) {
        std::cout << "RTA of task " << index++ << ": " << x << "\n";
    }
    VariableOD variable_od =
        FindBestPossibleVariableOD(dag_tasks, tasks_info, rta);
    EXPECT_EQ(9, variable_od[0].offset);
    EXPECT_EQ(1, variable_od[0].deadline);
    EXPECT_EQ(17, variable_od[1].offset);
    EXPECT_EQ(3, variable_od[1].deadline);
    EXPECT_EQ(14, variable_od[2].offset);
    EXPECT_EQ(6, variable_od[2].deadline);
}
int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}