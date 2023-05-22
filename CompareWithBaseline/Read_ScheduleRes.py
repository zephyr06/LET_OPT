from ScheduleResult import *

import numpy as np
import pandas as pd




def AverageList(lst):
    return sum(lst) / len(lst)


# return two float vectors, which save the average obj and run-time
def ReadOptResultsOneMethod(method_name="TOM_Sort", obj_type="DataAge", task_set_number_range=[5], exclude_time_out=False,
                            method_name_vec=[], root_path=ROOT_PATH):
    res_obj_per_task_number = []
    res_runtime_per_task_number = []
    excluded_table = []

    for task_num in task_set_number_range:
        if exclude_time_out:
            res_vec = ReadOptResults(
                "InitialMethod", obj_type, int(task_num), root_path)
            excluded_table = [False] * len(res_vec)
            for temp_name in method_name_vec:
                res_vec = ReadOptResults(
                    temp_name, obj_type, int(task_num), root_path)
                for i in range(len(res_vec)):
                    if exclude_time_out and res_vec[i].runtime >= 600:
                        excluded_table[i] = True
                        
        obj_base = ReadOptResults(
            "InitialMethod", obj_type, int(task_num), root_path)
        res_vec = ReadOptResults(
            method_name, obj_type, int(task_num), root_path)
        obj_curr, time_curr = Average(
            res_vec, obj_base, obj_type, task_num, exclude_time_out, excluded_table)
        res_obj_per_task_number.append(obj_curr)
        res_runtime_per_task_number.append(time_curr)
    return res_obj_per_task_number, res_runtime_per_task_number


# return panda's dataset dictionary
def ReadOptResultsAllMethod(method_name_vec, obj_type="ReactionTime", task_set_number_range=[5], exclude_time_out=False):
    dataset_pd_obj = pd.DataFrame()
    dataset_pd_runtime = pd.DataFrame()
    dataset_pd_obj.insert(0, "index", task_set_number_range)
    dataset_pd_runtime.insert(0, "index", task_set_number_range)

    for method in method_name_vec:
        obj_vec, runtime_vec = ReadOptResultsOneMethod(
            method, obj_type, task_set_number_range, exclude_time_out, method_name_vec)
        dataset_pd_obj.insert(0, method, obj_vec)
        dataset_pd_runtime.insert(0, method, runtime_vec)
        if verbose_mode:
            print(method, "has objs :", obj_vec)
    return dataset_pd_obj, dataset_pd_runtime
