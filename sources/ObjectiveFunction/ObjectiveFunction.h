#pragma once
#include "sources/ObjectiveFunction/ObjDataAge.h"
#include "sources/ObjectiveFunction/ObjDataAgeApprox.h"
#include "sources/ObjectiveFunction/ObjReactionTime.h"
#include "sources/ObjectiveFunction/ObjReactionTimeApprox.h"
#include "sources/ObjectiveFunction/ObjSensorFusion.h"

inline bool IfRT_Trait(const std::string &trait) {
  return trait == "ReactionTime" || trait == "ReactionTimeApprox";
}

inline bool IfDA_Trait(const std::string &trait) {
  return trait == "DataAge" || trait == "DataAgeApprox";
}
