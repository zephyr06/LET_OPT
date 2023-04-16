from ScheduleResult import *

import numpy as np
import pandas as pd


def ReadOptResultsOneMethod(obj_type="DataAge", method_name="TOM_Sort", task_set_min=5, task_set_max=5,
                            root_path=ROOT_PATH):
    res_obj_per_task_number = []
    res_runtime_per_task_number = []
    for task_num in np.linspace(task_set_min, task_set_max, task_set_max - task_set_min + 1):
        res_vec = ReadOptResults(obj_type, method_name, int(task_num), root_path)
        obj_curr, time_curr = Average(res_vec)
        res_obj_per_task_number.append(obj_curr)
        res_runtime_per_task_number.append(time_curr)
    return res_obj_per_task_number, res_runtime_per_task_number


def ReadOptResultsAllMethod(obj_type="ReactionTime", task_set_min=5, task_set_max=5):
    dataset_pd = pd.DataFrame()
    dataset_pd.insert(0, "index", np.linspace(task_set_min, task_set_max, task_set_max - task_set_min + 1))
