#pragma once

#include <string.h>

#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Parameters.h"

namespace DAG_SPACE {
struct ScheduleResult {
    bool schedulable_;
    double obj_;
    double timeTaken_;
};

std::string GetResFileName(const std::string &pathDataset,
                           const std::string &file, int batchTestMethod_);

void WriteToResultFile(const std::string &pathDataset, const std::string &file,
                       DAG_SPACE::ScheduleResult &res, int batchTestMethod_);

DAG_SPACE::ScheduleResult ReadFromResultFile(const std::string &pathDataset,
                                             const std::string &file,
                                             int batchTestMethod_);

bool VerifyResFileExist(const std::string &pathDataset, const std::string &file,
                        int batchTestMethod_);

std::vector<std::string> ReadFilesInDirectory(const char *path);
}  // namespace DAG_SPACE