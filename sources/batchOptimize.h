#pragma once
#include <dirent.h>
#include <sys/types.h>

#include <chrono>
#include <fstream>
#include <iostream>

#include "assert.h"
#include "sources/Baseline/StandardLET.h"
#include "sources/Optimization/TaskSetPermutationEnumerate.h"
#include "sources/Utils/BatchUtils.h"
#include "sources/Utils/VariadicTable.h"

using namespace std::chrono;
namespace DAG_SPACE {
enum BaselineMethods {
    InitialMethod,  // 0
    TOM,            // 1
    TOM_FAST,       // 2
    SA              // 3
};
// if adding more methods, need to update WriteScheduleToFile() and
// GlobalVariablesDAGOpt::TotalMethodUnderComparison

struct BatchResult {
    double schedulableRatio;
    double performance;
    double runTime;
};
void ClearResultFiles(std::string dataSetFolder);

template <typename ObjectiveFunctionBase>
DAG_SPACE::ScheduleResult PerformSingleScheduling(
    DAG_Model &dag_tasks, BaselineMethods batchTestMethod) {
    ScheduleResult res;
    switch (batchTestMethod) {
        case InitialMethod:
            res = PerformStandardLETAnalysis<ObjectiveFunctionBase>(dag_tasks);
            break;
        case TOM:
            res = PerformTOM_OPT<ObjectiveFunctionBase>(dag_tasks);
            break;
        case TOM_FAST:
            // TO ADD
            // SKIP_PERM=0;
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
    std::vector<DAG_SPACE::BaselineMethods> &baselineMethods, int N = -1,
    int chainNum = GlobalVariablesDAGOpt::CHAIN_NUMBER) {
    std::string dataSetFolder;
    if (N == -1)
        dataSetFolder =
            GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/dagTasks/";
    else
        dataSetFolder = GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/N" +
                        std::to_string(N) + "/";

    const char *pathDataset = dataSetFolder.c_str();
    std::cout << "Dataset Directory: " << pathDataset << std::endl;

    // Prepare intermediate data records
    std::vector<int> chain_lenth;
    std::vector<std::vector<double>> runTimeAll(10);
    std::vector<std::vector<double>> objsAll(10);
    std::vector<std::vector<int>> schedulableAll(10);  // values could
                                                       // only be 0 / 1
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
                    res =
                        ReadFromResultFile(pathDataset, file, batchTestMethod);
                } else {
                    res = PerformSingleScheduling<ObjectiveFunctionBase>(
                        dag_tasks, batchTestMethod);
                }
                std::cout << "Schedulable? " << res.schedulable_ << std::endl;
                std::cout << Color::green << "Objective: " << res.obj_
                          << Color::def << std::endl;
                std::cout << "res.timeTaken_: " << res.timeTaken_ << "\n\n";

                if (!res.schedulable_ || res.obj_ >= 1e8)
                    errorFiles.push_back(file);

                WriteToResultFile(pathDataset, file, res, batchTestMethod);
                runTimeAll[batchTestMethod].push_back(res.timeTaken_);
                schedulableAll[batchTestMethod].push_back(
                    (res.schedulable_ ? 1 : 0));
                objsAll[batchTestMethod].push_back(
                    res.obj_);  // /objsAll[0][fileIndex]
            }
            fileIndex++;
        }
    }

    VariadicTable<std::string, double, double, double, double> vt(
        {"Method", "Schedulable ratio", "Obj (Only used in RTDA experiment)",
         "Obj(Norm)", "TimeTaken"},
        10);
    vt.addRow("Initial", Average(schedulableAll[0]), Average(objsAll[0]),
              Average(objsAll[0], objsAll[0]), Average(runTimeAll[0]));
    vt.addRow("TOM", Average(schedulableAll[1]), Average(objsAll[1]),
              Average(objsAll[1], objsAll[0]), Average(runTimeAll[1]));
    // vt.addRow("TOM_FAST", Average(schedulableAll[2]), Average(objsAll[2]),
    //           Average(objsAll[2], objsAll[0]), Average(runTimeAll[2]));
    vt.print(std::cout);

    std::vector<DAG_SPACE::BaselineMethods> baselineMethodsAll = {
        InitialMethod,  // 0
        TOM,            // 1
        TOM_FAST,       // 2
        SA              // 3
    };
    std::vector<BatchResult> batchResVec;
    for (uint i = 0; i < baselineMethods.size(); i++) {
        BatchResult batchRes{Average(schedulableAll[i]),
                             Average(objsAll[i], objsAll[0]),
                             Average(runTimeAll[i])};
        batchResVec.push_back(batchRes);
    }
    if (runTimeAll[1].size() > 0) {
        std::cout << "Case that takes the longest time in TOM: "
                  << int((std::max_element(runTimeAll[1].begin(),
                                           runTimeAll[1].end())) -
                         runTimeAll[1].begin())
                  << ", which takes "
                  << *(std::max_element(runTimeAll[1].begin(),
                                        runTimeAll[1].end()))
                  << " seconds"
                  << "\n";
    }

    std::cout << "Error files:\n";
    for (auto file : errorFiles) std::cout << file << "\n";

    std::cout << "Time-out files:\n";
    for (uint i = 0; i < runTimeAll[1].size(); i++) {
        if (runTimeAll[1][i] >= GlobalVariablesDAGOpt::TIME_LIMIT - 5) {
            std::cout << i << "\n";
        }
    }
    std::cout << "Average length of cause-effect chains: "
              << Average(chain_lenth) << std::endl;
    std::cout << "Maximum length of cause-effect chains: "
              << *std::max_element(chain_lenth.begin(), chain_lenth.end())
              << std::endl;

    return batchResVec;
}
}  // namespace DAG_SPACE