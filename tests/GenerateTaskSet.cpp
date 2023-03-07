#include "sources/TaskModel/GenerateRandomTaskset.h"
#include "sources/Utils/argparse.hpp"
// #include "sources/Optimization/OptimizeOrder.h"
#include "sources/Baseline/RTSS21IC.h"
// #include "sources/RTA/RTA_DAG_Model.h"

using namespace GlobalVariablesDAGOpt;
void deleteDirectoryContents(const std::string &dir_path) {
  for (const auto &entry : std::filesystem::directory_iterator(dir_path))
    std::filesystem::remove_all(entry.path());
}
int main(int argc, char *argv[]) {
  using namespace std;
  argparse::ArgumentParser program("program name");
  program.add_argument("-v", "--verbose"); // parameter packing

  program.add_argument("--N").default_value(5).help("the number of tasks in DAG").scan<'i', int>();
  program.add_argument("--taskSetNumber")
      .default_value(10)
      .help("the number DAGs to create")
      .scan<'i', int>();
  program.add_argument("--NumberOfProcessor")
      .default_value(2)
      .help("the NumberOfProcessor of tasks in DAG")
      .scan<'i', int>();
  program.add_argument("--totalUtilization")
      .default_value(0.4)
      .help("the total utilization of tasks in each DAG")
      .scan<'f', double>();
  program.add_argument("--aveUtilization")
      .default_value(0.0)
      .help("the average utilization of tasks in each core")
      .scan<'f', double>();
  program.add_argument("--useRandomUtilization")
      .default_value(1)
      .help("if 1, a random utilization in range [0.3 * numberOfProcessor, 0.9 * numberOfProcessor] will be "
            "used")
      .scan<'i', int>();
  program.add_argument("--minUtilizationPerCore")
      .default_value(0.3)
      .help("only used when --useRandomUtilization is 1")
      .scan<'f', double>();
  program.add_argument("--maxUtilizationPerCore")
      .default_value(0.9)
      .help("only used when --useRandomUtilization is 1")
      .scan<'f', double>();
  program.add_argument("--periodMin")
      .default_value(100)
      .help("the minimum period of tasks in DAG, used only when taskType is 0")
      .scan<'i', int>();
  program.add_argument("--periodMax")
      .default_value(500)
      .help("the maximum period of tasks in DAG, used only when taskType is 0")
      .scan<'i', int>();
  program.add_argument("--taskType")
      .default_value(1)
      .help("type of tasksets, 0 means normal, 1 means DAG")
      .scan<'i', int>();
  program.add_argument("--deadlineType")
      .default_value(0)
      .help("type of tasksets, 0 means implicit, 1 means random")
      .scan<'i', int>();
  program.add_argument("--taskSetType")
      .default_value(3)
      .help("type of taskset period generation method, 1 means normal, 2 means automobile method, 3 means "
            "automobile with WATERS distribution")
      .scan<'i', int>();
  program.add_argument("--coreRequireMax")
      .default_value(1)
      .help("maximum number of cores that each task could require, default 1")
      .scan<'i', int>();
  program.add_argument("--excludeUnschedulable")
      .default_value(1)
      .help("whether exclude unschedulable task set on List Scheduler, default 1")
      .scan<'i', int>();
  program.add_argument("--excludeEmptyEdgeDag")
      .default_value(1)
      .help("whether exclude dags that don't have edges, default 1")
      .scan<'i', int>();
  program.add_argument("--randomSeed")
      .default_value(-1)
      .help("seed of random, negative means use current time as seed, otherwise means self-defined seed")
      .scan<'i', int>();
  program.add_argument("--outDir")
      .default_value(std::string("TaskData/dagTasks/"))
      .help("directory to save task sets, only within the root folder");

  // program.add_argument("--parallelismFactor")
  //     .default_value(1000)
  //     .help("the parallelismFactor DAG")
  //     .scan<'i', int>();

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    exit(0);
  }

  int N = program.get<int>("--N");
  int DAG_taskSetNumber = program.get<int>("--taskSetNumber");
  int numberOfProcessor = program.get<int>("--NumberOfProcessor");
  double totalUtilization;
  double aveUtilization = program.get<double>("--aveUtilization");
  int useRandomUtilization = program.get<int>("--useRandomUtilization");
  double minUtilizationPerCore = program.get<double>("--minUtilizationPerCore");
  double maxUtilizationPerCore = program.get<double>("--maxUtilizationPerCore");
  int periodMin = program.get<int>("--periodMin");
  int periodMax = program.get<int>("--periodMax");
  int taskType = program.get<int>("--taskType");
  int deadlineType = program.get<int>("--deadlineType");
  int taskSetType = program.get<int>("--taskSetType");
  int coreRequireMax = program.get<int>("--coreRequireMax");
  int excludeUnschedulable = program.get<int>("--excludeUnschedulable");
  int excludeEmptyEdgeDag = program.get<int>("--excludeEmptyEdgeDag");
  int randomSeed = program.get<int>("--randomSeed");
  std::string outDir = program.get<std::string>("--outDir");
  if (randomSeed < 0) {
    srand(time(0));
  } else {
    srand(randomSeed);
  }
  if (aveUtilization != 0) {
    totalUtilization = aveUtilization * numberOfProcessor;
  } else {
    totalUtilization = program.get<double>("--totalUtilization");
  }

  std::cout << "Task configuration: " << std::endl
            << "the number of tasks in DAG(--N): " << N << std::endl
            << "DAG_taskSetNumber(--taskSetNumber): " << DAG_taskSetNumber << std::endl
            << "NumberOfProcessor(--NumberOfProcessor): " << numberOfProcessor << std::endl
            << "totalUtilization(--totalUtilization): " << totalUtilization << std::endl
            << "aveUtilization(--aveUtilization): " << aveUtilization << std::endl
            << "whether use random utilization(--useRandomUtilization): " << useRandomUtilization << std::endl
            << "minimum utilization per core, only work in random utilization mode(--minUtilizationPerCore): "
            << minUtilizationPerCore << std::endl
            << "maximum utilization per core, only work in random utilization mode(--maxUtilizationPerCore): "
            << maxUtilizationPerCore << std::endl
            << "periodMin, only work in normal(random) taskSetType(--periodMin), : " << periodMin << std::endl
            << "periodMax, only work in normal(random) taskSetType(--periodMax): " << periodMax << std::endl
            << "taskType(--taskType), 0 means normal, 1 means DAG: " << taskType << std::endl
            << "deadlineType(--deadlineType), 1 means random, 0 means implicit: " << deadlineType << std::endl
            << "taskSetType(--taskSetType), 1 means normal, 2 means AutoMobile, 3 means automobile with "
               "WATERS distribution: "
            << taskSetType << std::endl
            << "coreRequireMax(--coreRequireMax): " << coreRequireMax << std::endl
            << "excludeUnschedulable(--excludeUnschedulable): " << excludeUnschedulable << std::endl
            << "excludeEmptyEdgeDag(--excludeEmptyEdgeDag): " << excludeEmptyEdgeDag << std::endl
            << "randomSeed, negative will use current time, otherwise use the given seed(--randomSeed): "
            << randomSeed << std::endl
            << std::endl;

  std::string outDirectory = GlobalVariablesDAGOpt::PROJECT_PATH + outDir;
  // std::string outDirectory = GlobalVariablesDAGOpt::PROJECT_PATH +  "Energy_Opt_NLP/TaskData/task_number/";
  deleteDirectoryContents(outDirectory);

  for (size_t i = 0; i < DAG_taskSetNumber; i++) {
    if (useRandomUtilization) {
      totalUtilization =
          numberOfProcessor * (minUtilizationPerCore +
                               (double(rand()) / RAND_MAX) * (maxUtilizationPerCore - minUtilizationPerCore));
    }
    if (taskType == 0) // normal task set
    {
      TaskSet tasks = GenerateTaskSet(N, totalUtilization, numberOfProcessor, periodMin, periodMax,
                                      coreRequireMax, taskSetType, deadlineType);
      std::string fileName = "periodic-set-" + std::string(3 - std::to_string(i).size(), '0') + to_string(i) +
                             "-syntheticJobs" + ".csv";
      std::ofstream myfile;
      myfile.open(outDirectory + fileName);
      WriteTaskSets(myfile, tasks);
    } else if (taskType == 1) // DAG task set
    {
      while (true) {
        DAG_Model tasks = GenerateDAG(N, totalUtilization, numberOfProcessor, periodMin, periodMax,
                                      coreRequireMax, taskSetType, deadlineType);
        if (excludeEmptyEdgeDag == 1) {
          bool whether_empty_edges = true;
          for (auto pair : tasks.mapPrev) {
            if (pair.second.size() > 0) {
              whether_empty_edges = false;
              break;
            }
          }
          if (whether_empty_edges) {
            continue;
          }
        }
        if (excludeUnschedulable == 1) {
          // rt_num_opt::RTA_DAG_Model rta(tasks);
          // std::cout << rta.CheckSchedulability() << std::endl;
          TaskSet &taskSet = tasks.tasks;
          TaskSetInfoDerived tasksInfo(taskSet);
          std::vector<uint> processorJobVec;
          // std::optional<JobOrderMultiCore> emptyOrder;
          VectorDynamic initialSTV =
              ListSchedulingLFTPA(tasks, tasksInfo, numberOfProcessor, processorJobVec);
          if ((considerSensorFusion == 0 &&
               (!ExamBasic_Feasibility(tasks, tasksInfo, initialSTV, processorJobVec, numberOfProcessor))) ||
              (considerSensorFusion != 0 && (!ExamDDL_Feasibility(tasks, tasksInfo, initialSTV)))) {
            if (GlobalVariablesDAGOpt::debugMode) {
              std::cout << "Un feasible case, skipped.\n";
            }
            continue;
          }
        }
        string fileName = "dag-set-N" + to_string(N) + "-" + string(3 - to_string(i).size(), '0') +
                          to_string(i) + "-syntheticJobs" + ".csv";
        std::ofstream myfile;
        myfile.open(outDirectory + fileName);
        WriteDAG(myfile, tasks);
        myfile.close();
        break;
      }
    }
  }

  return 0;
}
