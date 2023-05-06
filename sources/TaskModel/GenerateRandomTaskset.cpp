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
      deadline = round(
          RandRange(std::max(1.0, ceil(periodCurr * utilVec[i])), periodCurr));
    Task task(0, periodCurr, 0, std::max(1.0, ceil(periodCurr * utilVec[i])),
              deadline, i, processorId, coreRequire);
    tasks.push_back(task);
  }
  return tasks;
}
void WriteTaskSets(std::ofstream &file, const TaskSet &tasks) {
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
DAG_Model GenerateDAG(const TaskSetGenerationParameters &tasks_params) {
  double totalUtilization = RandRange(tasks_params.totalUtilization_min,
                                      tasks_params.totalUtilization_max);
  TaskSet tasks = GenerateTaskSet(
      tasks_params.N, totalUtilization, tasks_params.numberOfProcessor,
      tasks_params.coreRequireMax, tasks_params.period_generation_type,
      tasks_params.deadlineType);
  MAP_Prev mapPrev;
  DAG_Model dagModel(tasks, mapPrev, 0);
  // add edges randomly
  for (int i = 0; i < tasks_params.N; i++) {
    for (int j = i + 1; j < tasks_params.N; j++) {
      if (double(rand()) / RAND_MAX < tasks_params.parallelismFactor) {
        dagModel.addEdge(i, j);
      }
    }
  }
  return DAG_Model(tasks, dagModel.mapPrev, tasks_params.numCauseEffectChain,
                   tasks_params.chain_length, tasks_params.SF_ForkNum,
                   tasks_params.fork_sensor_num);
}

void WriteDAG(std::ofstream &file, DAG_Model &dag_tasks) {
  WriteTaskSets(file, dag_tasks.GetTaskSet());
  file << "Note: "
       << "*a,b"
       << " means a->b, i.e., a writes data and b reads data from it\n";
  for (const auto &chain : dag_tasks.chains_) {
    file << "@Chain:";
    for (int x : chain) file << x << ", ";
    file << "\n";
  }
  for (const auto &sf_fork : dag_tasks.sf_forks_) {
    file << "@Fork_source:";
    for (int x : sf_fork.source) file << x << ", ";
    file << "\n";
    file << "@Fork_sink:" << sf_fork.sink << ", \n";
  }
  for (auto itr = dag_tasks.mapPrev.begin(); itr != dag_tasks.mapPrev.end();
       itr++) {
    for (uint i = 0; i < itr->second.size(); i++)
      file << "*" << ((itr->second)[i].id) << "," << (itr->first) << "\n";
  }
}