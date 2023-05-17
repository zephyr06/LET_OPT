#include "sources/BatchOptimize/batchOptimize.h"

using namespace DAG_SPACE;

int main(int argc, char *argv[]) {
  // DAG_Model dag_tasks = ReadDAG_Tasks(
  //     GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/test_PaperExample.csv",
  //     "RM", 1);
  DAG_Model dag_tasks =
      ReadDAG_Tasks(GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/" +
                        GlobalVariablesDAGOpt::testDataSetName + ".csv",
                    "RM", 1);
  ScheduleResult res;

  // case InitialMethod:
  res = PerformStandardLETAnalysis<ObjDataAge>(dag_tasks);
  std::cout << Color::blue << "Standard LET, max Data Age: " << res.obj_
            << Color::def << "\n";

  // case TOM_BF:
  res = PerformTOM_OPT_BF<ObjDataAge>(dag_tasks);
  std::cout << Color::blue << "eLET_brute_force, max Data Age: " << res.obj_
            << Color::def << "\n";

  // case TOM_WSkip:
  res = PerformTOM_OPT_EnumW_Skip<ObjDataAge>(dag_tasks);
  std::cout << Color::blue << "eLET_SkipInfeasible, max Data Age: " << res.obj_
            << Color::def << "\n";

  // case TOM_Sort:
  res = PerformTOM_OPT_Sort<ObjDataAge>(dag_tasks);
  std::cout << Color::blue << "eLET_Sort, max Data Age: " << res.obj_
            << Color::def << "\n";

  // case Martinez18:
  res = PerformOPT_Martinez18_DA(dag_tasks);
  std::cout << Color::blue << "Martinez18, max Data Age: " << res.obj_
            << Color::def << "\n";

  // case TOM_Sort_Offset:
  res = PerformTOM_OPTOffset_Sort(dag_tasks);
  std::cout << Color::blue << "eLET_Sort_Offset, max Data Age: " << res.obj_
            << Color::def << "\n";

  // case ImplicitCommunication:
  res = PerformImplicitCommuAnalysis<ObjDataAge>(dag_tasks);
  std::cout << Color::blue
            << "ImplicitCommunication, max Data Age: " << res.obj_ << Color::def
            << "\n";
}