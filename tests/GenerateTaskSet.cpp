#include <sys/stat.h>

#include <cmath>
#include <iostream>

#include "sources/Baseline/StandardLET.h"
#include "sources/ObjectiveFunction/ObjSensorFusion.h"
#include "sources/RTA/RTA_LL.h"
#include "sources/TaskModel/GenerateRandomTaskset.h"
#include "sources/TaskModel/GenerateRandomTasksetWATERS.h"
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
  program.add_argument("--numberOfProcessor")
      .default_value(4)
      .help("the numberOfProcessor of tasks in DAG")
      .scan<'i', int>();
  program.add_argument("--per_core_utilization_min")
      .default_value(0.4)
      .help(
          "lower bound, the total utilization of each task set divided by the "
          "numerb of "
          "processors")
      .scan<'f', double>();
  program.add_argument("--per_core_utilization_max")
      .default_value(0.9)
      .help(
          "upper bound, the total utilization of each task set divided by the "
          "numerb of "
          "processors")
      .scan<'f', double>();
  program.add_argument("--taskSetType")
      .default_value(2)
      .help(
          "type of tasksets, 0 means normal, 1 means DAG with random chains, "
          "2 means DAG with chains conforms to WATERS15 distribution")
      .scan<'i', int>();
  program.add_argument("--deadlineType")
      .default_value(0)
      .help("type of tasksets, 0 means implicit, 1 means random")
      .scan<'i', int>();
  program.add_argument("--taskType")
      .default_value(2)
      .help(
          "type of task period generation method, 0 means normal, 1 means "
          "random choice from predefined set, 2 means "
          "automobile task with WATERS distribution")
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
      .help("the maximum parallelismFactor when generating random DAGs")
      .scan<'f', double>();
  program.add_argument("--numCauseEffectChain")
      .default_value(-1)
      .help(
          "the number of random cause-effect chains, a negative number will"
          "use random number of chains")
      .scan<'i', int>();
  program.add_argument("--chainLength")
      .default_value(0)
      .help(
          "the length of random cause-effect chains, only be effective when "
          "taskSetType is 1")
      .scan<'i', int>();
  program.add_argument("--chainLengthRatio")
      .default_value(0.0)
      .help(
          "the ratio of the length of random cause-effect chains over total "
          "number of tasks. will overwrite chainLength if greater than 0")
      .scan<'f', double>();
  program.add_argument("--SF_ForkNum")
      .default_value(-1)
      .help(
          "the number of forks that constitute sensor fusion objective "
          "functions, a negative number will generate random number of Fork "
          "range from 0.25N to N, default -1")
      .scan<'i', int>();
  program.add_argument("--excludeSF_StanLET0")
      .default_value(1)
      .help("exclude cases where standard LET return 0")
      .scan<'i', int>();
  program.add_argument("--fork_sensor_num_min")
      .default_value(2)
      .help(
          "the minimum number of sensor tasks for each fork in SF experiments")
      .scan<'i', int>();
  program.add_argument("--fork_sensor_num_max")
      .default_value(9)
      .help(
          "the maximum number of sensor tasks for each fork in SF experiments")
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
  int numberOfProcessor = program.get<int>("--numberOfProcessor");
  double per_core_utilization_min =
      program.get<double>("--per_core_utilization_min");
  double per_core_utilization_max =
      program.get<double>("--per_core_utilization_max");
  int taskSetType = program.get<int>("--taskSetType");
  int deadlineType = program.get<int>("--deadlineType");
  int taskType = program.get<int>("--taskType");
  int excludeUnschedulable = program.get<int>("--excludeUnschedulable");
  int excludeDAGWithWongChainNumber =
      program.get<int>("--excludeDAGWithWongChainNumber");
  int examChainsWithSharedNodes =
      program.get<int>("--examChainsWithSharedNodes");
  int randomSeed = program.get<int>("--randomSeed");
  double parallelismFactor_max = program.get<double>("--parallelismFactor");
  int numCauseEffectChain = program.get<int>("--numCauseEffectChain");
  int chainLength = program.get<int>("--chainLength");
  double chainLengthRatio = program.get<double>("--chainLengthRatio");
  if (chainLengthRatio >= 0.001) {
    chainLength = std::ceil(chainLengthRatio * task_number_in_tasksets);
  }
  std::string outDir = program.get<std::string>("--outDir");
  int clearOutputDir = program.get<int>("--clearOutputDir");
  int SF_ForkNum = program.get<int>("--SF_ForkNum");
  int excludeSF_StanLET0 = program.get<int>("--excludeSF_StanLET0");
  int fork_sensor_num_min = program.get<int>("--fork_sensor_num_min");
  int fork_sensor_num_max = program.get<int>("--fork_sensor_num_max");

  if (randomSeed < 0) {
    srand((unsigned int)(time(0) & 0x0FFFFFFF + (int64_t)&chainLength &
                         0x0FFFFFFF));
  } else {
    srand(randomSeed);
  }

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
      << "numberOfProcessor(--numberOfProcessor): " << numberOfProcessor
      << std::endl
      << "totalUtilization_min(--totalUtilization_min): "
      << totalUtilization_min << std::endl
      << "totalUtilization_max(--totalUtilization_max): "
      << totalUtilization_max << std::endl
      << "max parallelismFactor (--parallelismFactor): "
      << parallelismFactor_max << std::endl
      << "taskSetType(--taskSetType), 0 means normal, 1 means DAG with random "
         "chains, "
         "2 means DAG with chains conforms to WATERS15 distribution: "
      << taskSetType << std::endl
      << "deadlineType(--deadlineType), 1 means random, 0 means implicit: "
      << deadlineType << std::endl
      << "taskType(--taskType), 0 means normal, 1 means random choice from "
         "predefined set, "
      << "2 means automobile task with WATERS distribution: " << taskType
      << std::endl
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
      << "numCauseEffectChain, the number of random cause-effect chains, a "
         "negative number will "
         "use random number of chains (--numCauseEffectChain): "
      << numCauseEffectChain << std::endl
      << "chainLength, the length of random cause-effect chains, 0 means no "
         "length requirements, "
         "Note that this will only be effective when taskSetType is 1. "
         "(--chainLength): "
      << chainLength << std::endl
      << "chainLengthRatio, the ratio of random cause-effect chains length "
         "over the number of tasks in DAG, a value greater than 0 will "
         "overwrite chainLength. (--chainLengthRatio): "
      << chainLengthRatio << std::endl
      << "SF_ForkNum, the number of forks, a negative number will generate "
         "random number of Fork "
         "range from 0.25N to N, default -1. (--SF_ForkNum): "
      << SF_ForkNum << std::endl
      << "exclude cases where standard LET return 0 (--excludeSF_StanLET0): "
      << excludeSF_StanLET0 << std::endl
      << "the minimum number of sensor tasks for each fork in SF experiments "
         "(--fork_sensor_num_min): "
      << fork_sensor_num_min << std::endl
      << "the minimum number of sensor tasks for each fork in SF experiments "
         "(--fork_sensor_num_max): "
      << fork_sensor_num_max << std::endl
      << std::endl;

  std::string outDirectory = GlobalVariablesDAGOpt::PROJECT_PATH + outDir;
  if (clearOutputDir > 0) {
    deleteDirectoryContents(outDirectory);
  }

  double totalUtilization = totalUtilization_min;
  for (int i = DAG_taskSetNameStartIndex; i < DAG_taskSetNumber; i++) {
    if (taskSetType == 1 || taskSetType == 2)  // DAG task set
    {
      TaskSetGenerationParameters tasks_params;
      tasks_params.N = task_number_in_tasksets;
      tasks_params.totalUtilization_min = totalUtilization_min;
      tasks_params.totalUtilization_max = totalUtilization_max;
      tasks_params.numberOfProcessor = numberOfProcessor;
      tasks_params.coreRequireMax = 1;
      tasks_params.parallelismFactor = RandRange(0.3, parallelismFactor_max);
      tasks_params.taskSetType = taskSetType;
      tasks_params.taskType = taskType;
      tasks_params.deadlineType = deadlineType;
      tasks_params.numCauseEffectChain = numCauseEffectChain;
      tasks_params.chain_length = chainLength;
      tasks_params.SF_ForkNum = SF_ForkNum;
      tasks_params.fork_sensor_num_min = fork_sensor_num_min;
      tasks_params.fork_sensor_num_max = fork_sensor_num_max;

      if (numCauseEffectChain < 0)
        tasks_params.numCauseEffectChain = min(
            round(task_number_in_tasksets *
                  (1.5 + (double(rand()) / RAND_MAX) * 1.5)),
            round(0.15 * task_number_in_tasksets * task_number_in_tasksets));
      if (SF_ForkNum < 0)
        tasks_params.SF_ForkNum = 1 + floor(double(rand()) / RAND_MAX * 5);

      DAG_Model dag_tasks;
      if (taskSetType == 1) {  // DAG with random choice of chains
        dag_tasks = GenerateDAG_He21(tasks_params);
      } else if (taskSetType == 2) {
        dag_tasks = GenerateDAG_WATERS15(tasks_params);
      }

      // if (excludeDAGWithWongChainNumber == 1)
      // {
      //   if (dag_tasks.chains_.size() != tasks_params.numCauseEffectChain)
      //   {
      //     i--;
      //     continue;
      //   }
      //   // TODO: check whether need this
      //   // if (examChainsWithSharedNodes && numCauseEffectChain != 1)
      //   // {
      //   //   if (!WhetherDAGChainsShareNodes(dag_tasks))
      //   //   {
      //   //     i--;
      //   //     continue;
      //   //   }
      //   // }
      // }

      if (tasks_params.numCauseEffectChain > 0) {
        if (dag_tasks.chains_.size() < tasks_params.numCauseEffectChain) {
          i--;
        //   cout<<"not enough chain!\n";
          continue;
        }
      }

      if (excludeUnschedulable == 1) {
        const TaskSet &task_set = dag_tasks.GetTaskSet();
        auto dummy_task_set = Reorder(task_set, GlobalVariablesDAGOpt::priorityMode);
        DAG_Model dummy_dag_tasks;
        dummy_dag_tasks.tasks = dummy_task_set;
        dummy_dag_tasks.RecordTaskPosition();
        dummy_dag_tasks.CategorizeTaskSet();
        if (!CheckSchedulability(dummy_dag_tasks)) {
          i--;
        //   cout<<"unschedulable!\n";
          continue;  // re-generate a new task set
        }
      }

      if (tasks_params.SF_ForkNum > 0) {
        if (dag_tasks.sf_forks_.size() < tasks_params.SF_ForkNum) {
          i--;
        //   cout<<"not enough SF fork!\n";
          continue;
        }
        DAG_Model tmp_dag_tasks = dag_tasks;
        tmp_dag_tasks.chains_ = GetChainsForSF(tmp_dag_tasks);
        if (excludeSF_StanLET0 &&
            (PerformStandardLETAnalysis<ObjSensorFusion>(tmp_dag_tasks).obj_ <
                 1e-6 ||
             PerformStandardLETAnalysis<ObjSensorFusion>(tmp_dag_tasks)
                     .jitter_ < 1e-6)) {
          i--;
        //    cout<<"zero LET!\n";
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
      CoutError("taskSetType is not recognized!");
  }

  return 0;
}
