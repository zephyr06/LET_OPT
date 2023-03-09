
#include "sources/Optimization/TwoTaskPermutations.h"

namespace DAG_SPACE {

std::vector<JobCEC> GetPossibleReactingJobs(
    const JobCEC& job_curr, const Task& task_next, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasksInfo) {
    int job_min_finish = GetActivationTime(job_curr, tasksInfo) +
                         GetExecutionTime(job_curr, tasksInfo);
    int job_max_finish = GetDeadline(job_curr, tasksInfo);
    int period_next = tasksInfo.tasks[task_next.id].period;
    std::vector<JobCEC> reactingJobs;
    reactingJobs.reserve(superperiod / tasksInfo.tasks[job_curr.taskId].period);
    for (int i = std::floor(float(job_min_finish) / period_next);
         i <= std::ceil(float(job_max_finish) / period_next); i++)
        reactingJobs.push_back(JobCEC(task_next.id, i));

    return reactingJobs;
}

void SinglePairPermutation::print() const {
    inequality_.print();
    for (const auto& [key, value] : job_matches_) {
        std::cout << key.ToString() << "'s following jobs are ";
        for (auto job_next : value) std::cout << job_next.ToString() << ", ";
        std::cout << "\n";
    }
    std::cout << "\n";
}

bool SinglePairPermutation::AddMatchJobPair(const JobCEC& job_curr,
                                            const JobCEC& job_match) {
    auto itr = job_matches_.find(job_curr);
    if (itr == job_matches_.end())
        job_matches_[job_curr] = {job_match};
    else {
        JobCEC last_matched_job = itr->second.back();
        if (job_match.jobId < last_matched_job.jobId) return false;
        job_matches_[job_curr].push_back(job_match);
    }

    return true;
}

bool TwoTaskPermutations::AppendJobs(
    const JobCEC& job_curr, const JobCEC& job_match,
    SinglePairPermutation& permutation_current) {
    PermutationInequality perm_new(job_curr, job_match, tasks_info_);
    PermutationInequality perm_merged =
        MergeTwoSinglePermutations(perm_new, permutation_current.inequality_);
    if (perm_merged.IsValid()) {
        permutation_current.inequality_ = perm_merged;
        if (!(permutation_current.AddMatchJobPair(job_curr, job_match)))
            return false;
        return true;
    } else
        return false;
}

void TwoTaskPermutations::AppendAllPermutations(
    const JobCEC& job_curr, SinglePairPermutation& permutation_current) {
    std::vector<JobCEC> jobs_possible_match = GetPossibleReactingJobs(
        job_curr, tasks_info_.tasks[task_next_id_], superperiod_, tasks_info_);

    for (auto job_match : jobs_possible_match) {
        SinglePairPermutation permutation_current_copy = permutation_current;
        if (AppendJobs(job_curr, job_match, permutation_current_copy)) {
            if (job_curr.jobId ==
                superperiod_ / GetPeriod(job_curr, tasks_info_) -
                    1) {  // reach end, record the current permutations
                single_permutations_.push_back(permutation_current_copy);

                if (single_permutations_.size() > 1e5)
                    CoutError("Possibly too many permutations!");
            } else {
                JobCEC job_next(job_curr.taskId, job_curr.jobId + 1);
                AppendAllPermutations(job_next, permutation_current_copy);
            }
        } else  // skip this match because it's not feasible or useful
            ;
    }
}

void TwoTaskPermutations::FindAllPermutations() {
    JobCEC job_curr(task_prev_id_, 0);
    PermutationInequality perm_ineq(task_prev_id_, task_next_id_);
    SinglePairPermutation single_permutation(perm_ineq, tasks_info_);
    AppendAllPermutations(job_curr, single_permutation);
}

}  // namespace DAG_SPACE