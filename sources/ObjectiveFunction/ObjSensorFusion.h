#pragma once
#include "sources/ObjectiveFunction/ObjDataAge.h"

namespace DAG_SPACE {

std::vector<std::vector<int>> GetChainsForSF(const DAG_Model &dag_tasks);

struct SF_JobFork {
  SF_JobFork() {}
  SF_JobFork(JobCEC sink_job, std::vector<JobCEC> source_jobs)
      : sink_job(sink_job), source_jobs(source_jobs) {}
  // data members
  JobCEC sink_job;
  std::vector<JobCEC> source_jobs;
};

int GetSF_Diff(const SF_JobFork &job_forks, const Schedule &schedule,
               const TaskSetInfoDerived &tasks_info);
int GetSF_Diff(const SF_JobFork &job_forks, const VariableOD &variable_od,
               const TaskSetInfoDerived &tasks_info);

SF_JobFork GetSF_JobFork(JobCEC sink_job, const std::vector<int> &source_tasks,
                         const TaskSetInfoDerived &tasks_info,
                         const ChainsPermutation &chains_perm);

SF_JobFork GetSF_JobFork(JobCEC sink_job, const std::vector<int> &source_tasks,
                         const TaskSetInfoDerived &tasks_info,
                         const Schedule &schedule);
class ObjSensorFusion {
 public:
  static const std::string type_trait;
  static std::vector<double> ObjAllInstances_SingleFork(
      const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
      const ChainsPermutation &chains_perm, const VariableOD &variable_od,
      const SF_Fork &fork_curr);

  static std::vector<double> ObjAllInstances_SingleFork(
      const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info,
      const Schedule &schedule, const SF_Fork &fork_curr);

  static double Obj(const DAG_Model &dag_tasks,
                    const TaskSetInfoDerived &tasks_info,
                    const ChainsPermutation &chains_perm,
                    const VariableOD &variable_od,
                    const std::vector<std::vector<int>> /*unusedArg*/);
  static double Obj(const DAG_Model &dag_tasks,
                    const TaskSetInfoDerived &tasks_info,
                    const ChainsPermutation & /*unusedArg*/,
                    const Schedule &schedule,
                    const std::vector<std::vector<int>> /*unusedArg*/);

  static double Jitter(const DAG_Model &dag_tasks,
                       const TaskSetInfoDerived &tasks_info,
                       const ChainsPermutation &chains_perm,
                       const VariableOD &variable_od,
                       const std::vector<std::vector<int>> & /*unusedArg*/);
  static double Jitter(const DAG_Model &dag_tasks,
                       const TaskSetInfoDerived &tasks_info,
                       const ChainsPermutation & /*unusedArg*/,
                       const Schedule &schedule,
                       const std::vector<std::vector<int>> & /*unusedArg*/);
};

// *********************** Get Individual SF Forks
bool ForkIntersect(const SF_Fork &fork1, const SF_Fork &fork2);
bool ForkIntersect(const std::vector<SF_Fork> &forks1,
                   const std::vector<SF_Fork> &forks2);
void Merge_j2i(std::vector<std::vector<SF_Fork>> &decoupled_sf_forks, uint i,
               uint j);
std::vector<std::vector<SF_Fork>> ExtractDecoupledForks(
    const std::vector<SF_Fork> &sf_forks_all);

std::vector<DAG_Model> ExtractDAGsWithIndependentForks(
    const DAG_Model &dag_tasks);
// Get Individual SF Forks ***********************

}  // namespace DAG_SPACE