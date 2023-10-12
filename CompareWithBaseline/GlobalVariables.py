import pathlib
baseline_method_names = ["InitialMethod", "ImplicitCommunication", "Martinez18",
                         "TOM_Sort_Offset", "Bardatsch16", "TOM_BF", "TOM_WSkip", "TOM_Sort"]

baseline_method_labels = {"InitialMethod": "StanLET",
                          "ImplicitCommunication": "Implicit",
                          "TOM_BF": "iLET_BF",
                          "TOM_WSkip": "iLET_Skip_Infeasible",
                          "TOM_Sort": "iLET_Efficient",
                          "Martinez18": "Martinez18",
                          "TOM_Sort_Offset": "Martinez18_GLP_Efficient",
                          "Bardatsch16": "Bradatsch16"}

marker_map = {"InitialMethod": "o",
              "ImplicitCommunication": "*",
              "TOM_BF": "D",
              "TOM_WSkip": "^",
              "TOM_Sort": "s",
              "Martinez18": "D",
              "TOM_Sort_Offset": "P",
              "Bardatsch16": "X"}
marker_size_map = {"InitialMethod": 6,
                   "ImplicitCommunication": 12,
                   "TOM_BF": 6,
                   "TOM_WSkip": 8,
                   "TOM_Sort": 6,
                   "Martinez18": 8,
                   "TOM_Sort_Offset": 8,
                   "Bardatsch16": 4}
# marker_size_map = {"InitialMethod": 6,
#                    "ImplicitCommunication": 10,
#                    "TOM_BF": 6,
#                    "TOM_WSkip": 8,
#                    "TOM_Sort": 6,
#                    "Martinez18": 6,
#                    "TOM_Sort_Offset": 6,
#                    "Bardatsch16": 3}
color_map = {"InitialMethod": "#0084DB",
             "ImplicitCommunication": "#ffa64d",
             "TOM_BF": "r",
             "TOM_WSkip": "y",
             "TOM_Sort": "limegreen",
             "Martinez18": "purple",
             "TOM_Sort_Offset": "k",
             "Bardatsch16": "#00FFFF"}

# marker_list = ["o", "*", "D", "^", "s", "D", "P", "X"]
# markersize_list = [8, 12, 10, 12, 8, 8, 8, 4]
# # markersize_list = [6, 10, 6, 8, 6, 6, 6, 3]
# alpha_list = [1, 1, 1, 1, 1, 1, 1, 1]
# color_list = ["#0084DB", "#ffa64d", "r", "y",
#               "limegreen", "purple", "k", "#00FFFF"]

# ROOT_PATH = "/home/zephyr/Programming/LET_OPT/"
# ROOT_CompareWithBaseline_PATH = "/home/zephyr/Programming/LET_OPT/CompareWithBaseline/"

ROOT_PATH = str(pathlib.Path(__file__).parent.resolve().parent.resolve()) + "/"
ROOT_CompareWithBaseline_PATH = ROOT_PATH + "CompareWithBaseline/"

verbose_mode = False
