#pragma once
#include <Eigen/Core>
#include <iostream>
#include <vector>
// #include <Eigen/Geometry>
#include "sources/Utils/testMy.h"
#include <opencv2/core/core.hpp>
// using namespace std;
// All the global variables should be const

enum InitializeMethod { IndexMode, FixedRelativeStart, RM, RM_DAG, Custom_DAG, ListScheduling };
inline InitializeMethod Transform_enum(int a) {
  if (a == 0)
    return IndexMode;
  else if (a == 1)
    return FixedRelativeStart;
  else if (a == 2)
    return RM;
  else if (a == 3)
    return RM_DAG;
  else if (a == 4)
    return Custom_DAG;
  else if (a == 5)
    return ListScheduling;
  else {
    CoutError("Not recognized enum InitializeMethod\n");
    return IndexMode;
  }
}

namespace GlobalVariablesDAGOpt {
extern const std::string PROJECT_PATH;
// const std::string PROJECT_PATH = "/home/dong/workspace/DAG_NLP/";
// const std::string PROJECT_PATH = "/home/zephyr/Programming/batch_test_DAG_NLP/VerucchiOnly/N3_10/";
//  std::string path = GlobalVariablesDAGOpt::PROJECT_PATH +  "sources/parameters.yaml";
extern cv::FileStorage ConfigParameters;

extern const double barrierBase;
extern const double coolingRateSA;
extern const double deltaInitialDogleg;
extern double deltaOptimizer; // not used in the main SF optimization program
extern const int timeScaleFactor;

extern const double initialLambda;
extern const double lowerLambda;
extern double makespanWeight;  // not used in the main SF optimization program
extern double noiseModelSigma; // not used in the main SF optimization program
extern const double AcceptSchedulError;
extern const double parallelFactor;
extern const double relativeErrorTolerance;

extern const double toleranceEliminator;
extern const double upperLambda;
extern const double weightLogBarrier;
extern const double zeroJacobianDetectTol;
extern const double stepJacobianIteration;

extern int printSchedule;
extern int bigJobGroupCheck;
extern const int PrintOutput;
extern const int ResetInnerWeightLoopMax;
extern const int coreNumberAva;
extern int numericalJaobian; // not used in the main SF optimization program
extern const int setUseFixedLambdaFactor;
extern const int setDiagonalDamping;
extern const int RandomDrawWeightMaxLoop;

// ********************* Under experiment or not useful
extern int enableFastSearch;
extern int enableSmallJobGroupCheck;
extern const int RepeatExecution;

extern const int subJobGroupMaxSize;

// ********************* TO-FIX:
extern double sensorFusionTolerance;
extern double freshTol;

extern InitializeMethod initializeMethod;
extern double weightDDL_factor; // not used in the main SF optimization program
extern double weightDAG_factor; // not used in the main SF optimization program
extern const double weightInMpRTDA;
extern const double weightInMpSf;
extern const double weightInMpSfPunish;
extern const double weightInMpRTDAPunish;
extern const int64_t makeProgressTimeLimit;
extern const double NumericalPrecision;

extern const double RtdaWeight;
extern const double ResetRandomWeightThreshold;
extern double weightSF_factor; // not used in the main SF optimization program
extern const int TotalMethodUnderComparison;
extern const int processorAssignmentMode;
extern const int PrintInitial;
extern int debugMode; // why can't it be const?
// int debugMode=0;
extern const int exactJacobian;
extern const int optimizerType;
extern const int randomInitialize;
extern const int SA_iteration;
extern const int TaskSetType;
extern const int temperatureSA;
extern int tightEliminate;               // not used in the main SF optimization program
extern int withAddedSensorFusionError;   // not used in the main SF optimization program
extern int whetherRandomNoiseModelSigma; // not used in the main SF optimization program
extern int whether_ls;                   // not used in the main SF optimization program

extern const int whether_shuffle_CE_chain;
extern const int NumCauseEffectChain;

extern const int maxIterations;
extern const int saveGraph;
extern const int recordActionValue;
extern const int MaxEliminateDetectIterations;
extern const std::string priorityMode;
extern const std::string runMode;
extern const std::string testDataSetName;
extern const std::string ReOrderProblem;
extern const double punishmentInBarrier;
extern const double DataAgeThreshold;
extern const double ReactionTimeThreshold;
extern const double RoundingThreshold;

extern const std::string verbosityLM;
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

extern const int64_t kVerucchiTimeLimit;
extern const double kVerucchiReactionCost;
extern const double kVerucchiMaxReaction;
extern const double kVerucchiDataAgeCost;
extern const double kVerucchiMaxDataAge;
extern const double kVerucchiCoreCost;
extern const double kGlobalOptimizationTimeLimit;

extern const int64_t kWangRtss21IcNlpTimeLimit;

extern const double kCplexInequalityThreshold;

extern const int selectInitialFromPool;
extern const int doScheduleOptimization;
extern int considerSensorFusion; // previous code heavily depended on its value, but we must treat it as a
                                 // const int in the future
extern const int doScheduleOptimizationOnlyOnce;
extern const int BatchTestOnlyOneMethod;
extern const int FastOptimization;
extern const int EnableHardJobORder;
extern const double LPTolerance;
extern const int MakeProgressOnlyMax;

} // namespace GlobalVariablesDAGOpt