#pragma once
#include <dirent.h>
#include <sys/types.h>

#include <chrono>
#include <fstream>
#include <iostream>

#include "assert.h"
#include "sources/Baseline/StandardLET.h"
#include "sources/Optimization/TaskSetPermutation.h"
#include "sources/Utils/BatchUtils.h"
#include "sources/Utils/ScheduleResults.h"
#include "sources/Utils/VariadicTable.h"

using namespace std::chrono;
namespace DAG_SPACE {

void ClearResultFiles(std::string dataSetFolder);

template <typename ObjectiveFunctionBase>
DAG_SPACE::ScheduleResult PerformSingleScheduling(
    DAG_Model &dag_tasks, BASELINEMETHODS batchTestMethod) {
  ScheduleResult res;
  switch (batchTestMethod) {
    case InitialMethod:
      res = PerformStandardLETAnalysis<ObjectiveFunctionBase>(dag_tasks);
      break;
    case TOM:
      res = PerformTOM_OPT<ObjectiveFunctionBase>(dag_tasks, "Enumerate");
      break;
    case TOM_Sort:
      res = PerformTOM_OPT<ObjectiveFunctionBase>(dag_tasks, "Sort");
      break;
    case TOM_Approx:
      if (ObjectiveFunctionBase::type_trait == "ReactionTime" ||
          ObjectiveFunctionBase::type_trait == "ReactionTimeApprox")
        res = PerformTOM_OPT<ObjReactionTimeApprox>(dag_tasks, "Enumerate");
      else if (ObjectiveFunctionBase::type_trait == "DataAge" ||
               ObjectiveFunctionBase::type_trait == "DataAgeApprox")
        res = PerformTOM_OPT<ObjDataAgeApprox>(dag_tasks, "Enumerate");
      else
        CoutError("Unknown type trait in BatchOptimize!");
      break;
    case TOM_Sort_Approx:
      if (ObjectiveFunctionBase::type_trait == "ReactionTime" ||
          ObjectiveFunctionBase::type_trait == "ReactionTimeApprox")
        res = PerformTOM_OPT<ObjReactionTimeApprox>(dag_tasks, "Sort");
      else if (ObjectiveFunctionBase::type_trait == "DataAge" ||
               ObjectiveFunctionBase::type_trait == "DataAgeApprox")
        res = PerformTOM_OPT<ObjDataAgeApprox>(dag_tasks, "Sort");
      else
        CoutError("Unknown type trait in BatchOptimize!");
      break;
    case SA:
      // TO ADD
      break;

    default:
      CoutError("Please provide batchTestMethod implementation!");
  }
  return res;
}

template <typename ObjectiveFunctionBase>
std::vector<BatchResult> BatchOptimizeOrder(
    std::vector<DAG_SPACE::BASELINEMETHODS> &baselineMethods, int N = -1,
    int chainNum = GlobalVariablesDAGOpt::CHAIN_NUMBER) {
  std::string dataSetFolder;
  if (N == -1)
    dataSetFolder = GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/dagTasks/";
  else
    dataSetFolder = GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/N" +
                    std::to_string(N) + "/";

  const char *pathDataset = dataSetFolder.c_str();
  std::cout << "Dataset Directory: " << pathDataset << std::endl;

  // Prepare intermediate data records
  std::vector<int> chain_lenth;
  ResultsManager results_man(baselineMethods);
  std::vector<std::string> errorFiles;

  std::vector<std::string> files = ReadFilesInDirectory(pathDataset);
  int fileIndex = 0;
  for (const auto &file : files) {
    std::string delimiter = "-";
    if (file.substr(0, file.find(delimiter)) == "dag" &&
        file.find("Res") == std::string::npos &&
        file.find("LoopCount") == std::string::npos) {
      std::cout << file << std::endl;

      std::string path = dataSetFolder + file;
      DAG_SPACE::DAG_Model dag_tasks = DAG_SPACE::ReadDAG_Tasks(
          path, GlobalVariablesDAGOpt::priorityMode, chainNum);
      AssertBool(true, dag_tasks.chains_.size() > 0, __LINE__);
      for (const auto &chain : dag_tasks.chains_)
        chain_lenth.push_back(chain.size());

      for (auto batchTestMethod : baselineMethods) {
        DAG_SPACE::ScheduleResult res;
        if (VerifyResFileExist(pathDataset, file, batchTestMethod)) {
          res = ReadFromResultFile(pathDataset, file, batchTestMethod);
        } else {
          res = PerformSingleScheduling<ObjectiveFunctionBase>(dag_tasks,
                                                               batchTestMethod);
        }
        std::cout << "Schedulable? " << res.schedulable_ << std::endl;
        std::cout << Color::green << "Objective: " << res.obj_ << Color::def
                  << std::endl;
        std::cout << "res.timeTaken_: " << res.timeTaken_ << "\n\n";

        if (!res.schedulable_ || res.obj_ >= 1e8) errorFiles.push_back(file);

        WriteToResultFile(pathDataset, file, res, batchTestMethod);
        results_man.add(batchTestMethod, res, file);
      }
      fileIndex++;
    }
  }

  // result analysis
  results_man.PrintWorseCase(BASELINEMETHODS::TOM_Approx,
                             BASELINEMETHODS::TOM_Sort_Approx);
  results_man.PrintResultTable(baselineMethods);
  results_man.PrintLongestCase(BASELINEMETHODS::TOM);
  results_man.PrintTimeOutCase();

  std::cout << "Average length of cause-effect chains: " << Average(chain_lenth)
            << std::endl;
  std::cout << "Maximum length of cause-effect chains: "
            << *std::max_element(chain_lenth.begin(), chain_lenth.end())
            << std::endl;

  std::cout << "Error files:\n";
  for (auto file : errorFiles) std::cout << file << "\n";

  return results_man.GetBatchResVec(baselineMethods);
}
}  // namespace DAG_SPACE