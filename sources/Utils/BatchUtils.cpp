#include "sources/Utils/BatchUtils.h"

namespace OrderOptDAG_SPACE {

std::string GetResFileName(const std::string &pathDataset, const std::string &file, int batchTestMethod_) {
  std::string property;
  if (batchTestMethod_ == 0) {
    property = "_Initial_Res.txt";
  } else if (batchTestMethod_ == 1) {
    property = "_Verucchi20_Res.txt";
  } else if (batchTestMethod_ == 2) {
    property = "_Wang21_Res.txt";
  } else if (batchTestMethod_ == 3) {
    property = "_TOM_Res.txt";
  } else if (batchTestMethod_ == 4) {
    property = "_TOM_Fast_Res.txt";
  } else if (batchTestMethod_ == 5) {
    property = "_TOM_FastLP_Res.txt";
  } else if (batchTestMethod_ == 6) {
    property = "_TOM_Global_Res.txt";
  } else {
    property = "";
  }
  return pathDataset + file + property;
}
// TOTEST: read & write
void WriteToResultFile(const std::string &pathDataset, const std::string &file,
                       OrderOptDAG_SPACE::ScheduleResult &res, int batchTestMethod_) {
  std::string resFile = GetResFileName(pathDataset, file, batchTestMethod_);
  std::ofstream outfileWrite;
  outfileWrite.open(resFile, std::ofstream::out | std::ofstream::trunc); // std::ios_base::app
  outfileWrite << res.schedulable_ << std::endl;
  outfileWrite << res.obj_ << std::endl;
  outfileWrite << res.timeTaken_ << std::endl;
  outfileWrite.close();
  if (batchTestMethod_ > 2) {
    resFile = resFile.substr(0, resFile.length() - 4);
    resFile += "_LoopCount.txt";
    outfileWrite.open(resFile, std::ofstream::out | std::ofstream::trunc); // std::ios_base::app
    outfileWrite << res.countOutermostWhileLoop_ << std::endl;
    outfileWrite << res.countMakeProgress_ << std::endl;
    outfileWrite << res.countIterationStatus_ << std::endl;
    outfileWrite.close();
  }
}

void WriteScheduleToFile(const std::string &pathDataset, const std::string &file, DAG_Model &dagTasks,
                         ScheduleResult &res, int batchTestMethod_) {
  TaskSetInfoDerived tasksInfo(dagTasks.tasks);
  auto timeJobVector = ObtainAllJobSchedule(tasksInfo, res.startTimeVector_);
  timeJobVector = SortJobSchedule(timeJobVector);

  std::string resFile = GetResFileName(pathDataset, file, batchTestMethod_);
  resFile = resFile.substr(0, resFile.length() - 4);
  resFile += "_Schedule.txt";
  std::ofstream outfileWrite;
  outfileWrite.open(resFile, std::ofstream::out | std::ofstream::trunc); // std::ios_base::app

  outfileWrite << GlobalVariablesDAGOpt::coreNumberAva << std::endl;
  outfileWrite << tasksInfo.hyperPeriod << std::endl;
  outfileWrite << tasksInfo.variableDimension << std::endl;
  outfileWrite << 10 << std::endl; // total hyperperiods to run
  outfileWrite << GlobalVariablesDAGOpt::NumCauseEffectChain << std::endl;
  for (auto chain : dagTasks.chains_) {
    for (auto task_id : chain) {
      outfileWrite << task_id << ", ";
    }
    outfileWrite << std::endl;
  }
  for (auto time_job_pair : timeJobVector) {
    outfileWrite << time_job_pair.first.first << std::endl;
    outfileWrite << time_job_pair.second.taskId << std::endl;
  }
  outfileWrite.close();
}

OrderOptDAG_SPACE::ScheduleResult ReadFromResultFile(const std::string &pathDataset, const std::string &file,
                                                     int batchTestMethod_) {
  OrderOptDAG_SPACE::ScheduleResult result;
  std::string resFile = GetResFileName(pathDataset, file, batchTestMethod_);
  std::ifstream cResultFile(resFile.data());
  // double timeTaken = 0, obj = 0;
  // int schedulable = 0;
  cResultFile >> result.schedulable_ >> result.obj_ >> result.timeTaken_;
  cResultFile.close();
  return result;
}

bool VerifyResFileExist(const std::string &pathDataset, const std::string &file, int batchTestMethod_) {
  std::string resFile = GetResFileName(pathDataset, file, batchTestMethod_);
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
      files.push_back(en->d_name); // print all directory name
    }
    closedir(dr); // close all directory
  }
  std::sort(files.begin(), files.end());
  return files;
}
} // namespace OrderOptDAG_SPACE