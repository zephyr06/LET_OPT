#pragma once

#include "sources/RTA/RTA_LL.h"
#include "sources/TaskModel/DAG_Model.h"

namespace DAG_SPACE {
// offset and deadline
struct OD_Vir {
    int offset;
    int deadline;
};

// variableOD organizes variables following the order by task id;
class VariableOD {
   public:
    VariableOD() {}
    VariableOD(const TaskSet& tasks_) {
        variables_.reserve(tasks_.size());
        for (uint i = 0; i < tasks_.size(); i++) {
            variables_[tasks_[i].id] = (OD_Vir{0, tasks_[i].deadline});
        }
        valid_ = true;
    }
    OD_Vir& operator[](uint task_id) {
        auto itr = variables_.find(task_id);
        if (itr == variables_.end())
            CoutError("Not found task id in VariableOD!");
        return variables_[task_id];
    }
    OD_Vir at(uint task_id) const {
        auto itr = variables_.find(task_id);
        if (itr == variables_.end())
            CoutError("Not found task id in VariableOD!");
        return variables_.at(task_id);
    }
    inline size_t size() const { return variables_.size(); }

    bool SetDeadline(uint task_id, int value) {
        if (value <= variables_[task_id].deadline) {
            variables_[task_id].deadline = value;
            return true;
        } else {
            valid_ = false;
            return false;
        }
    }

    // assume offset is already set
    bool SetMinDeadline(uint task_id, const DAG_Model& dag_tasks) {
        int rta = GetResponseTime(dag_tasks, task_id);
        return SetDeadline(task_id, variables_[task_id].offset + rta);
    }

    void SetOffset(uint task_id, int value) {
        if (value > variables_[task_id].offset)
            variables_[task_id].offset = value;
    }

    void print() const;
    // data members
    bool valid_;

   private:
    std::unordered_map<int, OD_Vir> variables_;
};

struct VariableRange {
    VariableOD lower_bound;
    VariableOD upper_bound;
};

VariableRange FindVariableRange(const DAG_Model& dag_tasks);
}  // namespace DAG_SPACE