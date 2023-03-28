#pragma once
#include <vector>

#include "sources/Utils/DeclareDAG.h"
#include "sources/Utils/VariadicTable.h"

namespace DAG_SPACE {
// !!!if adding more methods, need to update BaselineMethodNames below !!!
enum BASELINEMETHODS {
  InitialMethod,  // 0
  TOM,            // 1
  TOM_DP,         // 2
  SA              // 3
};
const std::vector<std::string> BaselineMethodNames = {"InitialMethod", "TOM",
                                                      "TOM_DP", "SA"};

struct ScheduleResult {
  bool schedulable_;
  double obj_;
  double timeTaken_;
};

struct BatchResult {
  double schedulableRatio;
  double performance;
  double runTime;
};

class ResultsManager {
 public:
  ResultsManager() {}
  ResultsManager(
      const std::vector<DAG_SPACE::BASELINEMETHODS>& baselineMethods) {
    results_map_.reserve(baselineMethods.size());
    for (const auto& method : baselineMethods) results_map_[method] = {};
    runTimeAll_.reserve(baselineMethods.size());
    objsAll_.reserve(baselineMethods.size());
    schedulableAll_.reserve(baselineMethods.size());
  }

  void add(BASELINEMETHODS method, const ScheduleResult& result);

  std::vector<BatchResult> GetBatchResVec(
      const std::vector<DAG_SPACE::BASELINEMETHODS>& baselineMethods) const;

  void PrintLongestCase(BASELINEMETHODS method) const;

  // depend on global paramters: BaselineMethodNames
  void PrintResultTable(
      const std::vector<DAG_SPACE::BASELINEMETHODS>& baselineMethods) const;

  void PrintTimeOutCase() const;

  void PrintTimeOutCaseSingleMethod(BASELINEMETHODS method) const;

  // data members
  std::unordered_map<BASELINEMETHODS, std::vector<ScheduleResult>> results_map_;
  std::unordered_map<BASELINEMETHODS, std::vector<double>> runTimeAll_;
  std::unordered_map<BASELINEMETHODS, std::vector<double>> objsAll_;
  std::unordered_map<BASELINEMETHODS, std::vector<int>>
      schedulableAll_;  // values could
  // only be 0 / 1
};

}  // namespace DAG_SPACE