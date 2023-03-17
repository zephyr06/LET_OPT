#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/batchOptimize.h"
using namespace DAG_SPACE;

TEST(average_performance, v1) {
    BeginTimer("main");

    std::string dataSetFolder =
        GlobalVariablesDAGOpt::PROJECT_PATH + "TaskData/dagTasksPerfTest/";
    ClearResultFiles(dataSetFolder);
    std::vector<DAG_SPACE::BaselineMethods> baselineMethods = {
        DAG_SPACE::InitialMethod, DAG_SPACE::TOM};
    auto res = DAG_SPACE::BatchOptimizeOrder<DAG_SPACE::ObjReactionTime>(
        baselineMethods, 0, 2);
    EXPECT_THAT(res[BaselineMethods::TOM].performance, testing::Le(0.53));

    EXPECT_DOUBLE_EQ(1.0, res[BaselineMethods::TOM].schedulableRatio);
    EndTimer("main");
    PrintTimer();
}

int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}