#include "sources/Optimization/GraphOfChains.h"
namespace DAG_SPACE {

GraphOfChains::GraphOfChains(const std::vector<std::vector<int>>& chains)
    : chains_(chains) {
    edge_records_.reserve(1e2);      // this size is an upper bound
    edge_vec_ordered_.reserve(1e2);  // this size is an upper bound
    prev_tasks_.reserve(2e2);        // this size is an upper bound

    for (const auto& chain : chains) {
        for (unsigned int i = 0; i < chain.size() - 1; i++) {
            int from_id = chain[i];
            int to_id = chain[i + 1];
            Edge edge_curr(from_id, to_id);
            if (edge_records_.find(edge_curr) == edge_records_.end()) {
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
    }
}
}  // namespace DAG_SPACE