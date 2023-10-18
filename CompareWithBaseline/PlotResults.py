from GlobalVariables import *
from Read_ScheduleRes import *
import sys
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np

sys.path.insert(1, '~/programming/LET_OPT/CompareWithBaseline')

# if output_file_name is empty, default name settings will be used
def plot_Obj_results(task_set_number_range, method_names, obj, exclude_time_out=False, output_file_name=""):
    dataset_pd_obj, dataset_pd_jitter, dataset_pd_runtime = ReadOptResultsAllMethod(
        method_names, obj, task_set_number_range, exclude_time_out)

    plt.figure()
    ax = plt.subplot(111)
    for i in range(len(method_names)):
        splot = sns.lineplot(data=dataset_pd_obj, x="index", y=method_names[i], marker=marker_map[method_names[i]],
                             color=color_map[method_names[i]
                                             ], label=baseline_method_labels[method_names[i]],
                             markersize=marker_size_map[method_names[i]])  # , alpha=alpha_list[i])
        
    if obj == "SensorFusion":
        plt.xlabel("Number of Source Tasks per Merge", fontsize=axis_label_font_size)
    else:
        plt.xlabel("Number of Tasks", fontsize=axis_label_font_size)
        
    if (obj == "ReactionTime"):
        plt.ylabel("Relative Gap of Reaction Time (%)", fontsize=axis_label_font_size)
    elif (obj == "DataAge" or obj == "DataAgeOneChain"):
        plt.ylabel("Relative Gap of Data Age (%)", fontsize=axis_label_font_size)
    elif obj == "SensorFusion":
        plt.ylabel("Relative Gap of Time Disparity (%)", fontsize=axis_label_font_size)

    if (obj == "ReactionTime" or obj == "DataAge" or obj == "DataAgeOneChain"):
        splot.set_ylim([-62, 5])
    
    ax.get_legend().remove()
    # ax.set_title(obj + " Objective Performance")

    if obj != "SensorFusion":
        splot.set_xticks([i for i in range(5,22,2)])
    plt.grid(linestyle="--")
    plt.xticks(fontsize=tick_font_size)
    plt.yticks(fontsize=tick_font_size)
    plt.tight_layout()
    if(output_file_name==""):
        plt.savefig(ROOT_CompareWithBaseline_PATH + obj +
                "/Compare_Performance_" + obj + ".pdf", format='pdf')
    else:
        plt.savefig(ROOT_CompareWithBaseline_PATH + obj +
                    "/Compare_Performance_" + output_file_name + ".pdf", format='pdf')
    plt.show(block=False)
    plt.pause(3)

# if output_file_name is empty, default name settings will be used
def plot_Runtime_results(task_set_number_range, method_names, obj, exclude_time_out=False, output_file_name=""):
    dataset_pd_obj, dataset_pd_jitter, dataset_pd_runtime = ReadOptResultsAllMethod(
        method_names, obj, task_set_number_range, exclude_time_out)
    plt.figure()
    ax = plt.subplot(111)
    for i in range(len(method_names)):
        splot = sns.lineplot(data=dataset_pd_runtime, x="index", y=method_names[i], marker=marker_map[method_names[i]],
                             color=color_map[method_names[i]
                                             ], label=baseline_method_labels[method_names[i]],
                             markersize=marker_size_map[method_names[i]])  # , alpha=alpha_list[i])
    if obj == "SensorFusion":
        plt.xlabel("Number of Source Tasks per Merge", fontsize=axis_label_font_size)
    else:
        plt.xlabel("Number of Tasks", fontsize=axis_label_font_size)
        
    plt.ylabel("Running Time (Seconds)", fontsize=axis_label_font_size)
    splot.set_ylim([1e-5, 5e3])
    splot.set(yscale="log")
    
    ax.get_legend().remove()
    # ax.set_title(obj + " Running Time")
    
    if obj != "SensorFusion":
        splot.set_xticks([i for i in range(5,22,2)])
    splot.set_yticks([10**i for i in range(-5,4)])
    plt.grid(linestyle="--")
    plt.xticks(fontsize=tick_font_size)
    plt.yticks(fontsize=tick_font_size)
    plt.tight_layout()

    if(output_file_name==""):
        plt.savefig(ROOT_CompareWithBaseline_PATH + obj +
                "/Compare_RunTime_" + obj + ".pdf", format='pdf')
    else:
        plt.savefig(ROOT_CompareWithBaseline_PATH + obj +
                "/Compare_RunTime_" + output_file_name + ".pdf", format='pdf')
    plt.show(block=False)
    plt.pause(3)
    

# if output_file_name is empty, default name settings will be used
def plot_Jitter_results(task_set_number_range, method_names, obj, exclude_time_out=False, output_file_name=""):
    dataset_pd_obj, dataset_pd_jitter, dataset_pd_runtime = ReadOptResultsAllMethod(
        method_names, obj, task_set_number_range, exclude_time_out)

    plt.figure()
    ax = plt.subplot(111)
    for i in range(len(method_names)):
        splot = sns.lineplot(data=dataset_pd_jitter, x="index", y=method_names[i], marker=marker_map[method_names[i]],
                             color=color_map[method_names[i]
                                             ], label=baseline_method_labels[method_names[i]],
                             markersize=marker_size_map[method_names[i]])  # , alpha=alpha_list[i])
        
    if obj == "SensorFusion":
        plt.xlabel("Number of Source Tasks per Merge", fontsize=axis_label_font_size)
    else:
        plt.xlabel("Number of Tasks", fontsize=axis_label_font_size)
        
    if (obj == "ReactionTime" or obj == "DataAge" or obj == "DataAgeOneChain"):
        plt.ylabel("Relative Gap of Jitter (%)", fontsize=axis_label_font_size)
    elif obj == "SensorFusion":
        plt.ylabel("Relative Gap of Jitter (%)", fontsize=axis_label_font_size)
    
    ax.get_legend().remove()
    # ax.set_title(obj + " Jitter Performance")

    if obj != "SensorFusion":
        splot.set_xticks([i for i in range(5,22,2)])
    plt.grid(linestyle="--")
    plt.xticks(fontsize=tick_font_size)
    plt.yticks(fontsize=tick_font_size)
    plt.tight_layout()
    if(output_file_name==""):
        plt.savefig(ROOT_CompareWithBaseline_PATH + obj +
                "/Compare_Jitter_" + obj + ".pdf", format='pdf')
    else:
        plt.savefig(ROOT_CompareWithBaseline_PATH + obj +
                    "/Compare_Jitter_" + output_file_name + ".pdf", format='pdf')
    plt.show(block=False)
    plt.pause(3)


def draw_RT_results(task_set_number_range):
    method_names = ["InitialMethod", "ImplicitCommunication",
                    "TOM_BF", "TOM_WSkip", "TOM_Sort"]
    plot_Obj_results(task_set_number_range, method_names, "ReactionTime")
    plot_Runtime_results(task_set_number_range, method_names, "ReactionTime")


def draw_DA_resultsOneChain(task_set_number_range):
    method_names = ["InitialMethod", "ImplicitCommunication", "Martinez18",
                    "TOM_Sort_Offset", "Bardatsch16", "TOM_BF", "TOM_WSkip", "TOM_Sort"]
    plot_Obj_results(task_set_number_range, method_names, "DataAgeOneChain", output_file_name="DataAgeOneChain")
    plot_Runtime_results(task_set_number_range, method_names, "DataAgeOneChain", output_file_name="DataAgeOneChain")

def draw_DA_results(task_set_number_range):
    method_names = ["InitialMethod", "ImplicitCommunication",
                    "TOM_Sort_Offset", "Bardatsch16", "TOM_BF", "TOM_WSkip", "TOM_Sort"]
    plot_Obj_results(task_set_number_range, method_names, "DataAge")
    plot_Runtime_results(task_set_number_range, method_names, "DataAge")



def draw_SF_results(task_set_number_range, exclude_time_out=False):
    method_names = ["InitialMethod",
                    "ImplicitCommunication", "TOM_BF", "TOM_WSkip"]
    plot_Obj_results(task_set_number_range, method_names,
                     "SensorFusion", exclude_time_out)
    plot_Jitter_results(task_set_number_range, method_names,
                     "SensorFusion", exclude_time_out)
    plot_Runtime_results(task_set_number_range, method_names,
                         "SensorFusion", exclude_time_out)

def print_SF_table_results(task_set_number_range, exclude_time_out=False):
    # draw_SF_results(task_set_number_range, exclude_time_out=False)

    method_names = ["InitialMethod",
                    "ImplicitCommunication", "TOM_BF", "TOM_WSkip"]
    dataset_pd_obj, dataset_pd_runtime = ReadOptResultsAllMethod(
        method_names, "SensorFusion", task_set_number_range, exclude_time_out)
    pd.set_option("display.precision", 8)
    pd.set_option('display.precision', 3)
    print("######## SF runtime, 'index' is the maximum of source tasks  ########")
    print(dataset_pd_runtime)
    # pd.options.display.float_format = "{:,.2f}".format
    print("######## SF object, 'index' is the maximum of source tasks  ########")
    print(dataset_pd_obj)


    file = open(ROOT_CompareWithBaseline_PATH + "SensorFusion/stat.txt", "w")
    file.write("\n######## SF object, 'index' is the maximum of source tasks  ########\n")
    file.write(dataset_pd_obj.to_string())
    file.write("\n######## SF runtime, 'index' is the maximum of source tasks  ########\n")
    file.write(dataset_pd_runtime.to_string())
    file.close()