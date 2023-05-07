import pathlib
baseline_method_names = ["InitialMethod", "ImplicitCommunication", "TOM_BF", "TOM_WSkip",
                         "TOM_Sort", "Martinez18", "TOM_Sort_Offset", "Bardatsch16"]
baseline_method_labels = {"InitialMethod": "StanLET",
                          "ImplicitCommunication": "Implicit",
                          "TOM_BF": "eLET_BF",
                          "TOM_WSkip": "eLET_WSkip",
                          "TOM_Sort": "eLET_Sort",
                          "Martinez18": "Martinez18",
                          "TOM_Sort_Offset": "eLET_Sort_Offset",
                          "Bardatsch16": "Bardatsch16"}

marker_list = ["o", "*", "D", "^", "s", "D", "P", "X"]
markersize_list = [8, 12, 10, 12, 8, 8, 8, 4]
# markersize_list = [6, 10, 6, 8, 6, 6, 6, 3]
alpha_list = [1, 1, 1, 1, 1, 1, 1, 1]
color_list = ["#0084DB", "#ffa64d", "r", "y", "limegreen", "purple", "k", "#00FFFF"]

# ROOT_PATH = "/home/zephyr/Programming/LET_OPT/"
# ROOT_CompareWithBaseline_PATH = "/home/zephyr/Programming/LET_OPT/CompareWithBaseline/"

ROOT_PATH = str(pathlib.Path(__file__).parent.resolve().parent.resolve()) + "/"
ROOT_CompareWithBaseline_PATH = ROOT_PATH + "CompareWithBaseline/"

verbose_mode = False
