
#pragma once
#include <string>

namespace DAG_SPACE {
inline bool IfRT_Trait(const std::string &trait) {
  return trait == "ReactionTime" || trait == "ReactionTimeApprox";
}

inline bool IfDA_Trait(const std::string &trait) {
  return trait == "DataAge" || trait == "DataAgeApprox";
}

}  // namespace DAG_SPACE