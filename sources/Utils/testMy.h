#pragma once

#include <Eigen/Dense>
#include <bits/stdc++.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "sources/Utils/colormod.h"

inline void CoutWarning(std::string message) {
  std::cout << Color::red << message << Color::def << std::endl;
}
inline void CoutError(std::string message) {
  CoutWarning(message);
  throw;
}

/**
 * @brief This function always trigger throw, it just prints information
 *
 * @tparam T
 * @param expect
 * @param actual
 */
template <typename T> inline void AssertUnEqual(T expect, T actual, int lineNumber = 0) {
  if (lineNumber != 0)
    std::cout << Color::red << "Line Number: " << std::to_string(lineNumber) << Color::def << std::endl;
  std::cout << "Assertion failed!" << std::endl;
  std::cout << Color::red << "EXpect is " << expect << ", while the actual is " << actual << Color::def
            << std::endl;
  throw;
}
void AssertEqualScalar(double expected, double actual, double tolerance = 1e-6, int lineNumber = 0);

inline void AssertBool(bool expected, bool actual, int lineNumber = 0) {
  if (expected != actual)
    return AssertUnEqual<bool>(expected, actual, lineNumber);
}

template <typename T>
void AssertEqualVectorNoRepeat(const std::vector<T> &expected, const std::vector<T> &actual, double tolerance,
                               int lineNumber) {
  if (expected.size() != actual.size()) {
    std::cout << Color::red << "Length error! " << Color::def;
    AssertUnEqual(expected.size(), actual.size());
    return;
  }
  // size_t N = expected.size();
  std::unordered_set<T> s;
  for (size_t i = 0; i < expected.size(); i++)
    s.insert(expected.at(i));
  for (size_t i = 0; i < expected.size(); i++) {
    if (s.find(actual.at(i)) == s.end()) {
      CoutError("Actual element at index " + std::to_string(i) + " is not found in expected vector");
    } else {
      s.erase(actual.at(i));
    }
  }

  return;
}
template <typename T>
void AssertEqualVectorExact(const std::vector<T> &expected, const std::vector<T> &actual, double tolerance,
                            int lineNumber) {
  if (expected.size() != actual.size()) {
    std::cout << Color::red << "Length error! " << Color::def;
    AssertUnEqual(expected.size(), actual.size());
    return;
  }

  for (size_t i = 0; i < expected.size(); i++) {
    if (expected[i] != actual[i]) {
      CoutError("Expected element at index " + std::to_string(i) + " does not match actual element at " +
                std::to_string((i)));
    }
  }

  return;
}

template <typename T> bool CompareVector(const std::vector<T> &expected, const std::vector<T> &actual) {
  if (expected.size() != actual.size()) {
    std::cout << Color::red << "Length error! " << Color::def;
    return false;
  }
  for (size_t i = 0; i < expected.size(); i++) {
    if (expected[i] != actual[i]) {
      return false;
    }
  }

  return true;
}

void AssertEigenEqualVector(Eigen::Matrix<double, Eigen::Dynamic, 1> expected,
                            Eigen::Matrix<double, Eigen::Dynamic, 1> actual, int lineNumber = 0);

void AssertEigenEqualMatrix(Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> expected,
                            Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> actual, int lineNumber = 0);

template <class T1, class T2>
void AssertEqualMap(std::unordered_map<T1, T2> &mExpect, std::unordered_map<T1, T2> &mActual) {
  using namespace std;
  if (mExpect.size() != mActual.size()) {
    CoutWarning("Size error!");
    AssertEqualScalar(mExpect.size(), mActual.size());
  }
  for (auto itr = mExpect.begin(); itr != mExpect.end(); itr++) {
    auto itrActual = mActual.find(itr->first);
    if (itrActual == mActual.end() || (itrActual->second).notEqual(itr->second)) {

      try {
        std::cout << "Expect is " << itr->first << ", " << itr->second << std::endl;
        std::cout << "Actual is " << itrActual->first << ", " << itrActual->second << std::endl;
      } catch (const std::exception &e) {
        std::cout << "Cannot print the key to show mismatch element\n";
      }
      CoutError("Element in mExpect is not found in or not equal to mActual!");
    }
  }
}
double SquareError(std::vector<double> &seq);