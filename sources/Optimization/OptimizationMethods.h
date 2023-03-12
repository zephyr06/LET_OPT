#pragma once

#include "sources/Optimization/TaskSetPermutation.h"
#include "sources/Utils/BatchUtils.h"

namespace DAG_SPACE {

template <typename ObjectiveFunctionBase>
ScheduleResult PerformTOM_OPT(const DAG_Model& dag_tasks) {
    auto start = std::chrono::high_resolution_clock::now();

    ScheduleResult res;
    TaskSetPermutation task_sets_perms =
        TaskSetPermutation(dag_tasks, dag_tasks.chains_[0]);
    res.obj_ = task_sets_perms.PerformOptimization();
    res.schedulable_ = task_sets_perms.ExamSchedulabilityOptSol();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    res.timeTaken_ = double(duration.count()) / 1e6;
    return res;
}

}  // namespace DAG_SPACE