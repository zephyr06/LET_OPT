#include "sources/Optimization/GraphOfChains.h"
namespace DAG_SPACE {

int GetLongestChainLength(const std::vector<std::vector<int>>& chains) {
  int max_length = 0;
  for (const auto& chain : chains)
    max_length = std::max(max_length, static_cast<int>(chain.size()));
  return max_length;
}

GraphOfChains::GraphOfChains(const std::vector<std::vector<int>>& chains)
    : chains_(chains) {
  edge_records_.reserve(1e2);      // this size is an upper bound
  edge_vec_ordered_.reserve(1e2);  // this size is an upper bound
  prev_tasks_.reserve(2e2);        // this size is an upper bound

  uint chain_num = chains.size();
  int edge_index = 0;
  int max_chain_length = GetLongestChainLength(chains);
  while (edge_index < max_chain_length) {
    bool find_update = false;
    for (const auto& chain : chains) {
      if (edge_index >= chain.size() - 1) continue;
      int from_id = chain[edge_index];
      int to_id = chain[edge_index + 1];
      Edge edge_curr(from_id, to_id);
      if (edge_records_.find(edge_curr) == edge_records_.end()) {
        find_update = true;
        edge_records_.insert(edge_curr);
        edge_vec_ordered_.push_back(edge_curr);
        auto itr = prev_tasks_.find(to_id);
        if (itr == prev_tasks_.end())
          prev_tasks_[to_id] = {from_id};
        else
          prev_tasks_[to_id].push_back(from_id);
        itr = next_tasks_.find(from_id);
        if (itr == next_tasks_.end())
          next_tasks_[from_id] = {to_id};
        else
          next_tasks_[from_id].push_back(to_id);
      }
    }
    edge_index++;
  }

  // for (const auto& chain : chains) {
  //     for (unsigned int i = 0; i < chain.size() - 1; i++) {
  //         int from_id = chain[i];
  //         int to_id = chain[i + 1];
  //         Edge edge_curr(from_id, to_id);
  //         if (edge_records_.find(edge_curr) == edge_records_.end()) {
  //             edge_records_.insert(edge_curr);
  //             edge_vec_ordered_.push_back(edge_curr);
  //             auto itr = prev_tasks_.find(to_id);
  //             if (itr == prev_tasks_.end())
  //                 prev_tasks_[to_id] = {from_id};
  //             else
  //                 prev_tasks_[to_id].push_back(from_id);
  //             itr = next_tasks_.find(from_id);
  //             if (itr == next_tasks_.end())
  //                 next_tasks_[from_id] = {to_id};
  //             else
  //                 next_tasks_[from_id].push_back(to_id);
  //         }
  //     }
  // }
}

std::vector<int> GraphOfChains::GetPrevTasks(int task_id) const {
  auto itr = prev_tasks_.find(task_id);
  if (itr == prev_tasks_.end())
    // CoutError("Not found task_id in GraphOfChains!");
    return {};
  return prev_tasks_.at(task_id);
}
std::vector<int> GraphOfChains::GetNextTasks(int task_id) const {
  auto itr = next_tasks_.find(task_id);
  if (itr == next_tasks_.end())
    // CoutError("Not found task_id in GraphOfChains!");
    return {};
  return next_tasks_.at(task_id);
}
}  // namespace DAG_SPACE