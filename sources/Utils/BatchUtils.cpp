#include "sources/Utils/BatchUtils.h"

namespace DAG_SPACE {

std::string GetResFileName(const std::string &pathDataset,
                           const std::string &file, BASELINEMETHODS method) {
  std::string property;
  //   if (method == 0) {
  //     property = "_Initial_Res.txt";
  //   } else if (method == 1) {
  //     property = "_TOM_Res.txt";
  //   } else if (method == 2) {
  //     property = "_TOM_FAST_Res.txt";
  //   }
  if (method < BaselineMethodNames.size())
    property = "_" + BaselineMethodNames[method] + "_Res.txt";
  else {
    CoutError("Unknown method index in GetResFileName!");
  }
  return pathDataset + file + property;
}
// TOTEST: read & write
void WriteToResultFile(const std::string &pathDataset, const std::string &file,
                       DAG_SPACE::ScheduleResult &res, BASELINEMETHODS method) {
  std::string resFile = GetResFileName(pathDataset, file, method);
  std::ofstream outfileWrite;
  outfileWrite.open(resFile, std::ofstream::out |
                                 std::ofstream::trunc);  // std::ios_base::app
  outfileWrite << res.schedulable_ << std::endl;
  outfileWrite << res.obj_ << std::endl;
  outfileWrite << res.timeTaken_ << std::endl;
  outfileWrite.close();
  if (method > 2) {
    resFile = resFile.substr(0, resFile.length() - 4);
    resFile += "_LoopCount.txt";
    outfileWrite.open(
        resFile,
        std::ofstream::out | std::ofstream::trunc);  // std::ios_base::app
    outfileWrite.close();
  }
}

DAG_SPACE::ScheduleResult ReadFromResultFile(const std::string &pathDataset,
                                             const std::string &file,
                                             BASELINEMETHODS method) {
  DAG_SPACE::ScheduleResult result;
  std::string resFile = GetResFileName(pathDataset, file, method);
  std::ifstream cResultFile(resFile.data());
  // double timeTaken = 0, obj = 0;
  // int schedulable = 0;
  cResultFile >> result.schedulable_ >> result.obj_ >> result.timeTaken_;
  cResultFile.close();
  return result;
}

bool VerifyResFileExist(const std::string &pathDataset, const std::string &file,
                        BASELINEMETHODS method) {
  std::string resFile = GetResFileName(pathDataset, file, method);
  std::ifstream myfile;
  myfile.open(resFile);
  if (myfile) {
    return true;
  } else {
    return false;
  }
}

std::vector<std::string> ReadFilesInDirectory(const char *path) {
  std::vector<std::string> files;
  DIR *dr;
  struct dirent *en;
  dr = opendir(path);
  if (dr) {
    while ((en = readdir(dr)) != NULL) {
      files.push_back(en->d_name);  // print all directory name
    }
    closedir(dr);  // close all directory
  }
  std::sort(files.begin(), files.end());
  return files;
}
}  // namespace DAG_SPACE