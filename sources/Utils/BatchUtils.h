#pragma once

#include <string.h>

#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/ScheduleResults.h"

namespace DAG_SPACE {

std::string GetResFileName(const std::string &pathDataset,
                           const std::string &file, BASELINEMETHODS method);

void WriteToResultFile(const std::string &pathDataset, const std::string &file,
                       DAG_SPACE::ScheduleResult &res, BASELINEMETHODS method);

DAG_SPACE::ScheduleResult ReadFromResultFile(const std::string &pathDataset,
                                             const std::string &file,
                                             BASELINEMETHODS method);

bool VerifyResFileExist(const std::string &pathDataset, const std::string &file,
                        BASELINEMETHODS method);

std::vector<std::string> ReadFilesInDirectory(const char *path);
}  // namespace DAG_SPACE