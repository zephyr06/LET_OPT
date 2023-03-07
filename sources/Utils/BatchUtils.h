#pragma once

#include <string.h>
#include "sources/Utils/Parameters.h"
// #include "sources/Optimization/OptimizeOrder.h"
#include "sources/Utils/OptimizeOrderUtils.h"

namespace OrderOptDAG_SPACE
{

    std::string GetResFileName(const std::string &pathDataset, const std::string &file, int batchTestMethod_);

    void WriteToResultFile(const std::string &pathDataset, const std::string &file, OrderOptDAG_SPACE::ScheduleResult &res, int batchTestMethod_);

    void WriteScheduleToFile(const std::string &pathDataset, const std::string &file, DAG_Model &dagTasks, ScheduleResult &res, int batchTestMethod_);

    OrderOptDAG_SPACE::ScheduleResult ReadFromResultFile(const std::string &pathDataset, const std::string &file, int batchTestMethod_);

    bool VerifyResFileExist(const std::string &pathDataset, const std::string &file, int batchTestMethod_);

   std::vector<std::string> ReadFilesInDirectory(const char *path);
}