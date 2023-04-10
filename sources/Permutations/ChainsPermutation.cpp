

#include "sources/Permutations/ChainsPermutation.h"

#include "sources/Utils/Interval.h"

namespace DAG_SPACE {

// symbols in this function are explained as follows, and we want d_0's range
// perm_prev: o_1 + x < d_0 <= o_1 + y
Interval GetDeadlineRange_RTPerm(const VariableRange& variable_od_range,
                                 const PermutationInequality& perm_ineq) {
  int o1_index = perm_ineq.task_next_id_;
  int low_bound_o1 = variable_od_range.lower_bound.at(o1_index).offset;
  int d0_lb_from_o1 = low_bound_o1 + perm_ineq.lower_bound_;
  int upp_bound_o1 = variable_od_range.upper_bound.at(o1_index).offset;
  int d0_ub_from_o1 = upp_bound_o1 + perm_ineq.upper_bound_;
  // return Interval(d0_lb_from_o1, d0_ub_from_o1 - d0_lb_from_o1);
  int d0_task_index = perm_ineq.task_prev_id_;
  Interval deadline_range(0, 0);
  deadline_range.start = std::max(
      variable_od_range.lower_bound.at(d0_task_index).deadline, d0_lb_from_o1);
  deadline_range.length =
      std::min(d0_ub_from_o1,
               variable_od_range.upper_bound.at(d0_task_index).deadline) -
      deadline_range.start;
  return deadline_range;
}

// symbols in this function are explained as follows, and we want o_1's range
// perm_prev: o_1 + x < d_0 <= o_1 + y
Interval GetOffsetRange_RTPerm(const VariableRange& variable_od_range,
                               const PermutationInequality& perm_ineq) {
  int d0_index = perm_ineq.task_prev_id_;
  int low_bound_d0 = variable_od_range.lower_bound.at(d0_index).deadline;
  int o1_lb_from_d0 = low_bound_d0 - perm_ineq.upper_bound_;
  int upp_bound_d0 = variable_od_range.upper_bound.at(d0_index).deadline;
  int o1_ub_from_d0 = upp_bound_d0 - perm_ineq.lower_bound_;
  // return Interval(o1_lb_from_d0, o1_ub_from_d0 - o1_lb_from_d0);
  Interval offset_range(0, 0);
  int o1_task_index = perm_ineq.task_next_id_;
  offset_range.start = std::max(
      variable_od_range.lower_bound.at(o1_task_index).offset, o1_lb_from_d0);
  offset_range.length =
      std::min(o1_ub_from_d0,
               variable_od_range.upper_bound.at(o1_task_index).offset) -
      offset_range.start;
  return offset_range;
}

// symbols in this function are explained as follows, and we want d_0's range
// perm_prev: d_0 + x <= o_1 < d_0 + y
Interval GetDeadlineRange_DAPerm(const VariableRange& variable_od_range,
                                 const PermutationInequality& perm_ineq) {
  int o1_task_index = perm_ineq.task_next_id_;
  int low_bound_o1 = variable_od_range.lower_bound.at(o1_task_index).offset;
  int d0_lb_from_o1 = low_bound_o1 - (perm_ineq.upper_bound_ - 1);
  int upp_bound_o1 = variable_od_range.upper_bound.at(o1_task_index).offset;
  int d0_ub_from_o1 = upp_bound_o1 - perm_ineq.lower_bound_;
  int d0_task_index = perm_ineq.task_prev_id_;
  Interval deadline_range(0, 0);
  deadline_range.start = std::max(
      variable_od_range.lower_bound.at(d0_task_index).deadline, d0_lb_from_o1);
  deadline_range.length =
      std::min(d0_ub_from_o1,
               variable_od_range.upper_bound.at(d0_task_index).deadline) -
      deadline_range.start;
  return deadline_range;
}

// symbols in this function are explained as follows, and we want o_1's range
// perm_prev: d_0 + x <= o_1 < d_0 + y
Interval GetOffsetRange_DAPerm(const VariableRange& variable_od_range,
                               const PermutationInequality& perm_ineq) {
  int d0_task_index = perm_ineq.task_prev_id_;
  int low_bound_d0 = variable_od_range.lower_bound.at(d0_task_index).deadline;
  int o1_lb_from_d0 = low_bound_d0 + perm_ineq.lower_bound_;
  int upp_bound_d0 = variable_od_range.upper_bound.at(d0_task_index).deadline;
  int o1_ub_from_d0 = upp_bound_d0 + perm_ineq.upper_bound_;
  Interval offset_range(0, 0);
  int o1_task_index = perm_ineq.task_next_id_;
  offset_range.start = std::max(
      variable_od_range.lower_bound.at(o1_task_index).offset, o1_lb_from_d0);
  offset_range.length =
      std::min(o1_ub_from_d0,
               variable_od_range.upper_bound.at(o1_task_index).offset) -
      offset_range.start;
  return offset_range;
}

// we analyze the lower bound and upper bound of d_0
// return true if conflicted, false if safe
bool IsTwoPermConflicted_SameSource(const VariableRange& variable_od_range,
                                    const SinglePairPermutation& perm_prev,
                                    const SinglePairPermutation& perm_curr) {
  Interval deadline_range_from_prev;
  Interval deadline_range_from_curr;
  if (perm_prev.type_trait_ == "ReactionTimeApprox" ||
      perm_prev.type_trait_ == "ReactionTime") {
    deadline_range_from_prev =
        GetDeadlineRange_RTPerm(variable_od_range, perm_prev.inequality_);
    deadline_range_from_curr =
        GetDeadlineRange_RTPerm(variable_od_range, perm_curr.inequality_);
  } else if (perm_prev.type_trait_ == "DataAgeApprox" ||
             perm_prev.type_trait_ == "DataAge") {
    deadline_range_from_prev =
        GetDeadlineRange_DAPerm(variable_od_range, perm_prev.inequality_);
    deadline_range_from_curr =
        GetDeadlineRange_DAPerm(variable_od_range, perm_curr.inequality_);
  } else
    CoutError("Unrecognized type_trait_ in IsTwoPermConflicted_SameSource!");
  return Overlap(deadline_range_from_prev, deadline_range_from_curr) == 0 &&
         (!(WhetherSerialAdjacent(deadline_range_from_prev,
                                  deadline_range_from_curr)));
}

// return true if conflicted, false if safe
bool IsTwoPermConflicted_SameSink(const VariableRange& variable_od_range,
                                  const SinglePairPermutation& perm_prev,
                                  const SinglePairPermutation& perm_curr) {
  Interval offset_range_from_prev, offset_range_from_curr;
  if (perm_prev.type_trait_ == "ReactionTimeApprox" ||
      perm_prev.type_trait_ == "ReactionTime") {
    offset_range_from_prev =
        GetOffsetRange_RTPerm(variable_od_range, perm_prev.inequality_);
    offset_range_from_curr =
        GetOffsetRange_RTPerm(variable_od_range, perm_curr.inequality_);
  } else if (perm_prev.type_trait_ == "DataAgeApprox" ||
             perm_prev.type_trait_ == "DataAge") {
    offset_range_from_prev =
        GetOffsetRange_DAPerm(variable_od_range, perm_prev.inequality_);
    offset_range_from_curr =
        GetOffsetRange_DAPerm(variable_od_range, perm_curr.inequality_);
  } else
    CoutError("Unrecognized type_trait_ in IsTwoPermConflicted_SameSource!");

  return Overlap(offset_range_from_prev, offset_range_from_curr) == 0 &&
         (!WhetherSerialAdjacent(offset_range_from_prev,
                                 offset_range_from_curr));
}

// return true if conflicted, false if safe
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
        variable_od_range.upper_bound.at(perm_curr.GetPrevTaskId()).deadline;
    deadline_curr_max =
        std::min(deadline_curr_max, deadline_curr_max_from_ineq);

    int rta_curr =
        variable_od_range.lower_bound.at(perm_curr.GetPrevTaskId()).deadline;
    int offset_curr_max = deadline_curr_max - rta_curr;
    if (offset_curr_max < offset_curr_min) {
      return false;
    }
  }
  return true;
}

// return true if conflicted, false if safe
bool ChainsPermutation::IsPermConflicted_CheckAllWithSameSource(
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
        permutation_chain_map_.end())  // this edge has not been added to the
                                       // chain yet, therefore no conflictions
      continue;
    const SinglePairPermutation& perm_ite =
        *permutation_chain_map_.at(edge_ite);
    if (IsTwoPermConflicted_SameSource(variable_od_range, perm_ite, perm_curr))
      return true;
  }
  return false;
}

// return true if conflicted, false if safe
bool ChainsPermutation::IsPermConflicted_CheckAllWithSameSink(
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
    if (permutation_chain_map_.find(edge_ite) == permutation_chain_map_.end())
      continue;  // this edge has not been added to the
                 // chain yet, therefore no conflictions
    const SinglePairPermutation& perm_ite =
        *permutation_chain_map_.at(edge_ite);
    if (IsTwoPermConflicted_SameSink(variable_od_range, perm_ite, perm_curr))
      return true;
  }
  return false;
}

// TODO: return true if conflicted, false if safe
bool ChainsPermutation::IsPermConflicted_CheckAllSerialConnect(
    const VariableRange& variable_od_range,
    const SinglePairPermutation& perm_curr,
    const GraphOfChains& graph_of_all_ca_chains) const {
  // check serial case: perm_ite -> perm_curr
  int source_task_id = perm_curr.GetPrevTaskId();
  std::vector<int> source_prev_task_ids =
      graph_of_all_ca_chains.GetPrevTasks(source_task_id);
  for (int prev_id : source_prev_task_ids) {
    Edge edge_ite(prev_id, source_task_id);
    if (permutation_chain_map_.find(edge_ite) == permutation_chain_map_.end())
      continue;  // this edge has not been added to the
                 // chain yet, therefore no conflictions
    const SinglePairPermutation& perm_ite =
        *permutation_chain_map_.at(edge_ite);
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
        permutation_chain_map_.end())  // this edge has not been added to the
                                       // chain yet, therefore no conflictions
      continue;
    const SinglePairPermutation& perm_ite =
        *permutation_chain_map_.at(edge_ite);
    if (!IsTwoPermConflicted_SerialConnect(variable_od_range, perm_curr,
                                           perm_ite))
      return false;
  }

  return true;
}

bool ChainsPermutation::IsValid(
    const VariableRange& variable_od_range,
    const SinglePairPermutation& perm_curr,
    const GraphOfChains& graph_of_all_ca_chains) const {
  int perm_single_chain_size = permutation_chain_map_.size();
  if (perm_single_chain_size > 0) {
    // TODO: this function has not been fully tested
    if (!IsPermConflicted_CheckAllSerialConnect(variable_od_range, perm_curr,
                                                graph_of_all_ca_chains))
      return false;
    if (IsPermConflicted_CheckAllWithSameSource(variable_od_range, perm_curr,
                                                graph_of_all_ca_chains))
      return false;

    if (IsPermConflicted_CheckAllWithSameSink(variable_od_range, perm_curr,
                                              graph_of_all_ca_chains))
      return false;
  }
  return true;
}

inline void UpdateVariableSafeMin(int& variable, int val,
                                  bool& whether_changed) {
  if (val > variable) whether_changed = true;
  variable = std::max(variable, val);
}
inline void UpdateVariableSafeMax(int& variable, int val,
                                  bool& whether_changed) {
  if (val < variable) whether_changed = true;
  variable = std::min(variable, val);
}

VariableRange FindPossibleVariableOD(const DAG_Model& dag_tasks,
                                     const TaskSetInfoDerived& tasks_info,
                                     const std::vector<int>& rta,
                                     const ChainsPermutation& chains_perm) {
  VariableRange variable_range = FindVariableRange(dag_tasks, rta);
  std::vector<Edge> edges = chains_perm.GetEdges();
  for (int i = 0; i <= chains_perm.size() + 1 + tasks_info.N;
       i++) {  // TODO: improve efficiency there
    bool whether_changed = false;
    for (const auto& edge_curr : edges) {
      const PermutationInequality& ineq = chains_perm[edge_curr]->inequality_;
      int prev_id = ineq.task_prev_id_;
      int next_id = ineq.task_next_id_;
      //      o_{task_next_id} + lower_bound < d_{task_prev_id} ;
      // variable_range.lower_bound[prev_id].deadline = std::max(
      //     variable_range.lower_bound[prev_id].deadline,
      //     variable_range.lower_bound[next_id].offset + ineq.lower_bound_ +
      //     1);
      UpdateVariableSafeMin(
          variable_range.lower_bound[prev_id].deadline,
          variable_range.lower_bound[next_id].offset + ineq.lower_bound_ + 1,
          whether_changed);

      // variable_range.upper_bound[next_id].offset = std::min(
      //     variable_range.upper_bound[next_id].offset,
      //     variable_range.upper_bound[prev_id].deadline - ineq.lower_bound_ -
      //     1);
      UpdateVariableSafeMax(
          variable_range.upper_bound[next_id].offset,
          variable_range.upper_bound[prev_id].deadline - ineq.lower_bound_ - 1,
          whether_changed);

      //      d_{task_prev_id} <= o_{task_next_id} + upper_bound ;
      // variable_range.lower_bound[next_id].offset = std::max(
      //     variable_range.lower_bound[next_id].offset,
      //     variable_range.lower_bound[prev_id].deadline - ineq.upper_bound_);
      UpdateVariableSafeMin(
          variable_range.lower_bound[next_id].offset,
          variable_range.lower_bound[prev_id].deadline - ineq.upper_bound_,
          whether_changed);

      // variable_range.upper_bound[prev_id].deadline = std::min(
      //     variable_range.upper_bound[prev_id].deadline,
      //     variable_range.upper_bound[next_id].offset + ineq.upper_bound_);
      UpdateVariableSafeMax(
          variable_range.upper_bound[prev_id].deadline,
          variable_range.upper_bound[next_id].offset + ineq.upper_bound_,
          whether_changed);

      // offset + rta <= deadline
      // variable_range.lower_bound[prev_id].deadline =
      //     std::max(variable_range.lower_bound[prev_id].deadline,
      //              variable_range.lower_bound[prev_id].offset +
      //              rta[prev_id]);
      UpdateVariableSafeMin(
          variable_range.lower_bound[prev_id].deadline,
          variable_range.lower_bound[prev_id].offset + rta[prev_id],
          whether_changed);

      // variable_range.upper_bound[prev_id].offset =
      //     std::min(variable_range.upper_bound[prev_id].offset,
      //              variable_range.upper_bound[prev_id].deadline -
      //              rta[prev_id]);
      UpdateVariableSafeMax(
          variable_range.upper_bound[prev_id].offset,
          variable_range.upper_bound[prev_id].deadline - rta[prev_id],
          whether_changed);

      // variable_range.lower_bound[next_id].deadline =
      //     std::max(variable_range.lower_bound[next_id].deadline,
      //              variable_range.lower_bound[next_id].offset +
      //              rta[next_id]);
      UpdateVariableSafeMin(
          variable_range.lower_bound[next_id].deadline,
          variable_range.lower_bound[next_id].offset + rta[next_id],
          whether_changed);

      // variable_range.upper_bound[next_id].offset =
      //     std::min(variable_range.upper_bound[next_id].offset,
      //              variable_range.upper_bound[next_id].deadline -
      //              rta[next_id]);
      UpdateVariableSafeMax(
          variable_range.upper_bound[next_id].offset,
          variable_range.upper_bound[next_id].deadline - rta[next_id],
          whether_changed);
    }
    if (!whether_changed) break;
  }
  return variable_range;
}

VariableOD FindBestPossibleVariableOD(const DAG_Model& dag_tasks,
                                      const TaskSetInfoDerived& tasks_info,
                                      const std::vector<int>& rta,
                                      const ChainsPermutation& chains_perm) {
#ifdef PROFILE_CODE
  BeginTimer(__FUNCTION__);
#endif
  VariableRange variable_range =
      FindPossibleVariableOD(dag_tasks, tasks_info, rta, chains_perm);
  VariableOD variable_od = variable_range.lower_bound;
  for (int i = 0; i < tasks_info.N; i++) {
    variable_od[i].offset = variable_range.upper_bound[i].offset;
  }
#ifdef PROFILE_CODE
  EndTimer(__FUNCTION__);
#endif
  return variable_od;
}

}  // namespace DAG_SPACE