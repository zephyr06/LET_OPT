#pragma once
#include "sources/Baseline/ImplicitCommunication/RunQueue.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/JobCEC.h"
#include "sources/Utils/MatirxConvenient.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/colormod.h"
namespace DAG_SPACE {

void AddTasksToRunQueue(RunQueue &run_queue, const DAG_Model &dag_tasks,
                        int processor_id, LLint time_now);

Schedule SimulatedFTP_SingleCore(const DAG_Model &dag_tasks,
                                 const TaskSetInfoDerived &tasks_info,
                                 int processor_id);

Schedule SimulateFixedPrioritySched(const DAG_Model &dag_tasks,
                                    const TaskSetInfoDerived &tasks_info);

Schedule SimulatedFTP_SingleCore_OD(const DAG_Model &dag_tasks,
                                    const TaskSetInfoDerived &tasks_info,
                                    int processor_id,
                                    const VariableOD &variable_od);

Schedule SimulateFixedPrioritySched_OD(const DAG_Model &dag_tasks,
                                       const TaskSetInfoDerived &tasks_info,
                                       const VariableOD &variable_od);
}  // namespace DAG_SPACE
