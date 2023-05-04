#include <sys/stat.h>

#include <cmath>
#include <iostream>

#include "sources/RTA/RTA_LL.h"
#include "sources/TaskModel/GenerateRandomTaskset.h"
#include "sources/Utils/argparse.hpp"
using namespace GlobalVariablesDAGOpt;
void deleteDirectoryContents(const std::string &dir_path) {
  for (const auto &entry : std::filesystem::directory_iterator(dir_path))
    std::filesystem::remove_all(entry.path());
}
int main(int argc, char *argv[]) {
  using namespace std;
  argparse::ArgumentParser program("program name");
  program.add_argument("-v", "--verbose");  // parameter packing

  program.add_argument("--task_number_in_tasksets")
      .default_value(5)
      .help("the number of tasks in DAG")
      .scan<'i', int>();
  program.add_argument("--taskSetNumber")
      .default_value(10)
      .help("the number DAGs to create")
      .scan<'i', int>();
  program.add_argument("--taskSetNameStartIndex")
      .default_value(0)
      .help("the start index of DAG's name to create")
      .scan<'i', int>();
  program.add_argument("--NumberOfProcessor")
      .default_value(2)
      .help("the NumberOfProcessor of tasks in DAG")
      .scan<'i', int>();
  program.add_argument("--per_core_utilization_min")
      .default_value(0.4)
      .help(
          "lower bound, the total utilization of each task set divided by the "
          "numerb of "
          "processors")
      .scan<'f', double>();
  program.add_argument("--per_core_utilization_max")
      .default_value(0.4)
      .help(
          "upper bound, the total utilization of each task set divided by the "
          "numerb of "
          "processors")
      .scan<'f', double>();
  program.add_argument("--taskType")
      .default_value(1)
      .help("type of tasksets, 0 means normal, 1 means DAG")
      .scan<'i', int>();
  program.add_argument("--deadlineType")
      .default_value(0)
      .help("type of tasksets, 0 means implicit, 1 means random")
      .scan<'i', int>();
  program.add_argument("--period_generation_type")
      .default_value(3)
      .help(
          "type of taskset period generation method, 1 means normal, 2 means "
          "automobile method, 3 means "
          "automobile with WATERS distribution")
      .scan<'i', int>();
  program.add_argument("--excludeUnschedulable")
      .default_value(1)
      .help(
          "whether exclude unschedulable task set on List Scheduler, default "
          "1")
      .scan<'i', int>();
  program.add_argument("--excludeDAGWithWongChainNumber")
      .default_value(1)
      .help("whether exclude dags that don't have edges, default 1")
      .scan<'i', int>();
  program.add_argument("--examChainsWithSharedNodes")
      .default_value(1)
      .help("whether exclude dags whose chains are independent, default 1")
      .scan<'i', int>();
  program.add_argument("--randomSeed")
      .default_value(-1)
      .help(
          "seed of random, negative means use current time as seed, "
          "otherwise means self-defined seed")
      .scan<'i', int>();
  program.add_argument("--outDir")
      .default_value(std::string("TaskData/dagTasks/"))
      .help("directory to save task sets, only within the root folder");
  program.add_argument("--clearOutputDir")
      .default_value(1)
      .help("whether clean the output directory")
      .scan<'i', int>();
  program.add_argument("--parallelismFactor")
      .default_value(0.4)
      .help("the parallelismFactor when generating random DAGs")
      .scan<'f', double>();
  program.add_argument("--numCauseEffectChain")
      .default_value(0)
      .help(
          "the number of random cause-effect chains, default value will read "
          "from config.yaml")
      .scan<'i', int>();
  program.add_argument("--chainLength")
      .default_value(0)
      .help("the length of random cause-effect chains ")
      .scan<'i', int>();
  program.add_argument("--chainLengthRatio")
      .default_value(0.0)
      .help(
          "the ratio of the length of random cause-effect chains over total "
          "number of tasks. will overwrite chainLength if greater than 0.")
      .scan<'f', double>();
  program.add_argument("--SF_ForkNum")
      .default_value(0)
      .help(
          "the number of forks that constitute sensor fusion objective "
          "functions")
      .scan<'i', int>();

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    exit(0);
  }

  int task_number_in_tasksets = program.get<int>("--task_number_in_tasksets");
  int DAG_taskSetNumber = program.get<int>("--taskSetNumber");
  int DAG_taskSetNameStartIndex = program.get<int>("--taskSetNameStartIndex");
  int numberOfProcessor = program.get<int>("--NumberOfProcessor");
  double per_core_utilization_min =
      program.get<double>("--per_core_utilization_min");
  double per_core_utilization_max =
      program.get<double>("--per_core_utilization_max");
  int taskType = program.get<int>("--taskType");
  int deadlineType = program.get<int>("--deadlineType");
  int period_generation_type = program.get<int>("--period_generation_type");
  int excludeUnschedulable = program.get<int>("--excludeUnschedulable");
  int excludeDAGWithWongChainNumber =
      program.get<int>("--excludeDAGWithWongChainNumber");
  int examChainsWithSharedNodes =
      program.get<int>("--examChainsWithSharedNodes");
  int randomSeed = program.get<int>("--randomSeed");
  double parallelismFactor = program.get<double>("--parallelismFactor");
  int numCauseEffectChain = program.get<int>("--numCauseEffectChain");
  int chainLength = program.get<int>("--chainLength");
  double chainLengthRatio = program.get<double>("--chainLengthRatio");
  if (chainLengthRatio >= 0.001) {
    chainLength = std::ceil(chainLengthRatio * task_number_in_tasksets);
  }
  std::string outDir = program.get<std::string>("--outDir");
  int clearOutputDir = program.get<int>("--clearOutputDir");
  int SF_ForkNum = program.get<int>("--SF_ForkNum");
  if (randomSeed < 0) {
    srand(time(0) + (int64_t)&chainLength);
  } else {
    srand(randomSeed);
  }
  if (numCauseEffectChain <= 0)
    numCauseEffectChain = GlobalVariablesDAGOpt::CHAIN_NUMBER;

  double totalUtilization_min = per_core_utilization_min * numberOfProcessor;
  double totalUtilization_max = per_core_utilization_max * numberOfProcessor;

  std::cout
      << "Task configuration: " << std::endl
      << "the number of tasks in DAG(--task_number_in_tasksets): "
      << task_number_in_tasksets << std::endl
      << "DAG_taskSetNumber(--taskSetNumber): " << DAG_taskSetNumber
      << std::endl
      << "DAG_taskSetNameStartIndex(--taskSetNameStartIndex): "
      << DAG_taskSetNameStartIndex << std::endl
      << "NumberOfProcessor(--NumberOfProcessor): " << numberOfProcessor
      << std::endl
      << "totalUtilization_min(--totalUtilization_min): "
      << totalUtilization_min << std::endl
      << "totalUtilization_max(--totalUtilization_max): "
      << totalUtilization_max << std::endl
      << "parallelismFactor (--parallelismFactor): " << parallelismFactor
      << std::endl
      << "taskType(--taskType), 0 means normal, 1 means DAG: " << taskType
      << std::endl
      << "deadlineType(--deadlineType), 1 means random, 0 means implicit: "
      << deadlineType << std::endl
      << "period_generation_type(--period_generation_type), 1 means normal, "
         "2 means AutoMobile, 3 "
         "means automobile with "
         "WATERS distribution: "
      << period_generation_type << std::endl
      << "excludeUnschedulable(--excludeUnschedulable): "
      << excludeUnschedulable << std::endl
      << "excludeDAGWithWongChainNumber(--excludeDAGWithWongChainNumber): "
      << excludeDAGWithWongChainNumber << std::endl
      << "examChainsWithSharedNodes(--examChainsWithSharedNodes): "
      << examChainsWithSharedNodes << std::endl
      << "randomSeed, negative will use current time, otherwise use the "
         "given seed(--randomSeed): "
      << randomSeed << std::endl
      << "outDir, directory to save task sets, only within the root folder "
         "(--outDir): "
      << outDir << std::endl
      << "clearOutputDir, whether clean the output directory "
         "(--clearOutputDir): "
      << clearOutputDir << std::endl
      << "numCauseEffectChain, the number of random cause-effect chains,"
         "default value will read from config.yaml (--numCauseEffectChain): "
      << numCauseEffectChain << std::endl
      << "chainLength, the length of random cause-effect "
         "chains, 0 means no length requirements (--chainLength): "
      << chainLength << std::endl
      << "chainLengthRatio, the ratio of random cause-effect chains length "
         "over the number of tasks in DAG, a value greater than 0 will "
         "overwrite "
         "chainLength. (--chainLengthRatio): "
      << chainLengthRatio << std::endl
      << "SF_FOrkNum, the number of forks (--SF_FOrkNum): " << SF_ForkNum
      << std::endl
      << std::endl;

  std::string outDirectory = GlobalVariablesDAGOpt::PROJECT_PATH + outDir;
  if (clearOutputDir > 0) {
    deleteDirectoryContents(outDirectory);
  }

  double totalUtilization = totalUtilization_min;
  for (int i = DAG_taskSetNameStartIndex; i < DAG_taskSetNumber; i++) {
    if (taskType == 1)  // DAG task set
    {
      TaskSetGenerationParameters tasks_params;
      tasks_params.N = task_number_in_tasksets;
      tasks_params.totalUtilization_min = totalUtilization_min;
      tasks_params.totalUtilization_max = totalUtilization_max;
      tasks_params.numberOfProcessor = numberOfProcessor;
      tasks_params.coreRequireMax = 1;
      tasks_params.parallelismFactor = parallelismFactor;
      tasks_params.period_generation_type = period_generation_type;
      tasks_params.deadlineType = deadlineType;
      tasks_params.numCauseEffectChain = numCauseEffectChain;
      tasks_params.chain_length = chainLength;
      tasks_params.SF_ForkNum = SF_ForkNum;
      DAG_Model dag_tasks = GenerateDAG(tasks_params);

      if (excludeDAGWithWongChainNumber == 1) {
        // TaskSet t = dag_tasks.GetTaskSet();
        // DAG_Model ttt(t, dag_tasks.mapPrev, 1e9, 1e9,
        //               numCauseEffectChain);
        if (dag_tasks.chains_.size() != numCauseEffectChain) {
          i--;
          continue;
        }
        if (examChainsWithSharedNodes && numCauseEffectChain != 1) {
          if (!WhetherDAGChainsShareNodes(dag_tasks)) {
            i--;
            continue;
          }
        }
      }

      if (excludeUnschedulable == 1) {
        const TaskSet &task_set = dag_tasks.GetTaskSet();
        Reorder(task_set, GlobalVariablesDAGOpt::priorityMode);
        if (!CheckSchedulability(dag_tasks)) {
          i--;
          continue;  // re-generate a new task set
        }
      }

      if (SF_ForkNum > 0) {
        if (dag_tasks.sf_forks_.size() < SF_ForkNum) {
          i--;
          continue;
        }
      }
      string fileName = GetTaskSetName(i, task_number_in_tasksets);
      //  "dag-set-N" + to_string(task_number_in_tasksets) +
      //                   "-" + string(3 - to_string(i).size(), '0') +
      //                   to_string(i) + "-syntheticJobs" + ".csv";
      std::ofstream myfile;
      myfile.open(outDirectory + fileName);
      WriteDAG(myfile, dag_tasks);
      myfile.close();

    } else
      CoutError("taskType is not recognized!");
  }

  return 0;
}
