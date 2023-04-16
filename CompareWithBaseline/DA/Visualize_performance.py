import argparse
import sys
import seaborn as sns
import matplotlib.pyplot as plt

sys.path.insert(1, '~/programming/LET_OPT/CompareWithBaseline')
from Read_ScheduleRes import *

parser = argparse.ArgumentParser()
parser.add_argument('--minTaskNumber', type=int, default=5,
                    help='Nmin')
parser.add_argument('--maxTaskNumber', type=int, default=30,
                    help='Nmax')
parser.add_argument('--title', type=str, default="EnergyPerformance",
                    help='tilte in produced figure')

args = parser.parse_args()
minTaskNumber = args.minTaskNumber
maxTaskNumber = args.maxTaskNumber
title = args.title

if __name__ == "__main__":
    task_set_number_range = [5]
    a, b = ReadOptResultsOneMethod("ReactionTime", "TOM_WSkip", task_set_number_range)
    print(a)
    print(b)
    method_names = ["InitialMethod", "TOM_BF", "TOM_WSkip", "TOM_Sort"]
    dataset_pd_obj, dataset_pd_runtime = ReadOptResultsAllMethod(method_names, "ReactionTime", task_set_number_range)
    dataset_pd_obj.plot()
    plt.show()
