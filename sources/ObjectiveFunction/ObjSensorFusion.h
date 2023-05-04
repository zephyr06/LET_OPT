#pragma once
#include "sources/ObjectiveFunction/ObjDataAge.h"

namespace DAG_SPACE {

std::vector<std::vector<int>> GetChainsForSF(
    const DAG_Model &dag_tasks, const TaskSetInfoDerived &tasks_info);

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
class ObjSensorFusion {
 public:
  static const std::string type_trait;
  static double Obj(const DAG_Model &dag_tasks,
                    const TaskSetInfoDerived &tasks_info,
                    const ChainsPermutation &chains_perm,
                    const VariableOD &variable_od);
  static double Obj(const DAG_Model &dag_tasks,
                    const TaskSetInfoDerived &tasks_info,
                    const ChainsPermutation &chains_perm,
                    const Schedule &schedule);
};

}  // namespace DAG_SPACE