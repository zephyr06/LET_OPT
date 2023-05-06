import pathlib
baseline_method_names = ["InitialMethod", "ImplicitCommunication", "TOM_BF", "TOM_WSkip",
                         "TOM_Sort", "Martinez18", "TOM_Sort_Offset", "TOM_Sort_Bound"]
baseline_method_labels = {"InitialMethod": "StanLET",
                          "ImplicitCommunication": "ImplicitCommunication",
                          "TOM_BF": "eLET_BF",
                          "TOM_WSkip": "eLET_WSkip",
                          "TOM_Sort": "eLET_Sort",
                          "Martinez18": "Martinez18",
                          "TOM_Sort_Offset": "eLET_Sort_Offset",
                          "TOM_Sort_Bound": "TOM_Sort_Bound"}

marker_list = ["o", "*", "D", "^", "s", "D", "+"]
markersize_list = [8, 12, 10, 12, 8, 8, 8]
# markersize_list = [6, 10, 6, 8, 6, 6, 8]
alpha_list = [0.4, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
color_list = ["#0084DB", "#ffa64d", "r", "y", "limegreen", "purple", "k"]

ROOT_PATH = "/home/zephyr/Programming/LET_OPT/"
ROOT_CompareWithBaseline_PATH = "/home/zephyr/Programming/LET_OPT/CompareWithBaseline/"

ROOT_PATH = str(pathlib.Path(__file__).parent.resolve().parent.resolve()) + "/"
ROOT_CompareWithBaseline_PATH = ROOT_PATH + "CompareWithBaseline/"


global sensor_fusion_info_print_list
sensor_fusion_info_print_list = []