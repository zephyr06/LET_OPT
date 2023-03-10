
#include "sources/RTA/RTA_LL.h"

namespace DAG_SPACE {

VectorDynamic RTAVector(const TaskSet &tasks) {
    BeginTimer(__func__);
    RTA_LL r(tasks);
    auto res = r.ResponseTimeOfTaskSet();
    EndTimer(__func__);
    return res;
}

int GetResponseTime(const TaskSet &tasks, int index, int warm_start) {
    RTA_LL r(tasks);
    if (warm_start < tasks[index].executionTime)
        warm_start = tasks[index].executionTime;
    return r.RTA_Common_Warm(warm_start, index);
}

std::vector<int> GetResponseTimeOfChain(const TaskSet &tasks,
                                        const std::vector<int> &chain) {
    std::vector<int> rta_vec;
    rta_vec.reserve(chain.size());
    for (uint i = 0; i < chain.size(); i++)
        rta_vec.push_back(GetResponseTime(tasks, chain[i]));
    return rta_vec;
}

}  // namespace DAG_SPACE