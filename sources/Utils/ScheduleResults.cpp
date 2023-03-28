
#include "sources/Utils/ScheduleResults.h"
namespace DAG_SPACE {

void ResultsManager::add(BaselineMethods method, const ScheduleResult& result) {
  results_map_[method].push_back(result);
  runTimeAll_[method].push_back(result.timeTaken_);
  objsAll_[method].push_back(result.obj_);
  schedulableAll_[method].push_back(result.schedulable_);
}

std::vector<BatchResult> ResultsManager::GetBatchResVec(
    const std::vector<DAG_SPACE::BaselineMethods>& baselineMethods) const {
  std::vector<BatchResult> batchResVec;
  for (auto method : baselineMethods) {
    BatchResult batchRes{Average(schedulableAll_.at(method)),
                         Average(objsAll_.at(method),
                                 objsAll_.at(BaselineMethods::InitialMethod)),
                         Average(runTimeAll_.at(method))};
    batchResVec.push_back(batchRes);
  }
  return batchResVec;
}

void ResultsManager::PrintLongestCase(BaselineMethods method) const {
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
    const std::vector<DAG_SPACE::BaselineMethods>& baselineMethods) const {
  VariadicTable<std::string, double, double, double, double> vt(
      {"Method", "Schedulable ratio", "Obj", "Obj(Norm)", "TimeTaken"}, 10);
  for (const auto& method : baselineMethods) {
    vt.addRow(BaselineMethodNames[method], Average(schedulableAll_.at(method)),
              Average(objsAll_.at(method)),
              Average(objsAll_.at(method),
                      objsAll_.at(BaselineMethods::InitialMethod)),
              Average(runTimeAll_.at(method)));
  }
  vt.print(std::cout);
}

void ResultsManager::PrintTimeOutCase() const {
  for (auto itr = runTimeAll_.begin(); itr != runTimeAll_.end(); itr++) {
    PrintTimeOutCaseSingleMethod(itr->first);
  }
}

void ResultsManager::PrintTimeOutCaseSingleMethod(
    BaselineMethods method) const {
  std::cout << "Time-out files of method " + BaselineMethodNames[method] +
                   ":\n";
  for (uint i = 0; i < runTimeAll_.at(method).size(); i++) {
    if (runTimeAll_.at(method)[i] >= GlobalVariablesDAGOpt::TIME_LIMIT - 5) {
      std::cout << i << "\n";
    }
  }
}

}  // namespace DAG_SPACE