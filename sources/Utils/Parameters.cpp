#include "Parameters.h"

#include <yaml-cpp/yaml.h>

namespace GlobalVariablesDAGOpt {
// const std::string PROJECT_PATH = "/home/zephyr/Programming/LET_OPT/";
const std::string PROJECT_PATH = std::string(PROJECT_ROOT_DIR) + "/";
// const std::string PROJECT_PATH = "/home/swang666/Programming/LET_OPT/";
// const std::string PROJECT_PATH = "/home/dong/workspace/LET_OPT/";
YAML::Node loaded_doc = YAML::LoadFile(GlobalVariablesDAGOpt::PROJECT_PATH +
                                       "sources/parameters.yaml");

// optimization settings
int debugMode = loaded_doc["debugMode"].as<int>();
int SKIP_PERM = loaded_doc["SKIP_PERM"].as<int>();
int TIME_LIMIT = loaded_doc["TIME_LIMIT"].as<int>();
double SAMPLE_SMALL_TASKS = loaded_doc["SAMPLE_SMALL_TASKS"].as<double>();
double SAMPLE_FEASIBLE_CHAINS =
    loaded_doc["SAMPLE_FEASIBLE_CHAINS"].as<double>();
int FEASIBLE_CHAINS_MAX = loaded_doc["FEASIBLE_CHAINS_MAX"].as<int>();
int FEASIBLE_INCOMPLETE_CHAINS_MAX =
    loaded_doc["FEASIBLE_INCOMPLETE_CHAINS_MAX"].as<int>();

const double kCplexInequalityThreshold =
    loaded_doc["kCplexInequalityThreshold"].as<double>();
// Task set settings
const std::string testDataSetName =
    loaded_doc["testDataSetName"].as<std::string>();
double OPTIMIZE_JITTER_WEIGHT =
    loaded_doc["OPTIMIZE_JITTER_WEIGHT"].as<double>();

const std::string priorityMode = loaded_doc["priorityMode"].as<std::string>();
const int TIME_SCALE_FACTOR = loaded_doc["TIME_SCALE_FACTOR"].as<int>();
const int CHAIN_NUMBER = loaded_doc["CHAIN_NUMBER"].as<int>();

// *************************************************************

// SA
const double coolingRateSA = loaded_doc["coolingRateSA"].as<double>();
const int randomInitialize = loaded_doc["randomInitialize"].as<int>();
const int temperatureSA = loaded_doc["temperatureSA"].as<int>();
const int SA_iteration = loaded_doc["SA_iteration"].as<int>();
// *************************************************************

}  // namespace GlobalVariablesDAGOpt