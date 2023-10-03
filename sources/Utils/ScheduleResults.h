#pragma once
#include <vector>

#include "sources/BatchOptimize/BaselineMethods.h"
#include "sources/Optimization/Variable.h"
#include "sources/Utils/DeclareDAG.h"
#include "sources/Utils/VariadicTable.h"

namespace DAG_SPACE {

struct ScheduleResult {
  bool schedulable_;
  double obj_;
  double jitter_;
  double timeTaken_;
  VariableOD variable_opt_;            // optional
  std::vector<double> obj_per_chain_;  // optional
};

struct BatchResult {
  double schedulableRatio;
  double performance;
  double jitter;
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
    file_name_.reserve(1e3);
  }

  void add(BASELINEMETHODS method, const ScheduleResult& result,
           std::string file);

  std::unordered_map<DAG_SPACE::BASELINEMETHODS, BatchResult> GetBatchResVec(
      const std::vector<DAG_SPACE::BASELINEMETHODS>& baselineMethods) const;

  void PrintLongestCase(BASELINEMETHODS method) const;

  // depend on global paramters: BaselineMethodNames
  void PrintResultTable(
      const std::vector<DAG_SPACE::BASELINEMETHODS>& baselineMethods) const;

  void PrintTimeOutCase() const;

  void PrintTimeOutRatio() const;

  void PrintTimeOutCaseSingleMethod(BASELINEMETHODS method) const;
  void PrintTimeOutRatio(BASELINEMETHODS method) const;

  // print case where method_compare perform worse than method_base
  void PrintWorseCase(BASELINEMETHODS method_base,
                      BASELINEMETHODS method_compare) const;

  // data members
  std::unordered_map<BASELINEMETHODS, std::vector<ScheduleResult>> results_map_;
  std::unordered_map<BASELINEMETHODS, std::vector<double>> runTimeAll_;
  std::unordered_map<BASELINEMETHODS, std::vector<double>> objsAll_;
  std::unordered_map<BASELINEMETHODS, std::vector<double>> jittersAll_;
  std::unordered_map<BASELINEMETHODS, std::vector<int>>
      schedulableAll_;  // values could
  std::vector<std::string> file_name_;
  // only be 0 / 1
};

}  // namespace DAG_SPACE