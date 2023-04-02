#include "sources/Optimization/TaskSetPermutationHelper.h"

namespace DAG_SPACE {

std::vector<std::vector<int>> GetSubChains(
    const std::vector<std::vector<int>>& chains_full_length,
    const ChainsPermutation& chains_perm) {
  std::vector<Edge> edges = chains_perm.GetEdges();
  std::unordered_set<Edge> edge_record;
  edge_record.reserve(edges.size());
  for (const auto& edge : edges) edge_record.insert(edge);
  std::vector<std::vector<int>> sub_chains;
  sub_chains.reserve(chains_full_length.size());
  for (uint i = 0; i < chains_full_length.size(); i++) {
    const auto& chain = chains_full_length[i];
    sub_chains.push_back({});
    for (uint j = 0; j < chain.size() - 1; j++) {
      Edge edge_curr(chain[j], chain[j + 1]);
      if (edge_record.find(edge_curr) == edge_record.end()) {
        break;
      } else {
        if (j == 0) {
          // sub_chains.push_back({chain[j], chain[j + 1]});
          sub_chains.back().push_back(chain[j]);
          sub_chains.back().push_back(chain[j + 1]);
        } else {
          // sub_chains.end()->push_back(chain[j]);
          sub_chains.back().push_back(chain[j + 1]);
        }
      }
    }
  }

  return sub_chains;
}
// Note: this function doesn't change chains_perm
std::vector<std::unordered_map<JobCEC, JobCEC>> GetFirstReactMaps(
    ChainsPermutation& chains_perm,
    const std::shared_ptr<const SinglePairPermutation> single_perm,
    const std::vector<std::vector<int>>& chains, const DAG_Model& dag_tasks,
    const TaskSetInfoDerived& tasks_info) {
  chains_perm.push_back(single_perm);
  auto first_job_map =
      GetFirstReactMaps(chains_perm, chains, dag_tasks, tasks_info);
  chains_perm.pop(*single_perm);
  return first_job_map;
}

std::vector<std::unordered_map<JobCEC, JobCEC>> GetFirstReactMaps(
    const ChainsPermutation& chains_perm,
    const std::vector<std::vector<int>>& chains, const DAG_Model& dag_tasks,
    const TaskSetInfoDerived& tasks_info) {
#ifdef PROFILE_CODE
  BeginTimer(__FUNCTION__);
#endif

  std::vector<std::unordered_map<JobCEC, JobCEC>> first_job_map;
  first_job_map.reserve(chains.size());

  std::vector<std::vector<int>> sub_chains = GetSubChains(chains, chains_perm);
  for (uint i = 0; i < chains.size(); i++) {
    const std::vector<int>& chain_sub = sub_chains[i];
    if (chain_sub.size() == 0) {
      first_job_map.push_back(std::unordered_map<JobCEC, JobCEC>());
    } else {
      first_job_map.push_back(
          GetFirstReactMap(dag_tasks, tasks_info, chains_perm, chain_sub));
    }
  }
#ifdef PROFILE_CODE
  EndTimer(__FUNCTION__);
#endif
  return first_job_map;
}

// return true if it's possible for curr_first_job_maps to achieve better
// performance than curr_best_first_job_maps
bool CompareNewPerm(
    const std::vector<std::unordered_map<JobCEC, JobCEC>>& curr_first_job_maps,
    const std::vector<std::unordered_map<JobCEC, JobCEC>>&
        curr_best_first_job_maps) {
  if (curr_best_first_job_maps.size() == 0) return true;
  uint chain_size = curr_first_job_maps.size();
  if (chain_size != curr_best_first_job_maps.size())
    CoutError("Inconsistent map size in CompareNewPerm!");
  bool whether_find_better_react = false;
  bool whether_find_worse_react = false;  // TODO: use it
  for (uint i = 0; i < chain_size; i++) {
    const std::unordered_map<JobCEC, JobCEC>& curr_map = curr_first_job_maps[i];
    const std::unordered_map<JobCEC, JobCEC>& best_map =
        curr_best_first_job_maps[i];
    for (auto itr = best_map.begin(); itr != best_map.end(); itr++) {
      const JobCEC& start_job = itr->first;
      const JobCEC& best_react_job = itr->second;
      if (curr_map.find(start_job) == curr_map.end()) {
        CoutError("Job not found in CompareNewPerm!");
      } else {
        if (best_react_job.jobId > curr_map.at(start_job).jobId) {
          whether_find_better_react = true;
          return true;
        } else if (best_react_job.jobId < curr_map.at(start_job).jobId)
          whether_find_worse_react = true;
      }
    }
  }
  if (GlobalVariablesDAGOpt::SearchDP_Friendly) {
    if (!whether_find_worse_react) return true;
  }

  return false;
}

bool ExamVariableFeasibility(const VariableOD& variable,
                             const ChainsPermutation& chains_perm,
                             const GraphOfChains& graph_of_all_ca_chains,
                             const DAG_Model& dag_task,
                             const std::vector<int>& chain) {
#ifdef PROFILE_CODE
  BeginTimer(__FUNCTION__);
#endif
  for (uint i = 0; i < chain.size() - 1; i++) {
    Edge edge(chain[i], chain[i + 1]);
    const PermutationInequality& inequality = chains_perm[edge]->inequality_;
    int prev_id = inequality.task_prev_id_;
    int next_id = inequality.task_next_id_;
    if (inequality.lower_bound_valid_) {
      if (variable.at(next_id).offset + inequality.lower_bound_ >=
              variable.at(prev_id).deadline ||
          variable.at(prev_id).deadline >
              variable.at(next_id).offset + inequality.upper_bound_) {
        if (GlobalVariablesDAGOpt::debugMode) inequality.print();
#ifdef PROFILE_CODE
        EndTimer(__FUNCTION__);
#endif
        return false;
      }
    }
  }
  for (uint i = 0; i < variable.size(); i++) {
    if (variable.at(i).deadline > dag_task.GetTask(i).deadline) {
#ifdef PROFILE_CODE
      EndTimer(__FUNCTION__);
#endif
      return false;
    }
  }
#ifdef PROFILE_CODE
  EndTimer(__FUNCTION__);
#endif
  return true;
}

//      o_{task_next_id} + lower_bound < d_{task_prev_id} ;
//      d_{task_prev_id} <= o_{task_next_id} + upper_bound ;
//  const std::vector<int>& rta
void SetVariableHelperSingleEdge(const Edge& edge, VariableOD& variable,
                                 const PermutationInequality& ineq,
                                 const DAG_Model& dag_tasks,
                                 const std::vector<int>& rta) {
#ifdef PROFILE_CODE
  BeginTimer(__FUNCTION__);
#endif
  //  edge.from_id, edge_to_id
  variable.SetMinDeadline(edge.from_id, dag_tasks, rta);
  variable.SetOffset(edge.to_id,
                     variable[edge.from_id].deadline - ineq.upper_bound_);
  if (variable[edge.from_id].deadline - ineq.upper_bound_ < 0) {
    variable[edge.to_id].offset = 0;
    variable[edge.from_id].deadline =
        std::max(variable[edge.from_id].deadline,
                 variable[edge.to_id].offset + ineq.lower_bound_ + 1);
    if (variable[edge.from_id].deadline >
        dag_tasks.GetTask(edge.from_id).deadline) {
      variable.valid_ = false;
      // return;
    }
  }
  if (variable[edge.to_id].offset + rta[edge.to_id] >
      dag_tasks.GetTask(edge.to_id).deadline) {
    variable.valid_ = false;
    // return;
  }
#ifdef PROFILE_CODE
  EndTimer(__FUNCTION__);
#endif
}

VariableOD FindODFromSingleChainPermutation(
    const DAG_Model& dag_tasks, const ChainsPermutation& chains_perm,
    const GraphOfChains& graph_of_all_ca_chains, const std::vector<int>& chain,
    const std::vector<int>& rta) {
#ifdef PROFILE_CODE
  BeginTimer(__FUNCTION__);
#endif
  const TaskSet& tasks = dag_tasks.GetTaskSet();
  VariableOD variable(tasks);

  for (uint i = 0; i < chain.size() - 1; i++) {
    Edge edge(chain[i], chain[i + 1]);
    const SinglePairPermutation& single_perm = *chains_perm[edge];
    SetVariableHelperSingleEdge(edge, variable, single_perm.inequality_,
                                dag_tasks, rta);
    if (!variable.valid_) break;
  }
  if (variable.valid_)
    variable.valid_ = ExamVariableFeasibility(
        variable, chains_perm, graph_of_all_ca_chains, dag_tasks, chain);
#ifdef PROFILE_CODE
  EndTimer(__FUNCTION__);
#endif
  return variable;
}

// set each variable's offset and deadline
// assumes acyclic graph
// void SetVariableHelperSingleChain(
//     int task_id, std::unordered_map<int, bool>& variable_set_record,
//     VariableOD& variable, const ChainsPermutation& chains_perm,
//     const GraphOfChains& graph_of_all_ca_chains, const DAG_Model& dag_tasks,
//     const std::unordered_set<Edge>& sub_chain_edge_record) {
//     if (variable_set_record.find(task_id) == variable_set_record.end() ||
//         variable_set_record[task_id] == false) {
//         if (!variable.valid_) return;
//         int offset_min = 0;
//         if (graph_of_all_ca_chains.prev_tasks_.find(task_id) !=
//             graph_of_all_ca_chains.prev_tasks_.end()) {
//             const std::vector<int>& dependent_tasks =
//                 graph_of_all_ca_chains.prev_tasks_.at(task_id);
//             for (int dependent_task : dependent_tasks) {
//                 SetVariableHelperSingleChain(
//                     dependent_task, variable_set_record, variable,
//                     chains_perm, graph_of_all_ca_chains, dag_tasks,
//                     sub_chain_edge_record);
//                 Edge edge_curr(dependent_task, task_id);
//                 offset_min = std::max(
//                     offset_min,
//                     variable[dependent_task].deadline -
//                         chains_perm[edge_curr].inequality_.upper_bound_);
//             }
//         }
//         variable[task_id].offset = offset_min;
//         variable[task_id].deadline =
//             offset_min + GetResponseTime(dag_tasks, task_id);
//         // exam whether the current offset assignment violates prev tasks'
//         // lower bound constraints
//         if (graph_of_all_ca_chains.prev_tasks_.find(task_id) !=
//             graph_of_all_ca_chains.prev_tasks_.end()) {
//             const std::vector<int>& dependent_tasks =
//                 graph_of_all_ca_chains.prev_tasks_.at(task_id);
//             for (int dependent_task : dependent_tasks) {
//                 SetVariableHelperSingleChain(
//                     dependent_task, variable_set_record, variable,
//                     chains_perm, graph_of_all_ca_chains, dag_tasks,
//                     sub_chain_edge_record);
//                 Edge edge_curr(dependent_task, task_id);
//                 const PermutationInequality& ineq =
//                     chains_perm[edge_curr].inequality_;
//                 if (variable[task_id].offset + ineq.lower_bound_ >=
//                     variable[dependent_task].deadline) {
//                     variable[dependent_task].deadline =
//                         variable[task_id].offset + ineq.lower_bound_ + 1;
//                     if (variable[dependent_task].deadline >
//                         dag_tasks.GetTask(dependent_task).deadline)
//                         variable.valid_ = false;
//                 }
//             }
//         }
//         if (variable[task_id].deadline > dag_tasks.GetTask(task_id).deadline)
//         {
//             variable.valid_ = false;
//         }
//         variable_set_record[task_id] = true;
//     } else
//         return;
// }

}  // namespace DAG_SPACE
