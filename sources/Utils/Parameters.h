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

// optimization settings
extern int debugMode;
extern int SKIP_PERM;
extern int TIME_LIMIT;
extern double SAMPLE_SMALL_TASKS;
extern double SAMPLE_FEASIBLE_CHAINS;
extern int FEASIBLE_CHAINS_MAX;
extern int SearchDP_Friendly;
extern const double kCplexInequalityThreshold;

// Task set settings
extern const std::string testDataSetName;
extern const std::string priorityMode;
extern const int TIME_SCALE_FACTOR;
extern const int CHAIN_NUMBER;

// used in SA
extern const double coolingRateSA;
extern const int randomInitialize;
extern const int temperatureSA;
extern const int SA_iteration;

}  // namespace GlobalVariablesDAGOpt