import pathlib
baseline_method_names = ["InitialMethod", "ImplicitCommunication", "Martinez18", 
                         "TOM_Sort_Offset", "Bardatsch16", "Maia23", "TOM_BF", "TOM_WSkip", "TOM_Sort"]

baseline_method_labels = {"InitialMethod": "DefLET",
                          "ImplicitCommunication": "Implicit",
                          "Maia23": "Maia23",
                          "TOM_BF": "fLET_BF",
                          "TOM_WSkip": "fLET_backtracking",
                          "TOM_Sort": "fLET_acktracking+",
                          "Martinez18": "Martinez18",
                          "TOM_Sort_Offset": "Martinez18_Alg1",
                          "Bardatsch16": "Bradatsch16",
                          "TOM_BF_No_Jitter_Weight": "fLET_BF \u03C9=0",
                          "TOM_WSkip_No_Jitter_Weight": "fLET_backtracking \u03C9=0"
                          }

marker_map = {"InitialMethod": "o",
              "ImplicitCommunication": "*",
              "Maia23": "v",
              "TOM_BF": "D",
              "TOM_WSkip": "^",
              "TOM_Sort": "s",
              "Martinez18": "D",
              "TOM_Sort_Offset": "P",
              "Bardatsch16": "X",
              "TOM_BF_No_Jitter_Weight": "D",
              "TOM_WSkip_No_Jitter_Weight": "^"}
marker_size_map = {"InitialMethod": 8,
                   "ImplicitCommunication": 12,
                    "Maia23": 10,
                   "TOM_BF": 9,
                   "TOM_WSkip": 10,
                   "TOM_Sort": 7,
                   "Martinez18": 8,
                   "TOM_Sort_Offset": 8,
                   "Bardatsch16": 7,
                   "TOM_BF_No_Jitter_Weight": 9,
                   "TOM_WSkip_No_Jitter_Weight": 10}
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
             "Maia23": "#b544c7",
             "TOM_BF": "r",
             "TOM_WSkip": "y",
             "TOM_Sort": "limegreen",
             "Martinez18": "purple",
             "TOM_Sort_Offset": "k",
             "Bardatsch16": "#00FFFF",
             "TOM_BF_No_Jitter_Weight": "#ff4d82",
             "TOM_WSkip_No_Jitter_Weight": "#ab7c0f"}

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

axis_label_font_size=17
tick_font_size=12

verbose_mode = False
