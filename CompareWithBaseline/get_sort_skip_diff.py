from ScheduleResult import *
from PlotResults import *
from IPython.display import display

# return two float vectors, which save the average obj and run-time


def GetSortSkipDiff(task_set_number_range=[5], obj_type = "DataAge"):
    method_names = ["TOM_WSkip", "TOM_Sort"]
    dataset_pd_obj, dataset_pd_jitter, dataset_pd_runtime, dataset_pd_timeout_rate = ReadOptResultsAllMethod(
    method_names, obj_type, task_set_number_range)
    
    dataset_pd_obj['ObjDiff'] = dataset_pd_obj['TOM_Sort'] - dataset_pd_obj['TOM_WSkip']
    dataset_pd_obj['ObjDiffRelative'] = (dataset_pd_obj['TOM_Sort'] - dataset_pd_obj['TOM_WSkip']) / (100 + dataset_pd_obj['TOM_WSkip']) * 100.
    dataset_pd_runtime['TimeDiff'] = dataset_pd_runtime['TOM_WSkip']-dataset_pd_runtime['TOM_Sort']
    dataset_pd_runtime['TimeDiffRelative'] = (dataset_pd_runtime['TOM_Sort'] - dataset_pd_runtime['TOM_WSkip']) / dataset_pd_runtime['TOM_WSkip'] * 100.
    dataset_pd_runtime['TimeDiffRate'] = dataset_pd_runtime['TOM_WSkip'] / dataset_pd_runtime['TOM_Sort']
    display(dataset_pd_obj)
    display(dataset_pd_runtime)
    

GetSortSkipDiff([5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21], obj_type="DataAgeOneChain")
