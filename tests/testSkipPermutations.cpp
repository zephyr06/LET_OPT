#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
#include "sources/Optimization/ChainsPermutation.h"
#include "sources/Optimization/PermutationInequality.h"
#include "sources/Optimization/TaskSetPermutationEnumerate.h"
#include "sources/Optimization/TwoTaskPermutations.h"
#include "sources/Optimization/Variable.h"
#include "sources/RTA/RTA_LL.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Interval.h"
#include "sources/Utils/JobCEC.h"

using namespace DAG_SPACE;

class PermutationTest1 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n5_v30.csv",
            "RM", 2);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
        // task_sets_perms = TaskSetPermutationEnumerate(dag_tasks, dag_tasks.chains_);
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
    // TaskSetPermutationEnumerate task_sets_perms;
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
    TaskSetPermutationEnumerate task_sets_perms(dag_tasks, dag_tasks.chains_);
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
    EXPECT_EQ(1889 - 723, objRI.ObjSingleChain(
                              dag_tasks, tasks_info, chains_perm, {1, 2, 4},
                              task_sets_perms.best_possible_variable_od_));

    EXPECT_EQ(
        0, objRI.ObjSingleChain(dag_tasks, tasks_info, chains_perm, {1, 2, 3},
                                task_sets_perms.best_possible_variable_od_));

    EXPECT_THAT(obj_curr, testing::Le(res.second));
}

// TODO: add it to TaskSetPermutationEnumerate
TEST_F(PermutationTest1, FindBestPossibleVariableOD) {
    TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
    TwoTaskPermutations perm24(2, 4, dag_tasks, tasks_info);
    TwoTaskPermutations perm23(2, 3, dag_tasks, tasks_info);
    ChainsPermutation chains_perm;
    chains_perm.push_back(perm12[8]);
    chains_perm.push_back(perm24[11]);
    chains_perm.push_back(perm23[3]);
    chains_perm.print();

    TaskSetPermutationEnumerate task_sets_perms(dag_tasks, dag_tasks.chains_);
    VariableRange variable_range = FindPossibleVariableOD(
        dag_tasks, tasks_info, task_sets_perms.rta_, chains_perm);

    EXPECT_EQ(0, variable_range.lower_bound[1].offset);
    EXPECT_EQ(341, variable_range.lower_bound[1].deadline);  // modified
    EXPECT_EQ(723, variable_range.upper_bound[1].offset);
    EXPECT_EQ(359, variable_range.upper_bound[1].deadline);

    EXPECT_EQ(0, variable_range.lower_bound[2].offset);
    EXPECT_EQ(1, variable_range.lower_bound[2].deadline);
    EXPECT_EQ(9, variable_range.upper_bound[2].offset);
    EXPECT_EQ(10, variable_range.upper_bound[2].deadline);

    EXPECT_EQ(151, variable_range.lower_bound[3].offset);  // modified
    EXPECT_EQ(14, variable_range.lower_bound[3].deadline);
    EXPECT_EQ(159 + 10, variable_range.upper_bound[3].offset);  // modified
    EXPECT_EQ(200, variable_range.upper_bound[3].deadline);

    EXPECT_EQ(0, variable_range.lower_bound[4].offset);
    EXPECT_EQ(889, variable_range.lower_bound[4].deadline);
    EXPECT_EQ(9, variable_range.upper_bound[4].offset);  // modified
    EXPECT_EQ(1000, variable_range.upper_bound[4].deadline);

    VariableOD variable_od = FindBestPossibleVariableOD(
        dag_tasks, tasks_info, task_sets_perms.rta_, chains_perm);

    EXPECT_EQ(723, variable_od[1].offset);
    EXPECT_EQ(341, variable_od[1].deadline);
    EXPECT_EQ(9, variable_od[2].offset);
    EXPECT_EQ(1, variable_od[2].deadline);
    EXPECT_EQ(169, variable_od[3].offset);
    EXPECT_EQ(14, variable_od[3].deadline);
    EXPECT_EQ(9, variable_od[4].offset);
    EXPECT_EQ(889, variable_od[4].deadline);
}

class PermutationTest2 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v19.csv",
            "RM", 2);
        tasks = dag_tasks.GetTaskSet();
        tasks_info = TaskSetInfoDerived(tasks);
        // task_sets_perms = TaskSetPermutationEnumerate(dag_tasks, dag_tasks.chains_);
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
    // TaskSetPermutationEnumerate task_sets_perms;
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

    TaskSetPermutationEnumerate task_sets_perms(dag_tasks, dag_tasks.chains_);
    VariableRange variable_range = FindPossibleVariableOD(
        dag_tasks, tasks_info, task_sets_perms.rta_, chains_perm);
    EXPECT_EQ(0, variable_range.lower_bound[0].offset);
    EXPECT_EQ(1, variable_range.lower_bound[0].deadline);
    EXPECT_EQ(9, variable_range.upper_bound[0].offset);
    EXPECT_EQ(10, variable_range.upper_bound[0].deadline);

    EXPECT_EQ(0, variable_range.lower_bound[1].offset);
    EXPECT_EQ(3, variable_range.lower_bound[1].deadline);
    EXPECT_EQ(17, variable_range.upper_bound[1].offset);
    EXPECT_EQ(13, variable_range.upper_bound[1].deadline);  // modified

    EXPECT_EQ(3, variable_range.lower_bound[2].offset);  // modified
    EXPECT_EQ(7, variable_range.lower_bound[2].deadline);
    EXPECT_EQ(13, variable_range.upper_bound[2].offset);
    EXPECT_EQ(20, variable_range.upper_bound[2].deadline);
}
// TEST_F(PermutationTest1, SKIP_perm1) {
//     TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
//     TwoTaskPermutations perm24(2, 4, dag_tasks, tasks_info);
//     TwoTaskPermutations perm23(2, 3, dag_tasks, tasks_info);
//     ChainsPermutation chains_perm;
//     chains_perm.push_back(perm12[73]);
//     chains_perm.push_back(perm24[11]);
//     chains_perm.push_back(perm23[19]);

//     std::unordered_map<JobCEC, JobCEC> react_chain_map;
//     std::pair<VariableOD, int> res =
//         FindODWithLP(task_sets_perms.dag_tasks_, task_sets_perms.tasks_info_,
//                      chains_perm, task_sets_perms.graph_of_all_ca_chains_,
//                      "ReactionTime", react_chain_map, task_sets_perms.rta_);
//     EXPECT_EQ(1459, res.second);
// }

int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}