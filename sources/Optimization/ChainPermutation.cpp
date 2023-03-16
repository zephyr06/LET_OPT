

#include "sources/Optimization/ChainPermutation.h"

#include "sources/Utils/Interval.h"

namespace DAG_SPACE {

// symbols in this function are explained as follows, and we want d_0's range
// perm_prev: o_1 + x < d_0 <= o_1 + y
Interval GetDeadlineRange(const VariableRange& variable_od_range,
                          const SinglePairPermutation& perm_prev) {
    int o1_index = perm_prev.GetNextTaskId();
    int low_bound_o1 = variable_od_range.lower_bound.at(o1_index).offset;
    int d0_lb_from_o1 = low_bound_o1 + perm_prev.inequality_.lower_bound_;
    int upp_bound_o1 = variable_od_range.upper_bound.at(o1_index).offset;
    int d0_ub_from_o1 = upp_bound_o1 + perm_prev.inequality_.upper_bound_;
    return Interval(d0_lb_from_o1, d0_ub_from_o1 - d0_lb_from_o1);
}

// symbols in this function are explained as follows, and we want o_1's range
// perm_prev: o_1 + x < d_0 <= o_1 + y
Interval GetOffsetRange(const VariableRange& variable_od_range,
                        const SinglePairPermutation& perm_prev) {
    int d0_index = perm_prev.GetPrevTaskId();
    int low_bound_d0 = variable_od_range.lower_bound.at(d0_index).deadline;
    int o1_lb_from_d0 = low_bound_d0 - perm_prev.inequality_.upper_bound_;
    int upp_bound_d0 = variable_od_range.upper_bound.at(d0_index).deadline;
    int o1_ub_from_d0 = upp_bound_d0 - perm_prev.inequality_.lower_bound_;
    return Interval(o1_lb_from_d0, o1_ub_from_d0 - o1_lb_from_d0);
}

// we analyze the lower bound and upper bound of d_0
bool IsTwoPermConflicted_SameSource(const VariableRange& variable_od_range,
                                    const SinglePairPermutation& perm_prev,
                                    const SinglePairPermutation& perm_curr) {
    Interval deadline_range_from_prev =
        GetDeadlineRange(variable_od_range, perm_prev);
    Interval deadline_range_from_curr =
        GetDeadlineRange(variable_od_range, perm_curr);
    return Overlap(deadline_range_from_prev, deadline_range_from_curr) > 0;
}

bool IsTwoPermConflicted_SameSink(const VariableRange& variable_od_range,
                                  const SinglePairPermutation& perm_prev,
                                  const SinglePairPermutation& perm_curr) {
    Interval offset_range_from_prev =
        GetOffsetRange(variable_od_range, perm_prev);
    Interval offset_range_from_curr =
        GetOffsetRange(variable_od_range, perm_curr);
    return Overlap(offset_range_from_prev, offset_range_from_curr) > 0;
}

bool IsTwoPermConflicted_SerialConnect(const VariableRange& variable_od_range,
                                       const SinglePairPermutation& perm_prev,
                                       const SinglePairPermutation& perm_curr) {
    if (perm_prev.GetNextTaskId() == perm_curr.GetPrevTaskId()) {
        int deadline_prev_min =
            variable_od_range.lower_bound.at(perm_prev.GetPrevTaskId())
                .deadline;  // this is rta
        int offset_curr_min =
            deadline_prev_min - perm_prev.inequality_.upper_bound_;

        int offset_next_max =
            variable_od_range.upper_bound.at(perm_curr.GetNextTaskId()).offset;
        int deadline_curr_max_from_ineq =
            offset_next_max + perm_curr.inequality_.upper_bound_;

        int deadline_curr_max =
            variable_od_range.upper_bound.at(perm_curr.GetPrevTaskId())
                .deadline;
        deadline_curr_max =
            std::min(deadline_curr_max, deadline_curr_max_from_ineq);

        int rta_curr =
            variable_od_range.lower_bound.at(perm_curr.GetPrevTaskId())
                .deadline;
        int offset_curr_max = deadline_curr_max - rta_curr;
        if (offset_curr_max < offset_curr_min) {
            return false;
        }
    }
    return true;
}

bool ChainPermutation::IsPermConflicted_CheckAllWithSameSource(
    const VariableRange& variable_od_range,
    const SinglePairPermutation& perm_curr,
    const GraphOfChains& graph_of_all_ca_chains) const {
    int source_task_id = perm_curr.GetPrevTaskId();
    int sink_task_id = perm_curr.GetNextTaskId();

    const std::vector<int>& source_next_task_ids =
        graph_of_all_ca_chains.next_tasks_.at(source_task_id);
    for (int next_id : source_next_task_ids) {
        if (next_id == sink_task_id) continue;
        Edge edge_ite(source_task_id, next_id);
        if (permutation_chain_map_.find(edge_ite) ==
            permutation_chain_map_
                .end())  // this edge has not been added to the
                         // chain yet, therefore no conflictions
            continue;
        const SinglePairPermutation& perm_ite =
            permutation_chain_map_.at(edge_ite);
        if (!IsTwoPermConflicted_SameSource(variable_od_range, perm_ite,
                                            perm_curr))
            return false;
    }
    return true;
}

bool ChainPermutation::IsPermConflicted_CheckAllWithSameSink(
    const VariableRange& variable_od_range,
    const SinglePairPermutation& perm_curr,
    const GraphOfChains& graph_of_all_ca_chains) const {
    int source_task_id = perm_curr.GetPrevTaskId();
    int sink_task_id = perm_curr.GetNextTaskId();
    const std::vector<int>& source_prev_task_ids =
        graph_of_all_ca_chains.prev_tasks_.at(sink_task_id);
    for (int prev_id : source_prev_task_ids) {
        if (prev_id == source_task_id) continue;
        Edge edge_ite(prev_id, sink_task_id);
        if (permutation_chain_map_.find(edge_ite) ==
            permutation_chain_map_.end())
            continue;  // this edge has not been added to the
                       // chain yet, therefore no conflictions
        const SinglePairPermutation& perm_ite =
            permutation_chain_map_.at(edge_ite);
        if (!IsTwoPermConflicted_SameSink(variable_od_range, perm_ite,
                                          perm_curr))
            return false;
    }
    return true;
}

bool ChainPermutation::IsPermConflicted_CheckAllSerialConnect(
    const VariableRange& variable_od_range,
    const SinglePairPermutation& perm_curr,
    const GraphOfChains& graph_of_all_ca_chains) const {
    // check serial case: perm_ite -> perm_curr
    int source_task_id = perm_curr.GetPrevTaskId();
    std::vector<int> source_prev_task_ids =
        graph_of_all_ca_chains.GetPrevTasks(source_task_id);
    for (int prev_id : source_prev_task_ids) {
        Edge edge_ite(prev_id, source_task_id);
        if (permutation_chain_map_.find(edge_ite) ==
            permutation_chain_map_.end())
            continue;  // this edge has not been added to the
                       // chain yet, therefore no conflictions
        const SinglePairPermutation& perm_ite =
            permutation_chain_map_.at(edge_ite);
        if (!IsTwoPermConflicted_SerialConnect(variable_od_range, perm_ite,
                                               perm_curr))
            return false;
    }

    // check serial case: perm_curr -> perm_ite
    int sink_task_id = perm_curr.GetNextTaskId();
    std::vector<int> source_next_task_ids =
        graph_of_all_ca_chains.GetNextTasks(sink_task_id);
    for (int next_id : source_next_task_ids) {
        Edge edge_ite(sink_task_id, next_id);
        if (permutation_chain_map_.find(edge_ite) ==
            permutation_chain_map_
                .end())  // this edge has not been added to the
                         // chain yet, therefore no conflictions
            continue;
        const SinglePairPermutation& perm_ite =
            permutation_chain_map_.at(edge_ite);
        if (!IsTwoPermConflicted_SerialConnect(variable_od_range, perm_curr,
                                               perm_ite))
            return false;
    }

    return true;
}

bool ChainPermutation::IsValid(
    const VariableRange& variable_od_range,
    const SinglePairPermutation& perm_curr,
    const GraphOfChains& graph_of_all_ca_chains) const {
    int perm_single_chain_size = permutation_chain_map_.size();
    if (perm_single_chain_size > 0) {
        if (!IsPermConflicted_CheckAllWithSameSource(
                variable_od_range, perm_curr, graph_of_all_ca_chains))
            return false;

        if (!IsPermConflicted_CheckAllWithSameSink(variable_od_range, perm_curr,
                                                   graph_of_all_ca_chains))
            return false;

        // TODO: this function has not been fully tested
        if (!IsPermConflicted_CheckAllSerialConnect(
                variable_od_range, perm_curr, graph_of_all_ca_chains))
            return false;
    }
    return true;
}

// int ChainPermutation::GetTaskId(size_t i) const {
//     if (i < size())
//         return permutation_chain_[i].inequality_.task_prev_id_;
//     else if (i == size()) {
//         return permutation_chain_[size() - 1].inequality_.task_next_id_;
//     } else
//         CoutError("Out-or-range error in ChainPermutation!");
//     return -1;
// }

}  // namespace DAG_SPACE