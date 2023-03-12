
#include "sources/Optimization/TwoTaskPermutations.h"

namespace DAG_SPACE {
// TODO: add a unit-test with test_n3_v8
std::vector<JobCEC> GetPossibleReactingJobs(
    const JobCEC& job_curr, const Task& task_next, int superperiod,
    const RegularTaskSystem::TaskSetInfoDerived& tasksInfo) {
    int job_min_finish = GetActivationTime(job_curr, tasksInfo) +
                         GetExecutionTime(job_curr, tasksInfo);
    int job_max_finish = GetDeadline(job_curr, tasksInfo);
    int period_next = tasksInfo.GetTask(task_next.id).period;
    std::vector<JobCEC> reactingJobs;
    reactingJobs.reserve(superperiod /
                         tasksInfo.GetTask(job_curr.taskId).period);
    for (int i = std::floor(float(job_min_finish) / period_next);
         i <= std::ceil(float(job_max_finish) / period_next); i++)
        reactingJobs.push_back(JobCEC(task_next.id, i));

    return reactingJobs;
}

PermutationInequality GenerateBoxPermutationConstraints(
    int task_prev_id, int task_next_id, const VariableRange& variable_range) {
    return PermutationInequality(
        task_prev_id, task_next_id,
        variable_range.lower_bound[task_prev_id].deadline -
            variable_range.upper_bound[task_next_id].offset,
        true,
        variable_range.upper_bound[task_prev_id].deadline -
            variable_range.lower_bound[task_next_id].offset,
        true);
}

void SinglePairPermutation::print() const {
    inequality_.print();
    for (const auto& [key, value] : job_first_react_matches_) {
        std::cout << key.ToString() << "'s following jobs are ";
        for (auto job_next : value) std::cout << job_next.ToString() << ", ";
        std::cout << "\n";
    }
    std::cout << "\n";
}

bool SinglePairPermutation::AddMatchJobPair(const JobCEC& job_curr,
                                            const JobCEC& job_match) {
    auto itr = job_first_react_matches_.find(job_curr);
    if (itr == job_first_react_matches_.end())
        job_first_react_matches_[job_curr] = {job_match};
    else {
        JobCEC last_matched_job = itr->second.back();
        if (job_match.jobId < last_matched_job.jobId) return false;
        if (job_first_react_matches_[job_curr].size() > 0 &&
            job_first_react_matches_[job_curr].back().jobId > job_match.jobId)
            CoutError("Wrong order in AddMatchJobPair!");
        job_first_react_matches_[job_curr].push_back(job_match);
    }

    return true;
}

bool TwoTaskPermutations::AppendJobs(
    const JobCEC& job_curr, const JobCEC& job_match,
    SinglePairPermutation& permutation_current) {
    PermutationInequality perm_new(job_curr, job_match, tasks_info_);
    PermutationInequality perm_merged =
        MergeTwoSinglePermutations(perm_new, permutation_current.inequality_);
    // Add bound constraints
    if (GlobalVariablesDAGOpt::SKIP_PERM) {
        PermutationInequality perm_bound = GenerateBoxPermutationConstraints(
            job_curr.taskId, job_match.taskId, variable_od_range_);
        perm_merged = MergeTwoSinglePermutations(perm_merged, perm_bound);
    }

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
    std::vector<JobCEC> jobs_possible_match =
        GetPossibleReactingJobs(job_curr, tasks_info_.GetTask(task_next_id_),
                                superperiod_, tasks_info_);

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