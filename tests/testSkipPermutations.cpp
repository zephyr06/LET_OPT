#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/ObjectiveFunction/ObjectiveFunction.h"
#include "sources/Optimization/ChainsPermutation.h"
#include "sources/Optimization/PermutationInequality.h"
#include "sources/Optimization/TaskSetPermutation.h"
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
        task_sets_perms = TaskSetPermutation(dag_tasks, dag_tasks.chains_);
    };

    DAG_Model dag_tasks;
    TaskSet tasks;
    TaskSetInfoDerived tasks_info;
    TaskSetPermutation task_sets_perms;
};

TEST_F(PermutationTest1, obj_) {
    TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
    TwoTaskPermutations perm24(2, 4, dag_tasks, tasks_info);
    TwoTaskPermutations perm23(2, 3, dag_tasks, tasks_info);
    ChainsPermutation chain_perm;
    chain_perm.push_back(perm12[8]);
    chain_perm.push_back(perm24[11]);
    chain_perm.push_back(perm23[3]);
    chain_perm.print();

    std::unordered_map<JobCEC, JobCEC> react_chain_map;
    std::pair<VariableOD, int> res =
        FindODWithLP(task_sets_perms.dag_tasks_, task_sets_perms.tasks_info_,
                     chain_perm, task_sets_perms.graph_of_all_ca_chains_,
                     "ReactionTime", react_chain_map, task_sets_perms.rta_);

    std::vector<std::vector<int>> sub_chains =
        GetSubChains(task_sets_perms.dag_tasks_.chains_, chain_perm);
    double obj_curr = ObjReactionTime::Obj(
        task_sets_perms.dag_tasks_, task_sets_perms.tasks_info_, chain_perm,
        task_sets_perms.best_possible_variable_od_, sub_chains);

    ObjReactionTimeIntermediate objRI;
    EXPECT_EQ(1889 - 723,
              objRI.ObjSingleChain(dag_tasks, tasks_info, chain_perm, {1, 2, 4},
                                   task_sets_perms.best_possible_variable_od_));

    EXPECT_EQ(200 + 14 - 723,
              objRI.ObjSingleChain(dag_tasks, tasks_info, chain_perm, {1, 2, 3},
                                   task_sets_perms.best_possible_variable_od_));

    EXPECT_THAT(obj_curr, testing::Le(res.second));
}

// TEST_F(PermutationTest1, SKIP_perm1) {
//     TwoTaskPermutations perm12(1, 2, dag_tasks, tasks_info);
//     TwoTaskPermutations perm24(2, 4, dag_tasks, tasks_info);
//     TwoTaskPermutations perm23(2, 3, dag_tasks, tasks_info);
//     ChainsPermutation chain_perm;
//     chain_perm.push_back(perm12[73]);
//     chain_perm.push_back(perm24[11]);
//     chain_perm.push_back(perm23[19]);

//     std::unordered_map<JobCEC, JobCEC> react_chain_map;
//     std::pair<VariableOD, int> res =
//         FindODWithLP(task_sets_perms.dag_tasks_, task_sets_perms.tasks_info_,
//                      chain_perm, task_sets_perms.graph_of_all_ca_chains_,
//                      "ReactionTime", react_chain_map, task_sets_perms.rta_);
//     EXPECT_EQ(1459, res.second);
// }

int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}