import pathlib
baseline_method_names = ["InitialMethod", "Martinez18", "Bardatsch16", 
                         "ImplicitCommunication", "Maia23", "TOM_BF", "TOM_WSkip", "TOM_Sort", "TOM_Sort_Maia23"]

baseline_method_labels = {"InitialMethod": "DefLET",
                          "ImplicitCommunication": "Implicit",
                          "Maia23": "Maia23",
                          "TOM_BF": "fLET_GCP_Enum",
                          "TOM_WSkip": "fLET_GCP_Backtracking_LP",
                          "TOM_Sort": "fLET_GCP_SymbOpt",
                          "Martinez18": "Martinez18",
                          "TOM_Sort_Offset": "Martinez18_Alg1",
                          "Bardatsch16": "Bradatsch16",
                          "TOM_BF_No_Jitter_Weight": "fLET_GCP_Enum \u03C9=0",
                          "TOM_WSkip_No_Jitter_Weight": "fLET_GCP_Backtracking_LP \u03C9=0",
                          "TOM_WSkip_Maia23_No_Jitter_Weight": "fLET_GCP_Extra \u03C9=0",
                          "TOM_Sort_Maia23": "fLET_GCP_Extra",
                          "TOM_WSkip_Maia23": "fLET_GCP_Extra"
                          }
marker_map = {"InitialMethod": "o",
              "ImplicitCommunication": "*",
              "Maia23": "v",
              "TOM_BF": "d",
              "TOM_WSkip": "s",
              "TOM_Sort": "P",
              "Martinez18": "X",
              "TOM_Sort_Offset": "P",
              "Bardatsch16": "p",
              "TOM_BF_No_Jitter_Weight": "d",
              "TOM_WSkip_No_Jitter_Weight": "s",
              "TOM_WSkip_Maia23_No_Jitter_Weight": "o",
              "TOM_Sort_Maia23": "o",
              "TOM_WSkip_Maia23": "o"
              }
marker_size_map = {"InitialMethod": 9,
                   "ImplicitCommunication": 12,
                   "Maia23": 10,
                   "TOM_BF": 10,
                   "TOM_WSkip": 8,
                   "TOM_Sort": 8,
                   "Martinez18": 7,
                   "TOM_Sort_Offset": 7,
                   "Bardatsch16": 7,
                   "TOM_BF_No_Jitter_Weight": 10,
                   "TOM_WSkip_No_Jitter_Weight": 8,
                   "TOM_WSkip_Maia23_No_Jitter_Weight": 7,
                   "TOM_Sort_Maia23": 7,
                   "TOM_WSkip_Maia23": 7
                   }
color_map = {"InitialMethod": "#9bb324",
             "Martinez18": "#a0b1ba",
             "Bardatsch16": "#a6761d",
             "ImplicitCommunication": "#ffc61e",
             "Maia23": "#f28522",
             "TOM_BF": "#ff1f5b",
             "TOM_WSkip": "#af58ba",
             "TOM_Sort": "#009ade",
             "TOM_Sort_Maia23": "#00cd6c",
             "TOM_WSkip_Maia23": "#00cd6c",
             "TOM_BF_No_Jitter_Weight": "#ff1f5b",
             "TOM_WSkip_No_Jitter_Weight": "#af58ba",
             "TOM_WSkip_Maia23_No_Jitter_Weight": "#00cd6c",
             "TOM_Sort_Offset": "k"
            }
# color_map = {"InitialMethod": "#05f2bf",
#              "ImplicitCommunication": "y",
#              "Maia23": "r",
#              "TOM_BF": "#b544c7",
#              "TOM_WSkip": "#0556e3",
#              "TOM_Sort": "limegreen",
#              "Martinez18": "k",
#              "Bardatsch16": "#ff4d82",
#              "TOM_Sort_Maia23": "#ff9100",
#              "TOM_WSkip_Maia23": "#ff9100",
#              "TOM_BF_No_Jitter_Weight": "#b544c7",
#              "TOM_WSkip_No_Jitter_Weight": "#0556e3",
#              "TOM_WSkip_Maia23_No_Jitter_Weight": "#ff9100",
#              "TOM_Sort_Offset": "k"
#             }

linestyle_map = {"InitialMethod": "solid",
                "ImplicitCommunication": "solid",
                "Maia23": "solid",
                "TOM_BF": "solid",
                "TOM_WSkip": "solid",
                "TOM_Sort": "solid",
                "Martinez18": "solid",
                "TOM_Sort_Offset": "solid",
                "Bardatsch16": "solid",
                "TOM_BF_No_Jitter_Weight": "dashed",
                "TOM_WSkip_No_Jitter_Weight": "dashed",
                "TOM_WSkip_Maia23_No_Jitter_Weight": "dashed",
                "TOM_Sort_Maia23": "solid",
                "TOM_WSkip_Maia23": "solid"
                }


alpha_map = {"InitialMethod": 1,
                "ImplicitCommunication": 0.9,
                "Maia23": 1,
                "TOM_BF": 1,
                "TOM_WSkip": 0.9,
                "TOM_Sort": 0.9,
                "Martinez18": 1,
                "TOM_Sort_Offset": 1,
                "Bardatsch16": 1,
                "TOM_BF_No_Jitter_Weight": 1,
                "TOM_WSkip_No_Jitter_Weight": 0.9,
                "TOM_WSkip_Maia23_No_Jitter_Weight": 0.9,
                "TOM_Sort_Maia23": 0.9,
                "TOM_WSkip_Maia23": 0.9,
                }

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
show_legend_mode = False
