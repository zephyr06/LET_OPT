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

    /**
     * @brief
     *
     * @tparam Schedul_Analysis
     * @param tasks
     * @param warmStart
     * @param whetherPrint
     * @param tol: positive value, makes schedulability check more strict
     * @return true: system is schedulable
     * @return false: system is not schedulable
     */
    bool CheckSchedulability(VectorDynamic warmStart, bool whetherPrint = false,
                             double tol = 0) {
        int N = tasks_.size();
        for (int i = 0; i < N; i++) {
            double rta = RTA_Common_Warm(warmStart(i, 0), i);
            if (whetherPrint)
                std::cout << "response time for task " << i << " is " << rta
                          << " and deadline is "
                          << min(tasks_[i].deadline, tasks_[i].period)
                          << std::endl;
            if (rta + tol > min(tasks_[i].deadline, tasks_[i].period)) {
                if (whetherPrint) {
                    std::cout << "The current task set is not schedulable "
                                 "because of task "
                              << i << " "
                              << "!\n";
                }

                return false;
            }
        }
        if (whetherPrint) std::cout << std::endl;
        return true;
    }

    bool CheckSchedulability(bool whetherPrint = false) {
        VectorDynamic warmStart =
            GetParameterVD<double>(tasks_, "executionTime");
        return CheckSchedulability(warmStart, whetherPrint);
    }

    inline int UnschedulableRTA(const Task &task) {
        return task.deadline + 1e4 * GlobalVariablesDAGOpt::TIME_SCALE_FACTOR;
    }
};

}  // namespace DAG_SPACE