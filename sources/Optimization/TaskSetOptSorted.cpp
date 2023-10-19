
#include "sources/Optimization/TaskSetOptSorted.h"

namespace DAG_SPACE {
bool AllChainsNotEmpty(const std::vector<std::vector<int>>& chains) {
  for (const std::vector<int>& chain : chains) {
    if (chain.size() == 0) return false;
  }
  return true;
}

void TaskSetOptSorted::PrintFeasibleChainsRecord() const {
  std::vector<Edge> edges = GetAllEdges();
  int count = 0;
  for (const auto& feasible_chain_man : feasible_chains_.chain_man_vec_) {
    std::cout << "\n****************************\n";
    std::cout << "Feasible chain index: " << count++ << "\n";
    const ChainsPermutation& chains_perm = feasible_chain_man.feasible_chain_;
    PrintSinglePermIndex(chains_perm, edges);
  }

  std::cout << "\n****************************\n";
}
}  // namespace DAG_SPACE