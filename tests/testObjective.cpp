#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/PermutationInequality.h"
#include "sources/Optimization/TaskSetPermutation.h"
#include "sources/Optimization/TwoTaskPermutations.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Interval.h"
#include "sources/Utils/JobCEC.h"
using namespace DAG_SPACE;

class ChainPermutation {
   public:
    ChainPermutation() {}

    inline size_t size() const { return permutation_chain_.size(); }
    inline void push_back(const SinglePairPermutation &perm) {
        permutation_chain_.push_back(perm);
    }
    inline void reserve(size_t n) { permutation_chain_.reserve(n); }
    bool IsValid() const { return true; }
    SinglePairPermutation operator[](size_t i) const {
        if (i >= size()) CoutError("Out-of-range error!");
        return permutation_chain_[i];
    }

    int GetTaskId(size_t i) const {
        if (i < size())
            return permutation_chain_[i].inequality_.task_prev_id_;
        else if (i == size()) {
            return permutation_chain_[size() - 1].inequality_.task_next_id_;
        } else
            CoutError("Out-or-range error in ChainPermutation!");
        return -1;
    }

    // data members
    std::vector<SinglePairPermutation> permutation_chain_;
};

class ObjectiveFunctionBase {
   public:
    static const std::string type_trait;

    static double Obj(const DAG_Model &dag_tasks,
                      const TaskSetInfoDerived &tasks_info,
                      const ChainPermutation &chain_perm) {
        CoutError("Base function should not be called!");
        return 0;
    }
};
const std::string ObjectiveFunctionBase::type_trait("ObjectiveFunctionBase");

class ObjReactionTime : public ObjectiveFunctionBase {
   public:
    static const std::string type_trait;
    static double Obj(const DAG_Model &dag_tasks,
                      const TaskSetInfoDerived &tasks_info,
                      const ChainPermutation &chain_perm);
};

const std::string ObjReactionTime::type_trait("ReactionTime");
double ObjReactionTime::Obj(const DAG_Model &dag_tasks,
                            const TaskSetInfoDerived &tasks_info,
                            const ChainPermutation &chain_perm) {
    for (uint i = 0; i < tasks_info.sizeOfVariables[chain_perm.GetTaskId(0)];
         i++)
        ;
    return 0;
}

class PermutationTest1 : public ::testing::Test {
   protected:
    void SetUp() override {
        dag_tasks = ReadDAG_Tasks(
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_n3_v18.csv",
            "orig", 1);
        tasks = dag_tasks.tasks;
        tasks_info = TaskSetInfoDerived(tasks);
        task0 = tasks[0];
        task1 = tasks[1];
        task2 = tasks[2];
        job00 = JobCEC(0, 0);
        job01 = JobCEC(0, 1);
        job10 = JobCEC(1, 0);
        job20 = JobCEC(2, 0);

        perm01 = TwoTaskPermutations(0, 1, tasks_info);
        perm12 = TwoTaskPermutations(1, 2, tasks_info);
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

TEST_F(PermutationTest1, GetTaskId) {
    ChainPermutation chain_perm;
    chain_perm.push_back(perm01[0]);
    chain_perm.push_back(perm12[0]);
    EXPECT_EQ(0, chain_perm.GetTaskId(0));
    EXPECT_EQ(1, chain_perm.GetTaskId(1));
    EXPECT_EQ(2, chain_perm.GetTaskId(2));
}

// TEST_F(PermutationTest1, ChainPermutation_v1) {
//     // chain is 0 -> 1 -> 2
//     TwoTaskPermutations perm01(0, 1, tasks_info);
//     TwoTaskPermutations perm12(1, 2, tasks_info);
//     perm01[0].print();
//     perm01[1].print();
//     perm01[2].print();
//     perm12[0].print();
//     perm12[1].print();

//     ChainPermutation chain_perm;
//     chain_perm.push_back(perm01[0]);
//     chain_perm.push_back(perm12[0]);
//     EXPECT_EQ(20, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm));

//     chain_perm.permutation_chain_.clear();
//     chain_perm.push_back(perm01[0]);
//     chain_perm.push_back(perm12[1]);
//     EXPECT_EQ(40, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm));

//     chain_perm.permutation_chain_.clear();
//     chain_perm.push_back(perm01[1]);
//     chain_perm.push_back(perm12[1]);
//     EXPECT_EQ(50, ObjReactionTime::Obj(dag_tasks, tasks_info, chain_perm));
// }

int main(int argc, char **argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}