#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/BatchOptimize/batchOptimize.h"
using namespace DAG_SPACE;

TEST(average_performance, v1) {
  BeginTimer("main");

  std::vector<DAG_SPACE::BASELINEMETHODS> baselineMethods = {
      DAG_SPACE::InitialMethod, DAG_SPACE::TOM};
  ClearResultFiles(GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/PerfTest");
  BatchSettings batch_test_settings(5, 0, 300, "TaskData/PerfTest/");
  auto res = DAG_SPACE::BatchOptimizeOrder<DAG_SPACE::ObjReactionTime>(
      baselineMethods, batch_test_settings);
  EXPECT_THAT(res[BASELINEMETHODS::TOM].performance, testing::Le(0.478));

  EXPECT_DOUBLE_EQ(1.0, res[BASELINEMETHODS::TOM].schedulableRatio);
  EndTimer("main");
  PrintTimer();
}

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}