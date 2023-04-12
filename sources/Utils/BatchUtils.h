#pragma once

#include <string.h>

#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/ScheduleResults.h"

namespace DAG_SPACE {

std::string GetResFileName(const std::string &pathDataset,
                           const std::string &file, BASELINEMETHODS method,
                           const std::string obj_trait);

void WriteToResultFile(const std::string &pathDataset, const std::string &file,
                       DAG_SPACE::ScheduleResult &res, BASELINEMETHODS method,
                       const std::string obj_trait);

DAG_SPACE::ScheduleResult ReadFromResultFile(const std::string &pathDataset,
                                             const std::string &file,
                                             BASELINEMETHODS method,
                                             const std::string obj_trait);

bool VerifyResFileExist(const std::string &pathDataset, const std::string &file,
                        BASELINEMETHODS method, const std::string obj_trait);

std::vector<std::string> ReadFilesInDirectory(const char *path);
}  // namespace DAG_SPACE