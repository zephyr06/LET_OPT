#pragma once
#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <fstream>
#include <iostream>

typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> MatrixDynamic;
typedef Eigen::Matrix<double, Eigen::Dynamic, 1> VectorDynamic;
typedef Eigen::SparseMatrix<double, Eigen::ColMajor> SM_Dynamic;
typedef long long int LLint;

inline double min(double a, double b) {
  if (a <= b)
    return a;
  else
    return b;
  return 0;
}
inline double max(double a, double b) {
  if (a >= b)
    return a;
  else
    return b;
  return 0;
}
void swap(VectorDynamic &x, LLint i, LLint j);

inline MatrixDynamic GenerateMatrixDynamic(int m, int n) {
  MatrixDynamic M;
  M.resize(m, n);
  M.setZero();
  return M;
}

MatrixDynamic GenerateOneMatrix(int m, int n);

// template <class T>
// std::vector<T> Eigen2Vector(const VectorDynamic &input);

// Templated function can't be put inito source files
template <class T> inline std::vector<T> Eigen2Vector(const VectorDynamic &input) {
  std::vector<T> res;
  LLint len = input.rows();
  res.reserve(len);
  for (LLint i = 0; i < len; i++)
    res.push_back(input.coeff(i, 0));
  return res;
}
template <class T> VectorDynamic Vector2Eigen(const std::vector<T> &input);

inline VectorDynamic GenerateVectorDynamic(LLint N) {
  VectorDynamic v;
  v.resize(N, 1);
  v.setZero();
  return v;
}

inline VectorDynamic GenerateVectorDynamic1D(double x) {
  VectorDynamic res = GenerateVectorDynamic(1);
  res << x;
  return res;
}

template <typename Derived> inline bool eigen_is_nan(const Eigen::MatrixBase<Derived> &x) {
  return Eigen::isnan(x.array()).any();
}

template <typename Derived> inline bool eigen_is_inf(const Eigen::MatrixBase<Derived> &x) {
  return Eigen::isinf(x.array()).any();
}
inline void WriteMatrixToFile(std::string filePath, const MatrixDynamic &m) {
  std::ofstream file;
  file.open(filePath);
  if (file.is_open()) {
    file << m << '\n';
    // file << "m" << '\n' <<  colm(m) << '\n';
  } else {
    std::cout << "Error writing the matrix to file\n";
  }
  file.close();
}
