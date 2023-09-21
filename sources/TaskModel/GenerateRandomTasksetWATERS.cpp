

#include "sources/TaskModel/GenerateRandomTasksetWATERS.h"
#include "sources/TaskModel/GenerateRandomTaskset.h"

void AddChains2DAG(DAG_Model &dag_tasks, int numCauseEffectChain) {
  RandomChainsGenerator generator(dag_tasks);
  dag_tasks.chains_ = generator.GenerateChain(numCauseEffectChain);
}

DAG_Model GenerateDAG_WATERS15(int N, double totalUtilization, int numberOfProcessor,
                               int coreRequireMax, int taskType, int deadlineType,
                               double parallelismFactor, int numCauseEffectChain,
                               int sf_fork_num, int fork_sensor_num_min, int fork_sensor_num_max) {
  TaskSet tasks = GenerateTaskSet(N, totalUtilization, numberOfProcessor,
                                  coreRequireMax, taskType, deadlineType);
  DAG_Model dagModel;
  dagModel.tasks = tasks;
  dagModel.RecordTaskPosition();

  AddEdges2DAG_He21(dagModel, N, parallelismFactor);

  dagModel.ConstructBGL_Graph();
  dagModel.CategorizeTaskSet();
  AddChains2DAG(dagModel, numCauseEffectChain);
  dagModel.sf_forks_ = dagModel.GetRandomForks(sf_fork_num, fork_sensor_num_min, fork_sensor_num_max);
  return dagModel;
}


DAG_Model GenerateDAG_WATERS15(TaskSetGenerationParameters tasks_params) {
  double totalUtilization = RandRange(tasks_params.totalUtilization_min,
                                      tasks_params.totalUtilization_max);
  return GenerateDAG_WATERS15(tasks_params.N, totalUtilization, tasks_params.numberOfProcessor,
      tasks_params.coreRequireMax, tasks_params.taskType, tasks_params.deadlineType,
      tasks_params.parallelismFactor, tasks_params.numCauseEffectChain,
      tasks_params.SF_ForkNum, tasks_params.fork_sensor_num_min, tasks_params.fork_sensor_num_max);
}
