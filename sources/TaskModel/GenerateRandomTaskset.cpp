#include "sources/TaskModel/GenerateRandomTaskset.h"

std::vector<double> Uunifast(int N, double utilAll, bool boundU)
{
    double sumU = utilAll;
    std::vector<double> utilVec(N, 0);

    double nextU = 0;
    for (int i = 1; i < N; i++)
    {

        nextU = sumU * pow(double(rand()) / RAND_MAX, 1.0 / (N - 1));
        if (boundU)
        {
            utilVec[i - 1] = std::min(1.0, sumU - nextU);
            nextU += std::max(0.0, sumU - nextU - 1.0);
        }
        else
        {
            utilVec[i - 1] = sumU - nextU;
        }
        sumU = nextU;
    }
    utilVec[N - 1] = nextU;
    return utilVec;
}

TaskSet GenerateTaskSet(int N, double totalUtilization,
                        int numberOfProcessor, int periodMin,
                        int periodMax, int coreRequireMax,
                        int taskSetType, int deadlineType)
{
    std::vector<double> utilVec = Uunifast(N, totalUtilization);
    TaskSet tasks;
    int periodMaxRatio = periodMax / periodMin;

    for (int i = 0; i < N; i++)
    {

        int periodCurr = 0;
        int processorId = rand() % numberOfProcessor;
        int coreRequire = 1 + rand() % (coreRequireMax);
        if (taskSetType == 1) // fully random task set
        {
            periodCurr = (1 + rand() % periodMaxRatio) * periodMin;
            double deadline = periodCurr;
            if (deadlineType == 1)
                deadline = RandRange(ceil(periodCurr * utilVec[i]), periodCurr);
            Task task(0, periodCurr,
                      0, std::max(1.0, ceil(periodCurr * utilVec[i])),
                      deadline, i,
                      processorId, coreRequire);
            tasks.push_back(task);
        }
        else if (taskSetType == 2) // random choice from AutoMobile set 'PeriodSetAM'
        {
            periodCurr = int(PeriodSetAM[rand() % PeriodSetAM.size()] * GlobalVariablesDAGOpt::timeScaleFactor);
            double deadline = periodCurr;
            if (deadlineType == 1)
                deadline = round(RandRange(std::max(1.0, ceil(periodCurr * utilVec[i])), periodCurr));
            Task task(0, periodCurr,
                      0, std::max(1.0, ceil(periodCurr * utilVec[i])),
                      deadline, i,
                      processorId, coreRequire);
            tasks.push_back(task);
        }
        else if (taskSetType == 3) // automobile periods with WATERS distribution
        {
            int probability = rand() % PeriodCDFWaters.back();
            int period_idx = 0;
            while (probability > PeriodCDFWaters[period_idx])
            {
                period_idx++;
            }
            periodCurr = int(PeriodSetWaters[period_idx] * GlobalVariablesDAGOpt::timeScaleFactor);
            double deadline = periodCurr;
            if (deadlineType == 1)
                deadline = round(RandRange(std::max(1.0, ceil(periodCurr * utilVec[i])), periodCurr));
            Task task(0, periodCurr,
                      0, std::max(1.0, ceil(periodCurr * utilVec[i])),
                      deadline, i,
                      processorId, coreRequire);
            tasks.push_back(task);
        }
        else
            CoutError("Not recognized taskSetType!");
    }
    return tasks;
}
void WriteTaskSets(std::ofstream &file, TaskSet &tasks)
{
    int N = tasks.size();
    file << "JobID,Offset,Period,Overhead,ExecutionTime,DeadLine,processorId,coreRequire\n";
    for (int i = 0; i < N; i++)
    {
        file << tasks[i].id << "," << tasks[i].offset << ","
             << tasks[i].period << "," << tasks[i].overhead << ","
             << tasks[i].executionTime << "," << tasks[i].deadline
             << "," << tasks[i].processorId
             << "," << tasks[i].coreRequire << "\n";
    }
}

using namespace OrderOptDAG_SPACE;
DAG_Model GenerateDAG(int N, double totalUtilization,
                      int numberOfProcessor, int periodMin,
                      int periodMax, int coreRequireMax,
                      int taskSetType, int deadlineType)
{

    TaskSet tasks = GenerateTaskSet(N, totalUtilization,
                                    numberOfProcessor, periodMin, periodMax, coreRequireMax, taskSetType, deadlineType);
    MAP_Prev mapPrev;
    DAG_Model dagModel(tasks, mapPrev);
    // add edges randomly
    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            if (double(rand()) / RAND_MAX < GlobalVariablesDAGOpt::parallelFactor)
            {
                dagModel.addEdge(i, j);
            }
        }
    }
    // set SF bound and RTDA bound randomly
    TaskSetInfoDerived tasksInfo(tasks);
    double max_execution_time_in_task_set = 0;
    for (auto &task : tasks)
    {
        if (task.executionTime > max_execution_time_in_task_set)
        {
            max_execution_time_in_task_set = task.executionTime;
        }
    }
    double min_sf_bound = max_execution_time_in_task_set;
    double min_rtda_bound = max_execution_time_in_task_set;
    double sf_range = N * max_execution_time_in_task_set;
    double rtda_range = N * tasksInfo.hyperPeriod;
    dagModel.setSfBound(std::floor(min_sf_bound + (double(rand()) / RAND_MAX) * sf_range));
    dagModel.setRtdaBound(std::floor(min_rtda_bound + (double(rand()) / RAND_MAX) * rtda_range));
    return dagModel;
}

void WriteDAG(std::ofstream &file, DAG_Model &tasksDAG)
{
    WriteTaskSets(file, tasksDAG.tasks);
    for (auto itr = tasksDAG.mapPrev.begin(); itr != tasksDAG.mapPrev.end(); itr++)
    {
        for (uint i = 0; i < itr->second.size(); i++)
            file << "*" << (itr->first) << "," << ((itr->second)[i].id) << "\n";
    }
    file << "@SF_Bound:" << tasksDAG.GetSfBound() << "\n";
    file << "@RTDA_Bound:" << tasksDAG.GetRtdaBound() << "\n";
}