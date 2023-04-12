#pragma once
#include <string>
#include <vector>

namespace DAG_SPACE {
// !!!if adding more methods, need to update BaselineMethodNames below !!!
// Note: *_Approx should not be considered as an independent method?
enum BASELINEMETHODS {
    InitialMethod,  // 0
    TOM_BF,         // 1
    TOM_WSkip,      // 2
    TOM_Sort,       // 3
    // TOM_BF_Approx,     // 4
    // TOM_WSkip_Approx,  // 5
    // TOM_Sort_Approx,   // 6
    SA  // 7
};
const std::vector<std::string> BaselineMethodNames = {
    "InitialMethod", "TOM_BF",           "TOM_WSkip",       "TOM_Sort",
    "TOM_BF_Approx", "TOM_WSkip_Approx", "TOM_Sort_Approx", "SA"};

}  // namespace DAG_SPACE