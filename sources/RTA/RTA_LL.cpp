
#include "sources/RTA/RTA_LL.h"

namespace DAG_SPACE {

int GetResponseTime(const TaskSet &tasks, int index, int warm_start) {
    RTA_LL r(tasks);
    if (warm_start < tasks[index].executionTime)
        warm_start = tasks[index].executionTime;
    return r.RTA_Common_Warm(warm_start, index);
}

int GetResponseTime(const DAG_Model &dag_tasks, int task_id) {
    const TaskSet &tasks = dag_tasks.tasks;
    int processor_id = tasks[task_id].processorId;
    auto itr = dag_tasks.processor2taskset_.find(processor_id);
    if (itr == dag_tasks.processor2taskset_.end())
        CoutError("Unknown processor id!");

    RTA_LL r(itr->second);
    auto itr2 = dag_tasks.task_id2task_index_within_processor_.find(task_id);
    if (itr2 == dag_tasks.task_id2task_index_within_processor_.end())
        CoutError(
            "Unknown processor id (task_id2task_index_within_processor_)!");
    int task_index_p =
        dag_tasks.task_id2task_index_within_processor_.at(task_id);
    return r.RTA_Common_Warm(tasks[task_id].executionTime, task_index_p);
}

// std::vector<int> GetResponseTimeOfChain(const TaskSet &tasks,
//                                         const std::vector<int> &chain) {
//     std::vector<int> rta_vec;
//     rta_vec.reserve(chain.size());
//     for (uint i = 0; i < chain.size(); i++)
//         rta_vec.push_back(GetResponseTime(tasks, chain[i]));
//     return rta_vec;
// }

}  // namespace DAG_SPACE