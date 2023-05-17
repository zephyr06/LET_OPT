from GlobalVariables import *
from Read_ScheduleRes import *
import sys
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np

sys.path.insert(1, '~/programming/LET_OPT/CompareWithBaseline')


def plot_Obj_results(task_set_number_range, method_names, obj, exclude_time_out=False):
    dataset_pd_obj, dataset_pd_runtime = ReadOptResultsAllMethod(
        method_names, obj, task_set_number_range, exclude_time_out)

    plt.figure()
    ax = plt.subplot(111)
    for i in range(len(method_names)):
        splot = sns.lineplot(data=dataset_pd_obj, x="index", y=method_names[i], marker=marker_map[method_names[i]],
                             color=color_map[method_names[i]
                                             ], label=baseline_method_labels[method_names[i]],
                             markersize=marker_size_map[method_names[i]])  # , alpha=alpha_list[i])
    font_size = 15
    plt.xlabel("Task Number", fontsize=font_size)
    plt.ylabel("Relative performance gap(%)", fontsize=font_size)
    # if obj != "SensorFusion":
    #     splot.set_ylim([0.15, 1.05])
    # else:
    #     TODO set a suitable range for sensor fusion
    #     splot.set_ylim([0.15, 2.05])

    # # Shrink current axis's height by 10% at the botom
    # box = ax.get_position()
    # ax.set_position([box.x0, box.y0 + box.height * 0.21,
    #                  box.width, box.height * 0.9])
    # handles, labels = ax.get_legend_handles_labels()
    # handles = np.concatenate((handles[::3],handles[1::3],handles[2::3]),axis=0)
    # labels = np.concatenate((labels[::3],labels[1::3],labels[2::3]),axis=0)
    # if obj == "DataAge":
    #     plt.legend(handles, labels, loc='lower center', bbox_to_anchor=(0.5, -0.39), ncol=3)
    # elif obj == "ReactionTime":
    #     plt.legend(handles, labels, loc='lower center', bbox_to_anchor=(0.5, -0.32), ncol=3)
    # elif obj == "SensorFusion":
    #     plt.legend(handles, labels, loc='lower center', bbox_to_anchor=(0.5, -0.32), ncol=3)
    # else:
    #     plt.legend()
    ax.get_legend().remove()

    plt.grid(linestyle="--")
    plt.savefig(ROOT_CompareWithBaseline_PATH + obj +
                "/Compare_Performance_" + obj + ".pdf", format='pdf')
    plt.show(block=False)
    plt.pause(3)


def plot_Runtime_results(task_set_number_range, method_names, obj, exclude_time_out=False):
    dataset_pd_obj, dataset_pd_runtime = ReadOptResultsAllMethod(
        method_names, obj, task_set_number_range, exclude_time_out)
    plt.figure()
    ax = plt.subplot(111)
    for i in range(len(method_names)):
        splot = sns.lineplot(data=dataset_pd_runtime, x="index", y=method_names[i], marker=marker_map[method_names[i]],
                             color=color_map[method_names[i]
                                             ], label=baseline_method_labels[method_names[i]],
                             markersize=marker_size_map[method_names[i]])  # , alpha=alpha_list[i])
    font_size = 15
    plt.xlabel("Task Number", fontsize=font_size)
    plt.ylabel("Running time (seconds)", fontsize=font_size)
    splot.set_ylim([1e-6, 1e3])
    splot.set(yscale="log")

    # # Shrink current axis's height by 10% at the botom
    # box = ax.get_position()
    # ax.set_position([box.x0, box.y0 + box.height * 0.21,
    #                  box.width, box.height * 0.9])
    # handles, labels = ax.get_legend_handles_labels()
    # handles = np.concatenate((handles[::3],handles[1::3],handles[2::3]),axis=0)
    # labels = np.concatenate((labels[::3],labels[1::3],labels[2::3]),axis=0)
    # if obj == "DataAge":
    #     plt.legend(handles, labels, loc='lower center', bbox_to_anchor=(0.5, -0.39), ncol=3)
    # elif obj == "ReactionTime":
    #     plt.legend(handles, labels, loc='lower center', bbox_to_anchor=(0.5, -0.32), ncol=3)
    # elif obj == "SensorFusion":
    #     plt.legend(handles, labels, loc='lower center', bbox_to_anchor=(0.5, -0.32), ncol=3)
    # else:
    #     plt.legend()
    ax.get_legend().remove()
    
    plt.grid(linestyle="--")
    plt.savefig(ROOT_CompareWithBaseline_PATH + obj +
                "/Compare_RunTime_" + obj + ".pdf", format='pdf')
    plt.show(block=False)
    plt.pause(3)


def draw_RT_results(task_set_number_range):
    method_names = ["InitialMethod", "ImplicitCommunication",
                    "TOM_BF", "TOM_WSkip", "TOM_Sort"]
    plot_Obj_results(task_set_number_range, method_names, "ReactionTime")
    plot_Runtime_results(task_set_number_range, method_names, "ReactionTime")


def draw_DA_results(task_set_number_range):
    method_names = ["InitialMethod", "ImplicitCommunication", "Martinez18", "TOM_Sort_Offset",
                    "Bardatsch16", "TOM_BF", "TOM_WSkip", "TOM_Sort"]
    plot_Obj_results(task_set_number_range, method_names, "DataAge")
    plot_Runtime_results(task_set_number_range, method_names, "DataAge")


def draw_SF_results(task_set_number_range, exclude_time_out=False):
    method_names = ["InitialMethod",
                    "ImplicitCommunication", "TOM_BF", "TOM_WSkip"]
    plot_Obj_results(task_set_number_range, method_names,
                     "SensorFusion", exclude_time_out)
    plot_Runtime_results(task_set_number_range, method_names,
                         "SensorFusion", exclude_time_out)
