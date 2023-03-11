#include "sources/TaskModel/GenerateRandomTaskset.h"

std::vector<double> Uunifast(int N, double utilAll, bool boundU) {
    double sumU = utilAll;
    std::vector<double> utilVec(N, 0);

    double nextU = 0;
    for (int i = 1; i < N; i++) {
        nextU = sumU * pow(double(rand()) / RAND_MAX, 1.0 / (N - 1));
        if (boundU) {
            utilVec[i - 1] = std::min(1.0, sumU - nextU);
            nextU += std::max(0.0, sumU - nextU - 1.0);
        } else {
            utilVec[i - 1] = sumU - nextU;
        }
        sumU = nextU;
    }
    utilVec[N - 1] = nextU;
    return utilVec;
}

TaskSet GenerateTaskSet(int N, double totalUtilization, int numberOfProcessor,
                        int coreRequireMax, int taskSetType, int deadlineType) {
    std::vector<double> utilVec = Uunifast(N, totalUtilization);
    TaskSet tasks;

    for (int i = 0; i < N; i++) {
        int periodCurr = 0;
        int processorId = rand() % numberOfProcessor;
        int coreRequire = 1 + rand() % (coreRequireMax);

        // automobile periods with WATERS distribution
        int probability = rand() % PeriodCDFWaters.back();
        int period_idx = 0;
        while (probability > PeriodCDFWaters[period_idx]) {
            period_idx++;
        }
        periodCurr = int(PeriodSetWaters[period_idx] *
                         GlobalVariablesDAGOpt::TIME_SCALE_FACTOR);
        double deadline = periodCurr;
        if (deadlineType == 1)
            deadline = round(RandRange(
                std::max(1.0, ceil(periodCurr * utilVec[i])), periodCurr));
        Task task(0, periodCurr, 0,
                  std::max(1.0, ceil(periodCurr * utilVec[i])), deadline, i,
                  processorId, coreRequire);
        tasks.push_back(task);
    }
    return tasks;
}
void WriteTaskSets(std::ofstream &file, TaskSet &tasks) {
    int N = tasks.size();
    file << "JobID,Period,ExecutionTime,DeadLine,processorId"
            "\n";
    for (int i = 0; i < N; i++) {
        file << tasks[i].id
             // << "," << tasks[i].offset
             << ","
             << tasks[i].period
             //  << "," << tasks[i].overhead
             << "," << tasks[i].executionTime << "," << tasks[i].deadline << ","
             << tasks[i].processorId

             // << "," << tasks[i].coreRequire
             << "\n";
    }
}

using namespace DAG_SPACE;
DAG_Model GenerateDAG(int N, double totalUtilization, int numberOfProcessor,
                      int coreRequireMax, double parallelismFactor,
                      int period_generation_type, int deadlineType) {
    TaskSet tasks =
        GenerateTaskSet(N, totalUtilization, numberOfProcessor, coreRequireMax,
                        period_generation_type, deadlineType);
    MAP_Prev mapPrev;
    DAG_Model dagModel(tasks, mapPrev);
    // add edges randomly
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            if (double(rand()) / RAND_MAX < parallelismFactor) {
                dagModel.addEdge(i, j);
            }
        }
    }
    // set SF bound and RTDA bound randomly
    TaskSetInfoDerived tasksInfo(tasks);
    double max_execution_time_in_task_set = 0;
    for (auto &task : tasks) {
        if (task.executionTime > max_execution_time_in_task_set) {
            max_execution_time_in_task_set = task.executionTime;
        }
    }
    double min_sf_bound = max_execution_time_in_task_set;
    double min_rtda_bound = max_execution_time_in_task_set;
    double sf_range = N * max_execution_time_in_task_set;
    double rtda_range = N * tasksInfo.hyper_period;
    dagModel.setSfBound(
        std::floor(min_sf_bound + (double(rand()) / RAND_MAX) * sf_range));
    dagModel.setRtdaBound(
        std::floor(min_rtda_bound + (double(rand()) / RAND_MAX) * rtda_range));
    return dagModel;
}

void WriteDAG(std::ofstream &file, DAG_Model &tasksDAG) {
    WriteTaskSets(file, tasksDAG.tasks);
    for (auto itr = tasksDAG.mapPrev.begin(); itr != tasksDAG.mapPrev.end();
         itr++) {
        for (uint i = 0; i < itr->second.size(); i++)
            file << "*" << (itr->first) << "," << ((itr->second)[i].id) << "\n";
    }
    file << "@SF_Bound:" << tasksDAG.GetSfBound() << "\n";
    file << "@RTDA_Bound:" << tasksDAG.GetRtdaBound() << "\n";
}