
#include "sources/Utils/ScheduleResults.h"
namespace DAG_SPACE {

void ResultsManager::add(BASELINEMETHODS method, const ScheduleResult& result,
                         std::string file) {
  results_map_[method].push_back(result);
  runTimeAll_[method].push_back(result.timeTaken_);
  objsAll_[method].push_back(result.obj_);
  schedulableAll_[method].push_back(result.schedulable_);
  if (file_name_.size() == 0 || file_name_.back() != file)
    file_name_.push_back(file);
}

std::unordered_map<DAG_SPACE::BASELINEMETHODS, BatchResult>
ResultsManager::GetBatchResVec(
    const std::vector<DAG_SPACE::BASELINEMETHODS>& baselineMethods) const {
  std::unordered_map<DAG_SPACE::BASELINEMETHODS, BatchResult> batchResVec;
  for (auto method : baselineMethods) {
    BatchResult batchRes{Average(schedulableAll_.at(method)),
                         Average(objsAll_.at(method),
                                 objsAll_.at(BASELINEMETHODS::InitialMethod)),
                         Average(runTimeAll_.at(method))};
    batchResVec[method] = (batchRes);
  }
  return batchResVec;
}

void ResultsManager::PrintLongestCase(BASELINEMETHODS method) const {
  if (runTimeAll_.find(method) == runTimeAll_.end()) return;
  if (runTimeAll_.at(method).size() > 0) {
    std::cout << "The case that takes the longest time in " +
                     BaselineMethodNames[method] + " is: "
              << int((std::max_element(runTimeAll_.at(method).begin(),
                                       runTimeAll_.at(method).end())) -
                     runTimeAll_.at(method).begin())
              << ", which takes "
              << *(std::max_element(runTimeAll_.at(method).begin(),
                                    runTimeAll_.at(method).end()))
              << " seconds"
              << "\n";
  }
}

// depend on global paramters: BaselineMethodNames
void ResultsManager::PrintResultTable(
    const std::vector<DAG_SPACE::BASELINEMETHODS>& baselineMethods) const {
  VariadicTable<std::string, double, double, double, double> vt(
      {"Method", "Schedulable ratio", "Obj", "Obj(Norm)", "TimeTaken"}, 10);
  for (const auto& method : baselineMethods) {
    vt.addRow(BaselineMethodNames[method], Average(schedulableAll_.at(method)),
              Average(objsAll_.at(method)),
              Average(objsAll_.at(method),
                      objsAll_.at(BASELINEMETHODS::InitialMethod)),
              Average(runTimeAll_.at(method)));
  }
  vt.print(std::cout);
}

void ResultsManager::PrintTimeOutCase() const {
  for (auto itr = runTimeAll_.begin(); itr != runTimeAll_.end(); itr++) {
    PrintTimeOutCaseSingleMethod(itr->first);
  }
}
void ResultsManager::PrintTimeOutRatio() const {
  for (auto itr = runTimeAll_.begin(); itr != runTimeAll_.end(); itr++) {
    PrintTimeOutRatio(itr->first);
  }
}

void ResultsManager::PrintTimeOutCaseSingleMethod(
    BASELINEMETHODS method) const {
  if (runTimeAll_.find(method) == runTimeAll_.end()) return;
  std::cout << "Time-out files of method " + BaselineMethodNames[method] +
                   ":\n";
  for (uint i = 0; i < runTimeAll_.at(method).size(); i++) {
    if (runTimeAll_.at(method)[i] >= GlobalVariablesDAGOpt::TIME_LIMIT - 5) {
      std::cout << i << "\n";
    }
  }
}
void ResultsManager::PrintTimeOutRatio(BASELINEMETHODS method) const {
  if (runTimeAll_.find(method) == runTimeAll_.end()) return;
  std::cout << "Time-out ratio of method " + BaselineMethodNames[method] + ":";
  float time_out_count = 0;
  for (uint i = 0; i < runTimeAll_.at(method).size(); i++) {
    if (runTimeAll_.at(method)[i] >= GlobalVariablesDAGOpt::TIME_LIMIT) {
      time_out_count++;
    }
  }
  std::cout << time_out_count / runTimeAll_.at(method).size() << "\n";
}

// print case where method_compare perform worse than method_base
void ResultsManager::PrintWorseCase(BASELINEMETHODS method_base,
                                    BASELINEMETHODS method_compare) const {
  std::cout << "files that " << BaselineMethodNames[method_compare]
            << " performs worse than " << BaselineMethodNames[method_base]
            << " are:\n";
  if (results_map_.find(method_base) == results_map_.end() ||
      results_map_.find(method_compare) == results_map_.end()) {
    CoutWarning("Method is not found in PrintWorseCase!");
    return;
  }
  for (uint i = 0; i < objsAll_.at(method_base).size(); i++) {
    if (objsAll_.at(method_base)[i] < objsAll_.at(method_compare)[i])
      std::cout << file_name_[i] << "\n";
  }
}

}  // namespace DAG_SPACE