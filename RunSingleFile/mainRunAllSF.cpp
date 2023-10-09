#include "sources/BatchOptimize/batchOptimize.h"

using namespace DAG_SPACE;

int main(int argc, char *argv[]) {
  DAG_Model dag_tasks =
      ReadDAG_Tasks(GlobalVariablesDAGOpt::PROJECT_PATH +
                        "TaskData/test_PaperExample2Chain_v2.csv",
                    "RM", 1);
  dag_tasks.chains_ = GetChainsForSF(dag_tasks);
  ScheduleResult res;

  // case InitialMethod:
  res = PerformStandardLETAnalysis<ObjSensorFusion>(dag_tasks);
  std::cout << Color::blue << "Standard LET, max Sensor Fusion: " << res.obj_
            << Color::def << "\n";
  std::cout << Color::blue
            << "Standard LET, jitter Sensor Fusion: " << res.jitter_
            << Color::def << "\n";

  // case TOM_BF:
  res = PerformTOM_OPT_BF<ObjSensorFusion>(dag_tasks);
  std::cout << Color::blue
            << "eLET_brute_force, max Sensor Fusion: " << res.obj_ << Color::def
            << "\n";
  std::cout << Color::blue
            << "eLET_brute_force LET, jitter Sensor Fusion: " << res.jitter_
            << Color::def << "\n";

  // case TOM_WSkip:
  res = PerformTOM_OPT_EnumW_Skip<ObjSensorFusion>(dag_tasks);
  std::cout << Color::blue
            << "eLET_SkipInfeasible, max Sensor Fusion: " << res.obj_
            << Color::def << "\n";
  std::cout << Color::blue
            << "eLET_SkipInfeasible LET, jitter Sensor Fusion: " << res.jitter_
            << Color::def << "\n";

  // case ImplicitCommunication:
  res = PerformImplicitCommuAnalysis<ObjSensorFusion>(dag_tasks);
  std::cout << Color::blue
            << "\nImplicitCommunication, max Sensor Fusion: " << res.obj_
            << Color::def << "\n";
  std::cout << Color::blue
            << "ImplicitCommunication LET, jitter Sensor Fusion: "
            << res.jitter_ << Color::def << "\n";
}