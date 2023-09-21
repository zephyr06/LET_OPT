

#pragma once

#include "sources/TaskModel/DAG_Model.h"
#include "sources/TaskModel/GenerateRandomTaskset.h"

template <typename T> void shuffleRowsIn2DVector(std::vector<std::vector<T>> &twoDVector) {
  // Create a vector of indices
  std::vector<size_t> indices(twoDVector.size());
  for (size_t i = 0; i < indices.size(); ++i) {
    indices[i] = i;
  }

  // Shuffle the indices using std::shuffle
  std::random_device rd;
  std::mt19937 rng(rd()); // Use a random number generator
  std::shuffle(indices.begin(), indices.end(), rng);

  // Create a temporary vector to hold the shuffled 2D vector
  std::vector<std::vector<T>> shuffledTwoDVector(twoDVector.size());

  // Populate the temporary vector with shuffled rows
  for (size_t i = 0; i < indices.size(); ++i) {
    shuffledTwoDVector[i] = twoDVector[indices[i]];
  }
  // Replace the original 2D vector with the shuffled vector
  twoDVector = std::move(shuffledTwoDVector);
}

class RandomSelector {
public:
  RandomSelector() {}
  RandomSelector(const DAG_Model &dag_tasks, const std::vector<std::vector<int>> &chains)
      : chains(chains), chain_count(chains.size()) {
    AssignSelectionChance(dag_tasks, chains);
  }

  void AssignSelectionChance(const DAG_Model &dag_tasks, const std::vector<std::vector<int>> &chains) {
    selection_chance = std::vector<double>(chain_count, 1.0 / chain_count);
    for (uint i = 0; i < chain_count; i++) {
      selection_chance[i] = EvaluateChainSelectionChance(dag_tasks, chains[i]);
    }
    NormalizeSelectionChance();
  }

  void NormalizeSelectionChance() {
    double sum = std::accumulate(selection_chance.begin(), selection_chance.end(), 0.0);
    for (uint i = 0; i < chain_count; i++)
      selection_chance[i] /= sum;
  }

  double EvaluateChainSelectionChance(const DAG_Model &dag_tasks, const std::vector<int> &chain) {
    std::unordered_map<int, int> period_count_map;
    for (int task_id : chain) {
      int period_curr = dag_tasks.tasks[task_id].period;
      if (period_count_map.count(period_curr) > 0)
        period_count_map[period_curr]++;
      else
        period_count_map[period_curr] = 1;
    }

    double chance = 1;
    for (auto period_count_pair : period_count_map) {
      switch (period_count_pair.second) {
      case 2:
        chance *= 0.3;
        break;
      case 3:
        chance *= 0.4;
        break;
      case 4:
        chance *= 0.2;
        break;
      case 5:
        chance *= 0.1;
        break;
      default: // <= 1 or > 5
        chance *= 0;
        break;
      }
    }
    return chance;
  }

  std::vector<std::vector<int>> RandomSelectChains(size_t select_num) {

    RemoveInvalidChains();

    if (chain_count < select_num)
      return {};

    // Create a distribution for generating random numbers
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    std::vector<std::vector<int>> chains_selected;
    for (uint i = 0; i < select_num; ++i) {
      double randNum = dis(gen); // Generate a random number between 0 and 1

      // Select an object based on its probability
      double cumulativeProb = 0.0;
      for (int j = 0; j < static_cast<int>(chains.size()); j++) {
        cumulativeProb += selection_chance[j];
        if (randNum <= cumulativeProb) {
          chains_selected.push_back(chains[j]);
          RemoveChain(j);
          break;
        }
      }
    }
    return chains_selected;
  }

  void RemoveChain(size_t chain_index) {
    chains.erase(chains.begin() + chain_index);
    selection_chance.erase(selection_chance.begin() + chain_index);
    NormalizeSelectionChance();
    chain_count--;
  }

  void RemoveInvalidChains() {
    for (int i = chain_count - 1; i >= 0; i--) {
      if (selection_chance[i] == 0)
        RemoveChain(i);
    }
  }

  // data members
  std::vector<std::vector<int>> chains;
  size_t chain_count;
  std::vector<double> selection_chance;
};

inline std::vector<std::vector<int>> RandomSampleChains(const std::vector<std::vector<int>> &chains,
                                                        const DAG_Model &dag_tasks, int target_size) {
  RandomSelector selector(dag_tasks, chains);
  return selector.RandomSelectChains(target_size);
}

class RandomChainsGenerator {
public:
  RandomChainsGenerator() {}
  RandomChainsGenerator(const DAG_Model &dag_tasks) : dag_tasks_(dag_tasks) {
    for (int i = 0; i < 3; i++) {
      chains_all_period_.push_back({});
    }
  }

  std::vector<std::vector<int>> GenerateChain(int chain_num) {
    std::vector<std::vector<int>> chains;
    chains.reserve(chain_num);
    for (uint source_id = 0; source_id < dag_tasks_.tasks.size(); source_id++) {
      for (uint sink_id = 0; sink_id < dag_tasks_.tasks.size(); sink_id++) {
        if (source_id == sink_id)
          continue;
        std::vector<int> path = shortest_paths(source_id, sink_id, dag_tasks_.graph_);
        if (path.size() > 1) {
          PushChain(path);
        }
      }
    }
    return ObtainRandomChains(chain_num);
  }

  size_t CountPeriodType(const std::vector<int> &chain) {
    std::unordered_set<int> set;
    for (int task_id : chain) {
      int period_curr = dag_tasks_.tasks[task_id].period;
      set.insert(period_curr);
    }
    return set.size();
  }

  void PushChain(const std::vector<int> &chain) {
    size_t period_count = CountPeriodType(chain);
    if (period_count > 3)
      return;
    chains_all_period_[period_count - 1].push_back(chain);
  }

  std::vector<std::vector<int>> ObtainRandomChains(int chain_num) {

    std::vector<std::vector<int>> chains;
    std::vector<double> chain_dist = {0.7, 0.2, 0.1};
    std::vector<int> target_size_vec;
    int target_sum = 0;
    for (int period_type = 1; period_type <= 3; period_type++) {
      target_size_vec.push_back(round(chain_num * chain_dist[period_type - 1]));
      target_sum += target_size_vec.back();
    }
    int idx = 0;
    while (target_sum < chain_num) {
      if (round(chain_num * chain_dist[idx - 1]) != round(chain_num * chain_dist[idx - 1] + 0.5)) {
        target_size_vec[idx]++;
        target_sum++;
      }
      idx++;
    }
    while (target_sum > chain_num) {
      if (round(chain_num * chain_dist[idx - 1]) != round(chain_num * chain_dist[idx - 1] - 0.5)) {
        target_size_vec[idx]--;
        target_sum--;
      }
      idx++;
    }

    for (int period_type = 1; period_type <= 3; period_type++) {
      std::vector<std::vector<int>> &chains_curr = chains_all_period_[period_type - 1];
      // shuffleRowsIn2DVector<int>(chains_curr);
      size_t target_size = target_size_vec[period_type - 1];
      // chains_curr.resize(target_size);
      chains_curr = RandomSampleChains(chains_curr, dag_tasks_, target_size);
      chains.insert(chains.end(), chains_curr.begin(), chains_curr.end());
    }
    shuffleRowsIn2DVector<int>(chains);
    return chains;
  }

  // data members
  DAG_Model dag_tasks_;
  // chains with different types of periods,
  // [0] -> only 1 type of period,
  // [1] -> 2 types of periods,
  // [2] -> 3 types of periods
  std::vector<std::vector<std::vector<int>>> chains_all_period_;
};

void AddChains2DAG(DAG_Model &dag_tasks, int numCauseEffectChain);

DAG_Model GenerateDAG_WATERS15(int N, double totalUtilization, int numberOfProcessor,
                               int coreRequireMax, int taskType, int deadlineType,
                               double parallelismFactor, int numCauseEffectChain,
                               int sf_fork_num, int fork_sensor_num_min, int fork_sensor_num_max);

DAG_Model GenerateDAG_WATERS15(TaskSetGenerationParameters tasks_params);