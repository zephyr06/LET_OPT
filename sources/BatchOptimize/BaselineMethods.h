#pragma once
#include <string>
#include <vector>

namespace DAG_SPACE {
// !!!if adding more methods, need to update BaselineMethodNames below !!!
// Note: *_Approx should not be considered as an independent method?
enum BASELINEMETHODS {
  InitialMethod,    // 0
  TOM_BF,           // 1
  TOM_WSkip,        // 2
  TOM_Sort,         // 3
  Martinez18,       // 4
  TOM_Sort_Offset,  // 5
  TOM_Sort_Bound,
  TOM_Sort_ImpBound,
  ImplicitCommunication,
  Bardatsch16,
  Maia23,
  TOM_Sort_Maia23,
  TOM_WSkip_Maia23
};
const std::vector<std::string> BaselineMethodNames = {
    "InitialMethod",
    "TOM_BF",
    "TOM_WSkip",
    "TOM_Sort",
    "Martinez18",
    "TOM_Sort_Offset",
    "TOM_Sort_Bound",
    "TOM_Sort_ImpBound",
    "ImplicitCommunication",
    "Bardatsch16",
    "Maia23",
    "TOM_Sort_Maia23",
    "TOM_WSkip_Maia23"};  // "TOM_BF_Approx",
// "TOM_WSkip_Approx", "TOM_Sort_Approx",  "SA"

}  // namespace DAG_SPACE