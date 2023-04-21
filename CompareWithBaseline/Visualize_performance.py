import argparse
import sys
import seaborn as sns
import matplotlib.pyplot as plt
from PlotResults import *

sys.path.insert(1, '~/programming/LET_OPT/CompareWithBaseline')
sys.path.insert(1, '~/workspace/LET_OPT/CompareWithBaseline')
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
    task_set_number_range = [5, 10, 15, 20, 25, 30, 35, 40, 45, 50]
    # a, b = ReadOptResultsOneMethod("TOM_WSkip", "ReactionTime", task_set_number_range)
    # print(a)
    # print(b)
    draw_RT_results(task_set_number_range)
    # draw_DA_results(task_set_number_range)
