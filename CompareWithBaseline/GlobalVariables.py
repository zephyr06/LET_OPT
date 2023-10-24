import pathlib
baseline_method_names = ["InitialMethod", "ImplicitCommunication", "Martinez18", 
                         "Bardatsch16", "Maia23", "TOM_BF", "TOM_WSkip", "TOM_Sort", "TOM_Sort_Maia23"]

baseline_method_labels = {"InitialMethod": "DefLET",
                          "ImplicitCommunication": "Implicit",
                          "Maia23": "Maia23",
                          "TOM_BF": "fLET_GCP_Enum",
                          "TOM_WSkip": "fLET_GCP_Ineq_Filter",
                          "TOM_Sort": "fLET_GCP_SymbOpt",
                          "Martinez18": "Martinez18",
                          "TOM_Sort_Offset": "Martinez18_Alg1",
                          "Bardatsch16": "Bradatsch16",
                          "TOM_BF_No_Jitter_Weight": "fLET_GCP_Enum \u03C9=0",
                          "TOM_WSkip_No_Jitter_Weight": "fLET_GCP_Ineq_Filter \u03C9=0",
                          "TOM_WSkip_Maia23_No_Jitter_Weight": "fLET_GCP_Extra \u03C9=0",
                          "TOM_Sort_Maia23": "fLET_GCP_Extra",
                          "TOM_WSkip_Maia23": "fLET_GCP_Extra"
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
              "TOM_WSkip_No_Jitter_Weight": "^",
              "TOM_WSkip_Maia23_No_Jitter_Weight": "s",
              "TOM_Sort_Maia23": "s",
              "TOM_WSkip_Maia23": "s"
              }
marker_size_map = {"InitialMethod": 9,
                   "ImplicitCommunication": 12,
                    "Maia23": 12,
                   "TOM_BF": 8,
                   "TOM_WSkip": 12,
                   "TOM_Sort": 7,
                   "Martinez18": 8,
                   "TOM_Sort_Offset": 8,
                   "Bardatsch16": 7,
                   "TOM_BF_No_Jitter_Weight": 8,
                   "TOM_WSkip_No_Jitter_Weight": 12,
                   "TOM_WSkip_Maia23_No_Jitter_Weight": 7,
                   "TOM_Sort_Maia23": 7,
                   "TOM_WSkip_Maia23": 7
                   }
color_map = {"InitialMethod": "#00b58e",
             "ImplicitCommunication": "#ffa64d",
             "Maia23": "#b544c7",
             "TOM_BF": "r",
             "TOM_WSkip": "y",
             "TOM_Sort": "limegreen",
             "Martinez18": "purple",
             "TOM_Sort_Offset": "k",
             "Bardatsch16": "#eb4034",
             "TOM_BF_No_Jitter_Weight": "#ff4d82",
             "TOM_WSkip_No_Jitter_Weight": "#ab7c0f",
             "TOM_WSkip_Maia23_No_Jitter_Weight": "#0084DB",
             "TOM_Sort_Maia23": "#0556e3",
             "TOM_WSkip_Maia23": "#0556e3"
            }

linestyle_map = {"InitialMethod": "solid",
                "ImplicitCommunication": "solid",
                "Maia23": "solid",
                "TOM_BF": "solid",
                "TOM_WSkip": "solid",
                "TOM_Sort": "solid",
                "Martinez18": "solid",
                "TOM_Sort_Offset": "solid",
                "Bardatsch16": "solid",
                "TOM_BF_No_Jitter_Weight": "dashdot",
                "TOM_WSkip_No_Jitter_Weight": "dashdot",
                "TOM_WSkip_Maia23_No_Jitter_Weight": "dashdot",
                "TOM_Sort_Maia23": "solid",
                "TOM_WSkip_Maia23": "solid"
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
