import pathlib
baseline_method_names = ["InitialMethod", "ImplicitCommunication", "TOM_BF", "TOM_WSkip",
                         "TOM_Sort", "Martinez18", "TOM_Sort_Offset", "TOM_Sort_Bound"]

marker_list = ["o", "*", "D", "^", "s", "D", "+"]
markersize_list = [8, 8, 10, 12, 8, 8, 8]   
# markersize_list = [6, 8, 6, 8, 6, 6, 8]
alpha_list = [0.4, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
color_list = ["#0084DB", "#ffa64d", "r", "y", "limegreen", "purple", "k"]

ROOT_PATH = "/home/zephyr/Programming/LET_OPT/"
ROOT_CompareWithBaseline_PATH = "/home/zephyr/Programming/LET_OPT/CompareWithBaseline/"

ROOT_PATH = str(pathlib.Path(__file__).parent.resolve().parent.resolve()) + "/"
ROOT_CompareWithBaseline_PATH = ROOT_PATH + "CompareWithBaseline/"
