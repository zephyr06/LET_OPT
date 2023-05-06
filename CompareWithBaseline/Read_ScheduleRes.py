from ScheduleResult import *

import numpy as np
import pandas as pd


# Normalize based on the method called "InitialMethod"
def Normalize(obj_vec, obj_base):
    for i in range(len(obj_vec)):
        obj_vec[i] = obj_vec[i] / obj_base[i]
    return obj_vec


def AverageList(lst):
    return sum(lst) / len(lst)


# return two float vectors, which save the average obj and run-time
def ReadOptResultsOneMethod(method_name="TOM_Sort", obj_type="DataAge", task_set_number_range=[5],
                            root_path=ROOT_PATH):
    res_obj_per_task_number = []
    res_runtime_per_task_number = []
    for task_num in task_set_number_range:
        obj_base = ReadOptResults("InitialMethod", obj_type, int(task_num), root_path)
        res_vec = ReadOptResults(method_name, obj_type, int(task_num), root_path)
        obj_curr, time_curr = Average(res_vec, obj_base, obj_type, task_num)
        res_obj_per_task_number.append(obj_curr)
        res_runtime_per_task_number.append(time_curr)
    return res_obj_per_task_number, res_runtime_per_task_number


# return panda's dataset dictionary
def ReadOptResultsAllMethod(method_name_vec, obj_type="ReactionTime", task_set_number_range=[5]):
    dataset_pd_obj = pd.DataFrame()
    dataset_pd_runtime = pd.DataFrame()
    dataset_pd_obj.insert(0, "index", task_set_number_range)
    dataset_pd_runtime.insert(0, "index", task_set_number_range)

    for method in method_name_vec:
        obj_vec, runtime_vec = ReadOptResultsOneMethod(method, obj_type, task_set_number_range)
        dataset_pd_obj.insert(0, method, obj_vec)
        dataset_pd_runtime.insert(0, method, runtime_vec)
    return dataset_pd_obj, dataset_pd_runtime
