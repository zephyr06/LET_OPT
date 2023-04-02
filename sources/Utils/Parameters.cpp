#include "Parameters.h"

namespace GlobalVariablesDAGOpt {
const std::string PROJECT_PATH = "/home/zephyr/Programming/LET_OPT/";
// const std::string PROJECT_PATH = "/home/dong/workspace/LET_OPT/";
cv::FileStorage ConfigParameters(GlobalVariablesDAGOpt::PROJECT_PATH +
                                     "sources/parameters.yaml",
                                 cv::FileStorage::READ);

// optimization settings
int debugMode = (int)ConfigParameters["debugMode"];
int SKIP_PERM = (int)ConfigParameters["SKIP_PERM"];
int TIME_LIMIT = (int)ConfigParameters["TIME_LIMIT"];
double SAMPLE_SMALL_TASKS = (double)ConfigParameters["SAMPLE_SMALL_TASKS"];
double SAMPLE_FEASIBLE_CHAINS = (double)ConfigParameters["SAMPLE_FEASIBLE_CHAINS"];
const double kCplexInequalityThreshold =
    (double)ConfigParameters["kCplexInequalityThreshold"];
int SearchDP_Friendly = (int)ConfigParameters["SearchDP_Friendly"];
// Task set settings
const std::string testDataSetName =
    (std::string)ConfigParameters["testDataSetName"];
const std::string priorityMode = (std::string)ConfigParameters["priorityMode"];
const int TIME_SCALE_FACTOR = (int)ConfigParameters["TIME_SCALE_FACTOR"];
const int CHAIN_NUMBER = (int)ConfigParameters["CHAIN_NUMBER"];

// *************************************************************

// SA
const double coolingRateSA = (double)ConfigParameters["coolingRateSA"];
const int randomInitialize = (int)ConfigParameters["randomInitialize"];
const int temperatureSA = (int)ConfigParameters["temperatureSA"];
const int SA_iteration = (int)ConfigParameters["SA_iteration"];
// *************************************************************

}  // namespace GlobalVariablesDAGOpt