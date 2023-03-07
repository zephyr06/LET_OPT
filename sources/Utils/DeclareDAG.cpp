#include "sources/Utils/DeclareDAG.h"

LLint IndexTran_Instance2Overall(LLint i, LLint instance_i, const std::vector<LLint> &sizeOfVariables) {
  if (instance_i < 0 || instance_i > sizeOfVariables[i])
    CoutError("Instance Index out of boundary in IndexTran_Instance2Overall");
  if (i < 0 || i > (LLint)sizeOfVariables.size())
    CoutError("Task Index out of boundary in IndexTran_Instance2Overall");
  LLint index = 0;
  for (size_t k = 0; k < (size_t)i; k++)
    index += sizeOfVariables[k];
  return index + instance_i;
}

int BigIndex2TaskIndex(LLint index, const std::vector<LLint> &sizeOfVariables) {
  int taskIndex = 0;
  int N = sizeOfVariables.size();
  while (index >= 0 && taskIndex < N) {
    index -= sizeOfVariables[taskIndex];
    taskIndex++;
  }
  return taskIndex - 1;
}

double ExtractVariable(const VectorDynamic &startTimeVector, const std::vector<LLint> &sizeOfVariables,
                       int taskIndex, int instanceIndex) {
  if (taskIndex < 0 || instanceIndex < 0 || instanceIndex > sizeOfVariables[taskIndex] - 1) {

    std::cout << Color::red << "Index Error in ExtractVariable!" << Color::def << std::endl;
    throw;
  }

  LLint firstTaskInstance = 0;
  for (int i = 0; i < taskIndex; i++) {
    firstTaskInstance += sizeOfVariables[i];
  }
  return startTimeVector(firstTaskInstance + instanceIndex, 0);
}

VectorDynamic CompresStartTimeVector(const VectorDynamic &startTimeComplete,
                                     const std::vector<bool> &maskForEliminate) {
  int variableDimension = maskForEliminate.size();
  std::vector<double> initialUpdateVec;
  initialUpdateVec.reserve(variableDimension - 1);

  for (int i = 0; i < variableDimension; i++) {
    if (not maskForEliminate[i]) {
      initialUpdateVec.push_back(startTimeComplete(i, 0));
    }
  }
  VectorDynamic initialUpdate;
  initialUpdate.resize(initialUpdateVec.size(), 1);
  for (size_t i = 0; i < initialUpdateVec.size(); i++) {
    initialUpdate(i, 0) = initialUpdateVec[i];
  }
  return initialUpdate;
}

double BarrierLog(double x) {
  if (x >= 0)
    // return pow(x, 2);
    return GlobalVariablesDAGOpt::weightLogBarrier * log(x + 1) + GlobalVariablesDAGOpt::barrierBase;
  else if (x < 0) {
    return GlobalVariablesDAGOpt::punishmentInBarrier * pow(1 - x, 2);
  }
  // else // it basically means x=0
  //     return weightLogBarrier *
  //            log(x + toleranceBarrier);
  return 0;
}
MatrixDynamic NumericalDerivativeDynamic(boost::function<VectorDynamic(const VectorDynamic &)> h,
                                         const VectorDynamic &x, double deltaOptimizer, int mOfJacobian) {
  int n = x.rows();
  MatrixDynamic jacobian;
  jacobian.resize(mOfJacobian, n);
  // VectorDynamic currErr = h(x);

  for (int i = 0; i < n; i++) {
    VectorDynamic xDelta = x;
    xDelta(i, 0) = xDelta(i, 0) + deltaOptimizer;
    VectorDynamic resPlus;
    resPlus.resize(mOfJacobian, 1);
    resPlus = h(xDelta);
    xDelta(i, 0) = xDelta(i, 0) - 2 * deltaOptimizer;
    VectorDynamic resMinus;
    resMinus.resize(mOfJacobian, 1);
    resMinus = h(xDelta);

    for (int j = 0; j < mOfJacobian; j++) {
      jacobian(j, i) = (resPlus(j, 0) - resMinus(j, 0)) / 2 / deltaOptimizer;
      // jacobian(j, i) = (resMinus(j, 0) - currErr(j, 0)) / deltaOptimizer * -1;
      // jacobian(j, i) = (resPlus(j, 0) - currErr(j, 0)) / deltaOptimizer;
    }
  }
  return jacobian;
}

MatrixDynamic NumericalDerivativeDynamic2D1(NormalErrorFunction2D h, const VectorDynamic &x1,
                                            const VectorDynamic &x2, double deltaOptimizer, int mOfJacobian) {
  int n = x1.rows();
  MatrixDynamic jacobian;
  jacobian.resize(mOfJacobian, n);
  NormalErrorFunction1D f = [h, x2](const VectorDynamic &x1) { return h(x1, x2); };

  return NumericalDerivativeDynamic(f, x1, deltaOptimizer, mOfJacobian);
}

MatrixDynamic NumericalDerivativeDynamic2D2(NormalErrorFunction2D h, const VectorDynamic &x1,
                                            const VectorDynamic &x2, double deltaOptimizer, int mOfJacobian) {
  int n = x2.rows();
  MatrixDynamic jacobian;
  jacobian.resize(mOfJacobian, n);
  NormalErrorFunction1D f = [h, x1](const VectorDynamic &x2) { return h(x1, x2); };

  return NumericalDerivativeDynamic(f, x2, deltaOptimizer, mOfJacobian);
}

double GetSingleElement(LLint index, VectorDynamic &actual, const MAP_Index2Data &mapIndex,
                        std::vector<bool> &filledTable) {
  if (filledTable[index])
    return actual(index, 0);
  auto it = mapIndex.find(index);

  if (it != mapIndex.end()) {
    MappingDataStruct curr = it->second;
    actual(index, 0) = GetSingleElement(curr.getIndex(), actual, mapIndex, filledTable) + curr.getDistance();
    filledTable[index] = true;
  } else {
    CoutError("Out of boundary in GetSingleElement, RecoverStartTimeVector!");
  }
  return actual(index, 0);
}

double RandRange(double a, double b) {
  if (b < a) {
    // CoutError("Range Error in RandRange");
    return a + (b - a) * double(rand()) / RAND_MAX;
  }
  return a + (b - a) * double(rand()) / RAND_MAX;
}

std::string ResizeStr(std::string s, uint size) {
  if (s.size() > size) {
    return s.substr(0, size);
  } else {
    std::string res = "";
    for (size_t i = 0; i < size - s.size(); i++) {
      res += " ";
    }
    return res + s;
  }
}
