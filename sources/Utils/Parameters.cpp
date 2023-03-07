#include "Parameters.h"

namespace GlobalVariablesDAGOpt {
const std::string PROJECT_PATH = "/home/zephyr/Programming/DAG_NLP/";
// const std::string PROJECT_PATH = "/home/dong/workspace/DAG_NLP/";
// const std::string PROJECT_PATH = "/home/zephyr/Programming/batch_test_DAG_NLP/VerucchiOnly/N3_10/";
//  std::string path = GlobalVariablesDAGOpt::PROJECT_PATH +  "sources/parameters.yaml";
cv::FileStorage ConfigParameters(GlobalVariablesDAGOpt::PROJECT_PATH + "sources/parameters.yaml",
                                 cv::FileStorage::READ);

const double barrierBase = (double)ConfigParameters["barrierBase"];
const double coolingRateSA = (double)ConfigParameters["coolingRateSA"];
const double deltaInitialDogleg = (double)ConfigParameters["deltaInitialDogleg"];
double deltaOptimizer =
    (double)ConfigParameters["deltaOptimizer"]; // not used in the main SF optimization program
const int timeScaleFactor = (int)ConfigParameters["timeScaleFactor"];

const int RepeatExecution = (int)ConfigParameters["RepeatExecution"];
const double initialLambda = (double)ConfigParameters["initialLambda"];
const double lowerLambda = (double)ConfigParameters["lowerLambda"];
double makespanWeight =
    (double)ConfigParameters["makespanWeight"]; // not used in the main SF optimization program
double noiseModelSigma =
    (double)ConfigParameters["noiseModelSigma"]; // not used in the main SF optimization program
const double AcceptSchedulError = (double)ConfigParameters["AcceptSchedulError"];
const double parallelFactor = (double)ConfigParameters["parallelFactor"];
const double relativeErrorTolerance = (double)ConfigParameters["relativeErrorTolerance"];

const double toleranceEliminator = (double)ConfigParameters["toleranceEliminator"];
const double upperLambda = (double)ConfigParameters["upperLambda"];
const double weightLogBarrier = (double)ConfigParameters["weightLogBarrier"];
const double zeroJacobianDetectTol = (double)ConfigParameters["zeroJacobianDetectTol"];
const double stepJacobianIteration = (double)ConfigParameters["stepJacobianIteration"];
const double NumericalPrecision = (double)ConfigParameters["NumericalPrecision"];

int printSchedule = (int)ConfigParameters["printSchedule"];
int bigJobGroupCheck = (int)ConfigParameters["bigJobGroupCheck"];
const int PrintOutput = (int)ConfigParameters["PrintOutput"];
const int ResetInnerWeightLoopMax = (int)ConfigParameters["ResetInnerWeightLoopMax"];
const int coreNumberAva = (int)ConfigParameters["coreNumberAva"];
int numericalJaobian =
    (int)ConfigParameters["numericalJaobian"]; // not used in the main SF optimization program
const int setUseFixedLambdaFactor = (int)ConfigParameters["setUseFixedLambdaFactor"];
const int setDiagonalDamping = (int)ConfigParameters["setDiagonalDamping"];
const int RandomDrawWeightMaxLoop = (int)ConfigParameters["RandomDrawWeightMaxLoop"];

// ********************* Under experiment or not useful
int enableFastSearch = (int)ConfigParameters["enableFastSearch"];
int enableSmallJobGroupCheck = (int)ConfigParameters["enableSmallJobGroupCheck"];
const int subJobGroupMaxSize = (int)ConfigParameters["subJobGroupMaxSize"];

// ********************* TO-FIX:
double sensorFusionTolerance = (double)ConfigParameters["sensorFusionTolerance"];
double freshTol = (double)ConfigParameters["freshTol"];

InitializeMethod initializeMethod = Transform_enum((int)ConfigParameters["initializeMethod"]);
double weightDDL_factor =
    (double)ConfigParameters["weightDDL_factor"]; // not used in the main SF optimization program
double weightDAG_factor =
    (double)ConfigParameters["weightDAG_factor"]; // not used in the main SF optimization program
const double weightInMpRTDA = (double)ConfigParameters["weightInMpRTDA"];
const double weightInMpSf = (double)ConfigParameters["weightInMpSf"];
const double weightInMpSfPunish = (double)ConfigParameters["weightInMpSfPunish"];
const double weightInMpRTDAPunish = (double)ConfigParameters["weightInMpRTDAPunish"];
const int64_t makeProgressTimeLimit = (int)ConfigParameters["makeProgressTimeLimit"];

const double RtdaWeight = (double)ConfigParameters["RtdaWeight"];
// const double RandomAccept = (double)ConfigParameters["RandomAccept"];

const double ResetRandomWeightThreshold = (double)ConfigParameters["ResetRandomWeightThreshold"];
double weightSF_factor =
    (double)ConfigParameters["weightSF_factor"]; // not used in the main SF optimization program
const int TotalMethodUnderComparison = (int)ConfigParameters["TotalMethodUnderComparison"];
const int processorAssignmentMode = (int)ConfigParameters["processorAssignmentMode"];
const int PrintInitial = (int)ConfigParameters["PrintInitial"];
int debugMode = (int)ConfigParameters["debugMode"]; // why can't it be const?
                                                    // int debugMode=0;
const int exactJacobian = (int)ConfigParameters["exactJacobian"];
const int optimizerType = (int)ConfigParameters["optimizerType"];
const int randomInitialize = (int)ConfigParameters["randomInitialize"];
const int SA_iteration = (int)ConfigParameters["SA_iteration"];
const int TaskSetType = (int)ConfigParameters["TaskSetType"];
const int temperatureSA = (int)ConfigParameters["temperatureSA"];
int tightEliminate = (int)ConfigParameters["tightEliminate"]; // not used in the main SF optimization program
int withAddedSensorFusionError =
    (int)ConfigParameters["withAddedSensorFusionError"]; // not used in the main SF optimization program
int whetherRandomNoiseModelSigma =
    (int)ConfigParameters["whetherRandomNoiseModelSigma"]; // not used in the main SF optimization program
int whether_ls = (int)ConfigParameters["whether_ls"];      // not used in the main SF optimization program

const int whether_shuffle_CE_chain = (int)ConfigParameters["whether_shuffle_CE_chain"];
const int NumCauseEffectChain = (int)ConfigParameters["NumCauseEffectChain"];

const int maxIterations = (int)ConfigParameters["maxIterations"];
const int saveGraph = (int)ConfigParameters["saveGraph"];
const int recordActionValue = (int)ConfigParameters["recordActionValue"];
// int recordRLFileCount = 0;
const int MaxEliminateDetectIterations = (int)ConfigParameters["MaxEliminateDetectIterations"];
const std::string priorityMode = (std::string)ConfigParameters["priorityMode"];
const std::string runMode = (std::string)ConfigParameters["runMode"];
const std::string testDataSetName = (std::string)ConfigParameters["testDataSetName"];
const std::string ReOrderProblem = (std::string)ConfigParameters["ReOrderProblem"];
const double punishmentInBarrier = (double)ConfigParameters["punishmentInBarrier"];
const double DataAgeThreshold = (double)ConfigParameters["DataAgeThreshold"];
const double ReactionTimeThreshold = (double)ConfigParameters["ReactionTimeThreshold"];
const double RoundingThreshold = (double)ConfigParameters["RoundingThreshold"];

const std::string verbosityLM = (std::string)ConfigParameters["verbosityLM"];
// code below is only used to show how to read vectors, but is not actually used in this project
// std::vector<int> readVector(std::string filename)
// {
//     cv::FileStorage fs;
//     fs.open(filename, cv::FileStorage::READ);
//     cv::Mat Tt;
//     fs["chain"] >> Tt;
//     int rows = Tt.rows;

//    std::vector<int> vec;
//     vec.reserve(rows);
//     for (int i = 0; i < rows; i++)
//     {
//         vec.push_back(Tt.at<int>(i, 0));
//     }
//     return vec;
// }
// std::vector<int> CA_CHAIN = readVector(GlobalVariablesDAGOpt::PROJECT_PATH +  "sources/parameters.yaml");

const int64_t kVerucchiTimeLimit = (int)ConfigParameters["kVerucchiTimeLimit"];
const double kVerucchiReactionCost = (double)ConfigParameters["kVerucchiReactionCost"];
const double kVerucchiMaxReaction = (double)ConfigParameters["kVerucchiMaxReaction"];
const double kVerucchiDataAgeCost = (double)ConfigParameters["kVerucchiDataAgeCost"];
const double kVerucchiMaxDataAge = (double)ConfigParameters["kVerucchiMaxDataAge"];
const double kVerucchiCoreCost = (double)ConfigParameters["kVerucchiCoreCost"];
const double kGlobalOptimizationTimeLimit = (double)ConfigParameters["kGlobalOptimizationTimeLimit"];

const int64_t kWangRtss21IcNlpTimeLimit = (int)ConfigParameters["kWangRtss21IcNlpTimeLimit"];

const double kCplexInequalityThreshold = (double)ConfigParameters["kCplexInequalityThreshold"];

const int doScheduleOptimization = (int)ConfigParameters["doScheduleOptimization"];
// probably figure out where this variable is used, clean related code
int considerSensorFusion =
    (int)ConfigParameters["considerSensorFusion"]; // previous code heavily depended on its value, but we must
                                                   // treat it as a const int in the future
const int doScheduleOptimizationOnlyOnce = (int)ConfigParameters["doScheduleOptimizationOnlyOnce"];
const int BatchTestOnlyOneMethod = (int)ConfigParameters["BatchTestOnlyOneMethod"];
const int selectInitialFromPool = (int)ConfigParameters["selectInitialFromPool"];
const int FastOptimization = (int)ConfigParameters["FastOptimization"];
const int EnableHardJobORder = (int)ConfigParameters["EnableHardJobORder"];
const int MakeProgressOnlyMax = (int)ConfigParameters["MakeProgressOnlyMax"];
const double LPTolerance = (double)ConfigParameters["LPTolerance"];
} // namespace GlobalVariablesDAGOpt