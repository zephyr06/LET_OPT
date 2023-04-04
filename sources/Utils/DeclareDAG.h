#pragma once

#include <dirent.h>
#include <math.h>

#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <algorithm>
#include <chrono>
#include <unordered_map>

#include "sources/Utils/MatirxConvenient.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/colormod.h"
// #include "sources/Utils/profilier.h"
#include "sources/Utils/testMy.h"

// using namespace std;

typedef long long int LLint;

/**
 * @brief given task index and instance index, return variable's index in
 * StartTimeVector
 *
 * @param i
 * @param instance_i
 * @param sizeOfVariables
 * @return LLint
 */
LLint IndexTran_Instance2Overall(LLint i, LLint instance_i,
                                 const std::vector<LLint> &sizeOfVariables);

/**
 * @brief Given index in startTimeVector, decode its task index
 *
 * @param index
 * @param sizeOfVariables
 * @return int: task index
 */
int BigIndex2TaskIndex(LLint index, const std::vector<LLint> &sizeOfVariables);

struct MappingDataStruct {
  LLint index;
  double distance;

  MappingDataStruct() : index(0), distance(0) {}
  MappingDataStruct(LLint index, double distance)
      : index(index), distance(distance) {}

  LLint getIndex() const { return index; }
  double getDistance() const { return distance; }
  bool notEqual(const MappingDataStruct &m1, double tolerance = 1e-3) {
    if (abs(this->index - m1.getIndex()) > tolerance ||
        abs(this->distance - m1.getDistance()) > tolerance)
      return true;
    return false;
  }
};
inline std::ostream &operator<<(std::ostream &os, MappingDataStruct const &m) {
  return os << m.getIndex() << ", " << m.getDistance() << std::endl;
}

typedef std::unordered_map<int, MappingDataStruct> MAP_Index2Data;

// ************************************************************ SOME FUNCTIONS
/**
 * @brief Given a task index and instance index, return its start time in
 * startTimeVector
 *
 * @param startTimeVector large, combined vector of start time for all instances
 * @param taskIndex task-index
 * @param instanceIndex instance-index
 * @return double start time of the extracted instance
 */
double ExtractVariable(const VectorDynamic &startTimeVector,
                       const std::vector<LLint> &sizeOfVariables, int taskIndex,
                       int instanceIndex);

inline double QuotientDouble(double a, int b) {
  double left = a - int(a);
  return left + int(a) % b;
}

/**
 * @brief helper function for RecoverStartTimeVector
 *
 * @param index
 * @param actual
 * @param mapIndex
 * @param filledTable
 * @return double
 */
double GetSingleElement(LLint index, VectorDynamic &actual,
                        const MAP_Index2Data &mapIndex,
                        std::vector<bool> &filledTable);

inline void UpdateSM(double val, LLint i, LLint j, SM_Dynamic &sm) {
  sm.coeffRef(i, j) = val;
}

/**
 * @brief generate a random number within the range [a,b];
 * a must be smaller than b
 *
 * @param a
 * @param b
 * @return double
 */
double RandRange(double a, double b);

inline void AddEntry(std::string pathRes, std::string s) {
  std::ofstream outfileWrite;
  outfileWrite.open(pathRes, std::ios_base::app);
  outfileWrite << s << std::endl;
  outfileWrite.close();
}

std::string ResizeStr(std::string s, uint size = 20);

// template <typename T>
// double Average(std::vector<T> &data);

template <typename T>
double Average(const std::vector<T> &data) {
  if (data.size()) {
    T sum = 0;
    for (int i = 0; i < int(data.size()); i++) sum += data[i];
    return double(sum) / data.size();
  } else {
    return -1;
  }
}

template <typename T>
double Average(const std::vector<T> &data,
               const std::vector<T> &data_baseline) {
  if (data.size() != data_baseline.size())
    CoutError("Inconsistent data size in Average!");
  if (data.size()) {
    T sum = 0;
    for (int i = 0; i < int(data.size()); i++)
      sum += double(data[i]) / data_baseline[i];
    return double(sum) / data.size();
  } else {
    return -1;
  }
}

template <typename T>
double Average(const std::vector<T> &data, const std::vector<bool> &valid) {
  if (data.size()) {
    T sum = 0;
    int count = 0;
    for (int i = 0; i < int(data.size()); i++) {
      if (valid[i]) {
        sum += data[i];
        count++;
      }
    }

    return double(sum) / count;
  } else {
    return -1;
  }
}