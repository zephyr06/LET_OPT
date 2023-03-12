#pragma once

#include "sources/TaskModel/RegularTasks.h"
#include "sources/Utils/MatirxConvenient.h"
#include "sources/Utils/profilier.h"

namespace DAG_SPACE {
using namespace RegularTaskSystem;

/**
 * @brief RTA_BASE encapsulate all the interafaces required
 *  by Energy_optimization.
 * All kinds of RTA should inherit
 * from RTA_BASE and implement its virtual function
 *
 */
class RTA_BASE {
   public:
    TaskSet tasks_;
    RTA_BASE() {}
    RTA_BASE(const TaskSet &tasks) : tasks_(tasks) {}

    /**
     * @brief
     *
     * @param tasks are inverse priority ordered
     * @param index
     * @return double
     */
    virtual double RTA_Common_Warm(double beginTime, int index) {
        CoutError("Calling RTA_Common_Warm that is supposed to be overwriten!");
        return 0;
    }

    inline int UnschedulableRTA(const Task &task) {
        return task.deadline + 1e4 * GlobalVariablesDAGOpt::TIME_SCALE_FACTOR;
    }
};

}  // namespace DAG_SPACE