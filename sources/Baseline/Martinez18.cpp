#include "sources/Baseline/Martinez18.h"

namespace DAG_SPACE {

VariableOD MartPerm2VariableOD(const Martinez18Perm& mart_perm,
                               const DAG_Model& dag_tasks) {
  VariableOD variable_od(dag_tasks.GetTaskSet());
  for (uint i = 0; i < variable_od.size(); i++) {
    variable_od[i].offset = mart_perm[i];
    variable_od[i].deadline =
        variable_od[i].offset + dag_tasks.GetTask(i).period;
  }
  return variable_od;
}

int ObjDataAgeFromVariable(const Martinez18Perm& mart_perm,
                           const DAG_Model& dag_tasks,
                           const TaskSetInfoDerived& tasks_info,
                           const std::vector<int>& chain) {
  VariableOD variable_od = MartPerm2VariableOD(mart_perm, dag_tasks);
  ChainsPermutation chains_perm = GetChainsPermFromMartVariable(
      dag_tasks, tasks_info, {chain}, "DataAge", variable_od);
  return ObjDataAge::Obj(dag_tasks, tasks_info, chains_perm, variable_od,
                         {chain});
}

std::vector<int> Martinez18TaskSetPerms::GetPossibleOffset(int gcd) {
  std::vector<int> offsets;
  offsets.reserve(gcd);
  for (int i = 0; i <= gcd; i++)
    offsets.push_back(i);  // i<gcd should also work
  return offsets;
}

void Martinez18TaskSetPerms::FindPossibleOffsets() {
  possible_offsets_map_.reserve(chain_.size());
  possible_offsets_map_[chain_[0]] = {0};
  int hp_yet = dag_tasks_.GetTask(chain_[0]).period;
  for (uint i = 1; i < chain_.size(); i++) {
    auto itr = possible_offsets_map_.find(chain_[i]);
    assert(itr == possible_offsets_map_.end());
    int period_curr = dag_tasks_.GetTask(chain_[i]).period;
    int gcd_max = std::gcd(hp_yet, period_curr);
    possible_offsets_map_[chain_[i]] = GetPossibleOffset(gcd_max);
    hp_yet = std::lcm(hp_yet, period_curr);
  }
}

int Martinez18TaskSetPerms::PerformOptimization() {
  Martinez18Perm perms_offset(tasks_info_.N);
  Iterate(0, perms_offset);
  return best_yet_obj_;
}

void Martinez18TaskSetPerms::Iterate(uint position,
                                     Martinez18Perm& perms_offset) {
  if (position == chain_.size()) {  // finish iterate all the pair permutations
    iteration_count_++;
    if (GlobalVariablesDAGOpt::debugMode) perms_offset.print();
    EvaluateMartPerm(perms_offset);
    return;
  };

  int task_id = chain_[position];
  const std::vector<int> possible_offsets_curr = possible_offsets_map_[task_id];

  for (int offset_curr : possible_offsets_curr) {
    if (ifTimeout(start_time_)) break;
    perms_offset[task_id] = offset_curr;
    Iterate(position + 1, perms_offset);
  }
}

void Martinez18TaskSetPerms::EvaluateMartPerm(Martinez18Perm& perms_offset) {
  if (EvaluateMartSchedulability(perms_offset)) {
    int obj_curr =
        ObjDataAgeFromVariable(perms_offset, dag_tasks_, tasks_info_, chain_);
    if (obj_curr < best_yet_obj_) {
      best_yet_obj_ = obj_curr;
      best_yet_variable_od_ = perms_offset;
    }
  }
}

bool Martinez18TaskSetPerms::EvaluateMartSchedulability(
    Martinez18Perm& perms_offset) {
  for (int task_id = 0; task_id < tasks_info_.N; task_id++) {
    if (rta_[task_id] > dag_tasks_.GetTask(task_id).deadline) return false;
    if (perms_offset[task_id] == -1) return false;
  }

  return true;
}

}  // namespace DAG_SPACE