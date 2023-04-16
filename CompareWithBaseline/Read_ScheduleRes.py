from ScheduleResult import *

import numpy as np
import pandas as pd


def ReadOptResultsOneMethod(method_name="TOM_Sort", obj_type="DataAge", task_set_number_range=[5],
                            root_path=ROOT_PATH):
    res_obj_per_task_number = []
    res_runtime_per_task_number = []
    for task_num in task_set_number_range:
        res_vec = ReadOptResults(method_name, obj_type, int(task_num), root_path)
        obj_curr, time_curr = Average(res_vec)
        res_obj_per_task_number.append(obj_curr)
        res_runtime_per_task_number.append(time_curr)
    return res_obj_per_task_number, res_runtime_per_task_number


# return panda's dataset dictionary
def ReadOptResultsAllMethod(method_name_vec, obj_type="ReactionTime", task_set_number_range=[5]):
    dataset_pd_obj = pd.DataFrame()
    dataset_pd_runtime = pd.DataFrame()
    index_seq = task_set_number_range
    dataset_pd_obj.insert(0, "index", index_seq)
    dataset_pd_runtime.insert(0, "index", index_seq)

    for method in method_name_vec:
        obj_vec, runtime_vec = ReadOptResultsOneMethod(obj_type, method, task_set_number_range)
        dataset_pd_obj.insert(0, method, obj_vec)
        dataset_pd_runtime.insert(0, method, runtime_vec)
    return dataset_pd_obj, dataset_pd_runtime
