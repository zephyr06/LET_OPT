import sys
import seaborn as sns
import matplotlib.pyplot as plt

sys.path.insert(1, '~/programming/LET_OPT/CompareWithBaseline')
from Read_ScheduleRes import *
from GlobalVariables import *


def plot_Obj_results(task_set_number_range, method_names, obj):
    dataset_pd_obj, dataset_pd_runtime = ReadOptResultsAllMethod(method_names, obj, task_set_number_range)

    plt.figure()
    for i in range(len(method_names)):
        splot = sns.lineplot(data=dataset_pd_obj, x="index", y=method_names[i], marker=marker_list[i],
                             color=color_list[i], label=method_names[i],
                             markersize=markersize_list[i])  # , alpha=alpha_list[i]
    font_size = 15
    plt.xlabel("Task Number", fontsize=font_size)
    plt.ylabel("Relative performance gap(%)", fontsize=font_size)
    splot.set_ylim([0.2, 1.05])
    plt.legend()
    plt.grid(linestyle="--")
    plt.savefig(ROOT_CompareWithBaseline_PATH + obj + "/Compare_Performance_" + obj + ".pdf", format='pdf')
    plt.show(block=False)
    plt.pause(3)


def plot_Runtime_results(task_set_number_range, method_names, obj):
    dataset_pd_obj, dataset_pd_runtime = ReadOptResultsAllMethod(method_names, obj, task_set_number_range)
    plt.figure()
    for i in range(len(method_names)):
        splot = sns.lineplot(data=dataset_pd_runtime, x="index", y=method_names[i], marker=marker_list[i],
                             color=color_list[i], label=method_names[i],
                             markersize=8)
    font_size = 15
    plt.xlabel("Task Number", fontsize=font_size)
    plt.ylabel("Running time (seconds)", fontsize=font_size)
    splot.set_ylim([1e-6, 1e3])
    splot.set(yscale="log")
    plt.legend()
    plt.grid(linestyle="--")
    plt.savefig(ROOT_CompareWithBaseline_PATH + obj + "/Compare_RunTime_" + obj + ".pdf", format='pdf')
    plt.show(block=False)
    plt.pause(3)


def draw_RT_results(task_set_number_range):
    method_names = ["InitialMethod", "TOM_BF", "TOM_WSkip", "TOM_Sort", "TOM_Sort_Bound"]
    plot_Obj_results(task_set_number_range, method_names, "ReactionTime")
    plot_Runtime_results(task_set_number_range, method_names, "ReactionTime")


def draw_DA_results(task_set_number_range):
    method_names = ["InitialMethod", "TOM_BF", "TOM_WSkip",
                    "TOM_Sort", "Martinez18", "TOM_Sort_Offset"]
    plot_Obj_results(task_set_number_range, method_names, "DataAge")
    plot_Runtime_results(task_set_number_range, method_names, "DataAge")
