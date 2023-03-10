#pragma once

#include "sources/Optimization/ChainPermutation.h"
#include "sources/RTA/RTA_LL.h"
#include "sources/TaskModel/DAG_Model.h"

namespace DAG_SPACE {
// offset and deadline
struct OD_Vir {
    int offset;
    int deadline;
};

class VariableOD {
   public:
    VariableOD() {}
    VariableOD(const TaskSet& tasks_) {
        variables_.reserve(tasks_.size());
        for (uint i = 0; i < tasks_.size(); i++) {
            variables_.push_back(OD_Vir{0, tasks_[i].deadline});
        }
        valid_ = true;
    }
    inline OD_Vir operator[](uint index) const { return variables_[index]; }
    bool SetDeadline(uint index, int value) {
        if (value <= variables_[index].deadline) {
            variables_[index].deadline = value;
            return true;
        } else {
            valid_ = false;
            return false;
        }
    }

    // assume offset is already set
    bool SetMinDeadline(uint index, const TaskSet& tasks) {
        int rta = GetResponseTime(tasks, index);
        return SetDeadline(index, variables_[index].offset + rta);
    }

    void SetOffset(uint index, int value) {
        if (value > variables_[index].offset) variables_[index].offset = value;
    }

    // data members
    std::vector<OD_Vir> variables_;
    bool valid_;
};

// assume the simple response time analysis
VariableOD FindODFromPermutation(const TaskSetInfoDerived& tasks_info,
                                 const ChainPermutation& chain_perm,
                                 std::vector<int> task_chain);
}  // namespace DAG_SPACE