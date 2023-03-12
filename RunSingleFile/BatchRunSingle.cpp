#include "sources/batchOptimize.h"

int main() {
    std::vector<DAG_SPACE::BaselineMethods> baselineMethods = {
        DAG_SPACE::InitialMethod, DAG_SPACE::TOM};  // , DAG_SPACE::TOM_FAST
    DAG_SPACE::BatchOptimizeOrder<DAG_SPACE::ObjReactionTime>(baselineMethods);
}