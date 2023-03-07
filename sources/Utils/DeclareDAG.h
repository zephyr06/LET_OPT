#pragma once

#include <algorithm>
#include <chrono>
#include <math.h>
#include <unordered_map>

#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <dirent.h>
#include <gtsam/base/numericalDerivative.h>
#include <gtsam/geometry/Point3.h>
#include <gtsam/inference/FactorGraph.h>
#include <gtsam/inference/Key.h>
#include <gtsam/inference/Symbol.h>
#include <gtsam/linear/VectorValues.h>
#include <gtsam/nonlinear/DoglegOptimizer.h>
#include <gtsam/nonlinear/GaussNewtonOptimizer.h>
#include <gtsam/nonlinear/LevenbergMarquardtOptimizer.h>
#include <gtsam/nonlinear/NonlinearFactorGraph.h>
#include <gtsam/nonlinear/Values.h>

#include <boost/function.hpp>

#include "sources/Utils/MatirxConvenient.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/colormod.h"
// #include "sources/Utils/profilier.h"
#include "sources/Utils/testMy.h"

// using namespace std;
// using namespace gtsam;

typedef boost::function<VectorDynamic(const VectorDynamic &)> NormalErrorFunction1D;
typedef boost::function<VectorDynamic(const VectorDynamic &, const VectorDynamic &)> NormalErrorFunction2D;
typedef boost::function<gtsam::Vector(const gtsam::Values &x)> LambdaMultiKey;

typedef long long int LLint;

/**
 * @brief given task index and instance index, return variable's index in StartTimeVector
 *
 * @param i
 * @param instance_i
 * @param sizeOfVariables
 * @return LLint
 */
LLint IndexTran_Instance2Overall(LLint i, LLint instance_i, const std::vector<LLint> &sizeOfVariables);

/**
 * @brief Given index in startTimeVector, decode its task index
 *
 * @param index
 * @param sizeOfVariables
 * @return int: task index
 */
int BigIndex2TaskIndex(LLint index, const std::vector<LLint> &sizeOfVariables);

/**
 * @brief generate key symbol for use in GTSAM
 *
 * @param idtask task index
 * @param j job index
 * @return gtsam::Symbol
 */
inline gtsam::Symbol GenerateKey(int idtask, LLint j) {
  if (idtask > 100 || j > 100000 || idtask < 0 || j < 0) {
    CoutWarning("Invalid index in GenerateKey!");
  }
  gtsam::Symbol key('a' + idtask, j);
  return key;
}

// return (taskId, jobId)
inline std::pair<int, LLint> AnalyzeKey(gtsam::Symbol key) {
  int id = key.chr() - 'a';
  LLint index = key.index();
  return std::make_pair(id, index);
}

struct MappingDataStruct {
  LLint index;
  double distance;

  MappingDataStruct() : index(0), distance(0) {}
  MappingDataStruct(LLint index, double distance) : index(index), distance(distance) {}

  LLint getIndex() const { return index; }
  double getDistance() const { return distance; }
  bool notEqual(const MappingDataStruct &m1, double tolerance = 1e-3) {
    if (abs(this->index - m1.getIndex()) > tolerance || abs(this->distance - m1.getDistance()) > tolerance)
      return true;
    return false;
  }
};
inline std::ostream &operator<<(std::ostream &os, MappingDataStruct const &m) {
  return os << m.getIndex() << ", " << m.getDistance() << std::endl;
}

typedef std::unordered_map<int, MappingDataStruct> MAP_Index2Data;
typedef boost::function<VectorDynamic(const VectorDynamic &)> FuncV2V;

// ************************************************************ SOME FUNCTIONS
/**
 * @brief Given a task index and instance index, return its start time in startTimeVector
 *
 * @param startTimeVector large, combined vector of start time for all instances
 * @param taskIndex task-index
 * @param instanceIndex instance-index
 * @return double start time of the extracted instance
 */
double ExtractVariable(const VectorDynamic &startTimeVector, const std::vector<LLint> &sizeOfVariables,
                       int taskIndex, int instanceIndex);

VectorDynamic CompresStartTimeVector(const VectorDynamic &startTimeComplete,
                                     const std::vector<bool> &maskForEliminate);

inline double QuotientDouble(double a, int b) {
  double left = a - int(a);
  return left + int(a) % b;
}

/**
 * barrier function for the optimization
 **/
inline double Barrier(double x) {
  if (x >= 0)
    // return pow(x, 2);
    return 0;
  else if (x < 0) {
    return -1 * x;
  }
  // else // it basically means x=0
  //     return weightLogBarrier *
  //            log(x + toleranceBarrier);
  return 0;
}

double BarrierLog(double x);

MatrixDynamic NumericalDerivativeDynamic(boost::function<VectorDynamic(const VectorDynamic &)> h,
                                         const VectorDynamic &x, double deltaOptimizer, int mOfJacobian);

MatrixDynamic NumericalDerivativeDynamic2D1(NormalErrorFunction2D h, const VectorDynamic &x1,
                                            const VectorDynamic &x2, double deltaOptimizer, int mOfJacobian);

MatrixDynamic NumericalDerivativeDynamic2D2(NormalErrorFunction2D h, const VectorDynamic &x1,
                                            const VectorDynamic &x2, double deltaOptimizer, int mOfJacobian);

/**
 * @brief helper function for RecoverStartTimeVector
 *
 * @param index
 * @param actual
 * @param mapIndex
 * @param filledTable
 * @return double
 */
double GetSingleElement(LLint index, VectorDynamic &actual, const MAP_Index2Data &mapIndex,
                        std::vector<bool> &filledTable);

inline void UpdateSM(double val, LLint i, LLint j, SM_Dynamic &sm) { sm.coeffRef(i, j) = val; }

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

template <typename T> double Average(std::vector<T> &data) {
  if (data.size()) {
    T sum = 0;
    for (int i = 0; i < int(data.size()); i++)
      sum += data[i];
    return double(sum) / data.size();
  } else {
    return -1;
  }
}

template <typename T> double Average(std::vector<T> &data, std::vector<bool> &valid) {
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