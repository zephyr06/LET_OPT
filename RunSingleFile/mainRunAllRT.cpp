#include "sources/BatchOptimize/batchOptimize.h"

using namespace DAG_SPACE;

int main(int argc, char *argv[]) {
  DAG_Model dag_tasks = ReadDAG_Tasks(
      GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_PaperExample.csv",
      "RM", 1);
  ScheduleResult res;

  // case InitialMethod:
  res = PerformStandardLETAnalysis<ObjReactionTime>(dag_tasks);
  std::cout << Color::blue << "Standard LET, max Reaction Time: " << res.obj_
            << Color::def << "\n";

  // case TOM_BF:
  res = PerformTOM_OPT_BF<ObjReactionTime>(dag_tasks);
  std::cout << Color::blue
            << "eLET_brute_force, max Reaction Time: " << res.obj_ << Color::def
            << "\n";

  // case TOM_WSkip:
  res = PerformTOM_OPT_EnumW_Skip<ObjReactionTime>(dag_tasks);
  std::cout << Color::blue
            << "eLET_SkipInfeasible, max Reaction Time: " << res.obj_
            << Color::def << "\n";

  // case TOM_Sort:
  res = PerformTOM_OPT_Sort<ObjReactionTime>(dag_tasks);
  std::cout << Color::blue << "eLET_Sort, max Reaction Time: " << res.obj_
            << Color::def << "\n";

  // case ImplicitCommunication:
  res = PerformImplicitCommuAnalysis<ObjReactionTime>(dag_tasks);
  std::cout << Color::blue
            << "ImplicitCommunication, max Reaction Time: " << res.obj_
            << Color::def << "\n";
}