#pragma once
#include <Eigen/Core>
#include <iostream>
#include <vector>
// #include <Eigen/Geometry>
#include <opencv2/core/core.hpp>

#include "sources/Utils/testMy.h"
// using namespace std;
// All the global variables should be const

namespace GlobalVariablesDAGOpt {

extern const std::string PROJECT_PATH;
extern cv::FileStorage ConfigParameters;

extern int debugMode;
extern int SKIP_PERM;

// Task set settings
extern const std::string testDataSetName;
extern const std::string priorityMode;
extern const int whether_shuffle_CE_chain;
extern const int TIME_SCALE_FACTOR;
extern const int CHAIN_NUMBER;

// used in SA
extern const double coolingRateSA;
extern const int randomInitialize;
extern const int temperatureSA;
extern const int SA_iteration;

}  // namespace GlobalVariablesDAGOpt