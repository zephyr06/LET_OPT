#pragma once

#include "sources/RTA/RTA_BASE.h"
#include "sources/TaskModel/DAG_Model.h"

namespace DAG_SPACE {
class RTA_LL : public RTA_BASE {
   public:
    RTA_LL(const TaskSet &tasks) : RTA_BASE(tasks) {}

    double RTA_Common_Warm(double beginTime, int index) override {
        TaskSet tasksHp;
        for (int i = 0; i < index; i++) {
            tasksHp.push_back(tasks_.at(i));
        }
        return ResponseTimeAnalysisWarm(beginTime, tasks_.at(index), tasksHp);
    }

    // the rest are helper functions
    static std::string type() { return "LL"; }

    double ResponseTimeAnalysisWarm_util_nece(
        double beginTime, const Task &taskCurr,
        const TaskSet &tasksHighPriority) {
        const std::vector<int> periodHigh =
            GetParameter<int>(tasksHighPriority, "period");
        const std::vector<double> executionTimeHigh =
            GetParameter<double>(tasksHighPriority, "executionTime");
        int N = periodHigh.size();

        if (beginTime < 0) {
            if (GlobalVariablesDAGOpt::debugMode == 1) {
                CoutWarning(
                    "During optimization, some variables drop below 0\n");
            }
            beginTime = 0;
        }
        if (isnan(taskCurr.executionTime) || isnan(beginTime)) {
            std::cout << Color::red << "Nan executionTime detected"
                      << Color::def << std::endl;
            throw "Nan";
        }

        double utilAll = Utilization(tasksHighPriority);
        if (utilAll >= 1.0 - 1e-3) {
            // cout << "The given task set is unschedulable\n";
            return UnschedulableRTA(taskCurr);
        }

        bool stop_flag = false;

        double responseTimeBefore = beginTime;
        int loopCount = 0;
        while (not stop_flag) {
            double responseTime = taskCurr.executionTime;
            for (int i = 0; i < N; i++)
                responseTime +=
                    ceil(responseTimeBefore / double(periodHigh[i])) *
                    executionTimeHigh[i];
            if (responseTime == responseTimeBefore) {
                stop_flag = true;
                if (responseTime > taskCurr.period) {
                    return UnschedulableRTA(taskCurr);
                }
                return responseTime;
            } else {
                responseTimeBefore = responseTime;
            }
            loopCount++;
            if (loopCount > 1500) {
                CoutError("LoopCount error in RTA_LL");
                return UnschedulableRTA(taskCurr);
            }
        }
        std::cout << "RTA analysis stops unexpectedly!\n";
        throw;
    }

    double ResponseTimeAnalysisWarm(const double beginTime,
                                    const Task &taskCurr,
                                    const TaskSet &tasksHighPriority) {
        if (Utilization(tasksHighPriority) + taskCurr.utilization() >
            1.0 + 1e-6) {
            return UnschedulableRTA(taskCurr);
        }
        return ResponseTimeAnalysisWarm_util_nece(beginTime, taskCurr,
                                                  tasksHighPriority);
    }

    double ResponseTimeAnalysis(const Task &taskCurr,
                                const TaskSet &tasksHighPriority) {
        const std::vector<double> executionTimeHigh =
            GetParameter<double>(tasksHighPriority, "executionTime");
        double executionTimeAll = taskCurr.executionTime;
        for (auto &task : tasksHighPriority)
            executionTimeAll += task.executionTime;
        return ResponseTimeAnalysisWarm(executionTimeAll, taskCurr,
                                        tasksHighPriority);
    }
};

int GetResponseTime(const TaskSet &tasks, int index, int warm_start = 0);

int GetResponseTime(const DAG_Model &dag_tasks, int task_id);

bool CheckSchedulability(const DAG_Model &dag_tasks, bool whetherPrint = false,
                         double tol = 0);

// std::vector<int> GetResponseTimeOfChain(const TaskSet &tasks,
//                                         const std::vector<int> &chain);

}  // namespace DAG_SPACE