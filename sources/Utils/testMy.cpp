#include "sources/Utils/testMy.h"

void AssertEqualScalar(double expected, double actual, double tolerance, int lineNumber) {
  if (expected != 0) {
    if (abs((expected - actual) / expected) < tolerance)
      return;
    else {
      if (lineNumber != 0)
        std::cout << Color::red << "Line Number: " << std::to_string(lineNumber) << Color::def << std::endl;
      AssertUnEqual<double>(expected, actual);
    }
  } else {
    if (actual != 0) {
      if (lineNumber != 0)
        std::cout << Color::red << "Line Number: " << std::to_string(lineNumber) << Color::def << std::endl;
      AssertUnEqual<double>(expected, actual);
    }
  }
}

void AssertEigenEqualVector(Eigen::Matrix<double, Eigen::Dynamic, 1> expected,
                            Eigen::Matrix<double, Eigen::Dynamic, 1> actual, int lineNumber) {
  int m = expected.rows();
  int n = expected.cols();
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      AssertEqualScalar(expected(i, j), actual(i, j), 1e-6, lineNumber);
    }
  }
}

void AssertEigenEqualMatrix(Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> expected,
                            Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> actual, int lineNumber) {
  int m = expected.rows();
  int n = expected.cols();
  AssertEqualScalar(m, actual.rows(), 1e-6, lineNumber);
  AssertEqualScalar(n, actual.cols(), 1e-6, lineNumber);
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      AssertEqualScalar(expected(i, j), actual(i, j), 1e-6, lineNumber);
    }
  }
}

double SquareError(std::vector<double> &seq) {
  double res = 0;
  for (double x : seq) {
    res += x * x / 2.0;
  }
  return res;
}