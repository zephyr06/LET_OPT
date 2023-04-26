#pragma once
#include <dirent.h>
#include <sys/types.h>

#include <chrono>
#include <fstream>
#include <iostream>

#include "assert.h"
#include "sources/Baseline/StandardLET.h"
#include "sources/OptimizeMain.h"
#include "sources/Utils/BatchUtils.h"
#include "sources/Utils/ScheduleResults.h"
#include "sources/Utils/VariadicTable.h"

using namespace std::chrono;
namespace DAG_SPACE {

void ClearResultFiles(std::string dataSetFolder);

template <typename ObjectiveFunctionBase>
DAG_SPACE::ScheduleResult PerformSingleScheduling(
    DAG_Model &dag_tasks, BASELINEMETHODS batchTestMethod,
    const char *pathDataset, const std::string &file) {
  ScheduleResult res;
  DAG_SPACE::ScheduleResult res_sort;
  switch (batchTestMethod) {
    case InitialMethod:
      res = PerformStandardLETAnalysis<ObjectiveFunctionBase>(dag_tasks);
      break;
    case TOM_BF:
      res = PerformTOM_OPT_BF<ObjectiveFunctionBase>(dag_tasks);
      break;
    case TOM_WSkip:
      res = PerformTOM_OPT_EnumW_Skip<ObjectiveFunctionBase>(dag_tasks);
      break;
    case TOM_Sort:
      res = PerformTOM_OPT_Sort<ObjectiveFunctionBase>(dag_tasks);
      break;
    case Martinez18:
      res = PerformOPT_Martinez18_DA(dag_tasks);
      break;
    case TOM_Sort_Offset:
      res = PerformTOM_OPTOffset_Sort(dag_tasks);
      break;
    case TOM_Sort_Bound:
      res = PerformTOM_OPT_SortBound<ObjectiveFunctionBase>(dag_tasks);
      break;
    // case TOM_Sort_ImpBound:  // TODO: Need ReadFromResultFile and Write to
    // save
    //                          // Variable_OD
    //   if (VerifyResFileExist(pathDataset, file, TOM_Sort,
    //                          ObjectiveFunctionBase::type_trait))
    //     res_sort = ReadFromResultFile(pathDataset, file, TOM_Sort,
    //                                   ObjectiveFunctionBase::type_trait);
    case ImplicitCommunication:
      res = PerformImplicitCommuAnalysis<ObjectiveFunctionBase>(dag_tasks);
      break;

    default:
      CoutError("Please provide batchTestMethod implementation!");
  }
  return res;
}

struct BatchSettings {
  BatchSettings()
      : N(-1),
        file_index_begin(0),
        file_index_end(299),
        folder_relative_path("TaskData/N0/") {}
  BatchSettings(int N, int begin, int end,
                const std::string &folder_relative_path)
      : N(N),
        file_index_begin(begin),
        file_index_end(end),
        folder_relative_path(folder_relative_path) {}

  // data members
  int N;  // number of nodes in DAG
  size_t file_index_begin;
  size_t file_index_end;
  int chainNum = GlobalVariablesDAGOpt::CHAIN_NUMBER;
  std::string folder_relative_path;
};

template <typename ObjectiveFunctionBase>
std::unordered_map<DAG_SPACE::BASELINEMETHODS, BatchResult> BatchOptimizeOrder(
    std::vector<DAG_SPACE::BASELINEMETHODS> &baselineMethods,
    const BatchSettings &batch_test_settings)
//  int N = -1,
// int chainNum = GlobalVariablesDAGOpt::CHAIN_NUMBER,
// int file_index_begin = 0, int file_index_end = 300,
// std::string dataSetFolderRelativePath = "TaskData/N0/")
{
  std::string dataSetFolder = GlobalVariablesDAGOpt::PROJECT_PATH +
                              batch_test_settings.folder_relative_path;
  const char *pathDataset = dataSetFolder.c_str();
  std::cout << "Dataset Directory: " << pathDataset << std::endl;

  // Prepare intermediate data records
  std::vector<int> chain_lenth;
  ResultsManager results_man(baselineMethods);
  std::vector<std::string> errorFiles;

  int fileIndex = 0;
  for (size_t file_index = batch_test_settings.file_index_begin;
       file_index < batch_test_settings.file_index_end; file_index++) {
    std::string file = GetTaskSetName(file_index, batch_test_settings.N);
    std::cout << file << std::endl;

    std::string path = dataSetFolder + file;
    DAG_SPACE::DAG_Model dag_tasks =
        DAG_SPACE::ReadDAG_Tasks(path, GlobalVariablesDAGOpt::priorityMode,
                                 batch_test_settings.chainNum);
    AssertBool(true, dag_tasks.chains_.size() > 0, __LINE__);
    for (const auto &chain : dag_tasks.chains_)
      chain_lenth.push_back(chain.size());

    for (auto batchTestMethod : baselineMethods) {
      DAG_SPACE::ScheduleResult res;
      if (VerifyResFileExist(pathDataset, file, batchTestMethod,
                             ObjectiveFunctionBase::type_trait)) {
        res = ReadFromResultFile(pathDataset, file, batchTestMethod,
                                 ObjectiveFunctionBase::type_trait);
      } else {
        res = PerformSingleScheduling<ObjectiveFunctionBase>(
            dag_tasks, batchTestMethod, pathDataset, file);
      }
      std::cout << "Schedulable? " << res.schedulable_ << std::endl;
      std::cout << Color::green << "Objective: " << res.obj_ << Color::def
                << std::endl;
      std::cout << "res.timeTaken_: " << res.timeTaken_ << "\n\n";

      if ((!res.schedulable_ && batchTestMethod == TOM_Sort) || res.obj_ >= 1e8)
        errorFiles.push_back(file);

      WriteToResultFile(pathDataset, file, res, batchTestMethod,
                        ObjectiveFunctionBase::type_trait);
      results_man.add(batchTestMethod, res, file);
    }
    fileIndex++;
  }
  // }

  // result analysis
  // results_man.PrintWorseCase(BASELINEMETHODS::TOM_Approx,
  //                            BASELINEMETHODS::TOM_Sort_Approx);
  results_man.PrintResultTable(baselineMethods);
  if (std::find(baselineMethods.begin(), baselineMethods.end(),
                BASELINEMETHODS::TOM_WSkip) != baselineMethods.end())
    results_man.PrintLongestCase(BASELINEMETHODS::TOM_WSkip);
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