#pragma once

#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>
// #include <filesystem>
#include <boost/function.hpp>
#include <boost/integer/common_factor.hpp>
// #include <CppUnitLite/TestHarness.h>

#include "sources/Utils/colormod.h"

#include "sources/Utils/DeclareDAG.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/testMy.h"
// using namespace std;

typedef std::map<int, std::vector<int>> ProcessorTaskSet;

namespace RegularTaskSystem {

inline bool CompareStringNoCase(const std::string &s1, const std::string s2) {
  return strcasecmp(s1.c_str(), s2.c_str()) == 0;
}
class Task {
public:
  // Member list
  int offset;
  int period;
  int overhead;
  double executionTime;
  int deadline;
  int id;
  int processorId;
  int coreRequire;
  int taskType;
  int priority_; // its value needs to be assigned by other functions before usage
  std::string priorityType_;

  // initializer

  Task()
      : offset(0), period(0), overhead(0), executionTime(0.0), deadline(0), id(-1), processorId(-1),
        coreRequire(1), taskType(0), priority_(-1) {}
  Task(int offset, int period, int overhead, double executionTime, int deadline)
      : offset(offset), period(period), overhead(overhead), executionTime(executionTime), deadline(deadline),
        taskType(0) {
    id = -1;
    processorId = -1;
    coreRequire = 1;
    priorityType_ = GlobalVariablesDAGOpt::priorityMode;
  }
  Task(int offset, int period, int overhead, double executionTime, int deadline, int id, int processorId)
      : offset(offset), period(period), overhead(overhead), executionTime(executionTime), deadline(deadline),
        id(id), processorId(processorId), taskType(0), priority_(-1) {
    coreRequire = 1;
    priorityType_ = GlobalVariablesDAGOpt::priorityMode;
  }
  Task(int offset, int period, int overhead, double executionTime, int deadline, int id, int processorId,
       int coreRequire)
      : offset(offset), period(period), overhead(overhead), executionTime(executionTime), deadline(deadline),
        id(id), processorId(processorId), coreRequire(coreRequire), taskType(0), priority_(-1) {
    priorityType_ = GlobalVariablesDAGOpt::priorityMode;
  }
  Task(int offset, int period, int overhead, double executionTime, int deadline, int id, int processorId,
       int coreRequire, int taskType)
      : offset(offset), period(period), overhead(overhead), executionTime(executionTime), deadline(deadline),
        id(id), processorId(processorId), coreRequire(coreRequire), taskType(taskType), priority_(-1) {
    priorityType_ = GlobalVariablesDAGOpt::priorityMode;
  }

  // modify public member priorityType_ to change how to calculate the value: priority_
  double priority() {
    if (CompareStringNoCase(priorityType_, "RM")) {
      if (period > 0)
        return 1.0 / period;
      else
        CoutError("Period parameter less or equal to 0!");
    } else if (CompareStringNoCase(priorityType_, "orig"))
      return id;
    else if (CompareStringNoCase(priorityType_, "assigned"))
      return priority_;
    else
      CoutError("Priority settings not recognized!");
    return -1;
  }
  /**
   * only used in ReadTaskSet because the input parameter's type is int
   **/
  Task(std::vector<double> dataInLine) {
    if (dataInLine.size() != 8) {
      // std::cout << Color::red << "The length of dataInLine in Task constructor is wrong! Must be 8!\n"
      //       << Color::def <<std::endl;
      //  throw;
    }
    id = dataInLine[0];
    offset = dataInLine[1];
    period = dataInLine[2];
    overhead = dataInLine[3];
    executionTime = dataInLine[4];
    deadline = dataInLine[5];
    processorId = dataInLine[6];
    coreRequire = dataInLine[7];
    taskType = 0;
    if (dataInLine.size() > 8)
      taskType = dataInLine[8];
    if (coreRequire < 1)
      coreRequire = 1;
  }

  void print() {
    std::cout << "The period is: " << period << " The executionTime is " << executionTime
              << " The deadline is " << deadline << " The overhead is " << overhead << " The offset is "
              << offset << " The coreRequire is " << coreRequire << " The taskType is " << taskType
              << std::endl;
  }

  double utilization() const { return double(executionTime) / period; }

  int slack() { return deadline - executionTime; }
};

typedef std::vector<RegularTaskSystem::Task> TaskSet;

inline void Print(TaskSet &tasks) {
  std::cout << "The task set is printed as follows" << std::endl;
  for (auto &task : tasks)
    task.print();
}

template <typename T> std::vector<T> GetParameter(const TaskSet &taskset, std::string parameterType);
template <typename T> VectorDynamic GetParameterVD(const TaskSet &taskset, std::string parameterType);

// some helper function for Reorder
inline static bool comparePeriod(Task task1, Task task2) { return task1.period < task2.period; };
inline static bool compareDeadline(Task task1, Task task2) { return task1.deadline < task2.deadline; };

inline bool compareUtilization(Task task1, Task task2) { return task1.utilization() < task2.utilization(); };

double Utilization(const TaskSet &tasks);

// Recursive function to return gcd of a and b
long long gcd(long long int a, long long int b);

// Function to return LCM of two numbers
long long int lcm(long long int a, long long int b);

long long int HyperPeriod(const TaskSet &tasks);

// should not be used anymore
TaskSet Reorder(TaskSet tasks, std::string priorityType);
TaskSet ReadTaskSet(std::string path, std::string priorityType = "RM", int taskSetType = 1);

void UpdateTaskSetExecutionTime(TaskSet &taskSet, VectorDynamic executionTimeVec,
                                int lastTaskDoNotNeedOptimize = -1);
/**
 * @brief
 *
 * @param tasks
 * @return ProcessorTaskSet map type! processorId tostd::vector<task ID>
 */
ProcessorTaskSet ExtractProcessorTaskSet(const TaskSet &tasks);

class TaskSetInfoDerived {
public:
  TaskSet tasks;
  int N;
  LLint hyperPeriod;
  LLint variableDimension;
  std::vector<LLint> sizeOfVariables;
  LLint length;
  ProcessorTaskSet processorTaskSet;

  TaskSetInfoDerived() {}

  TaskSetInfoDerived(const TaskSet &tasksInput) {
    tasks = tasksInput;
    N = tasks.size();
    hyperPeriod = HyperPeriod(tasks);
    variableDimension = 0;
    length = 0;
    for (int i = 0; i < N; i++) {
      LLint size = hyperPeriod / tasks[i].period;
      sizeOfVariables.push_back(size);
      variableDimension += size;
      length += sizeOfVariables[i];
    }
    processorTaskSet = ExtractProcessorTaskSet(tasks);
  }
};

} // namespace RegularTaskSystem