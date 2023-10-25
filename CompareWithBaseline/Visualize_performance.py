from Read_ScheduleRes import *
import argparse
import sys
import seaborn as sns
import matplotlib.pyplot as plt
from PlotResults import *
from GlobalVariables import *

sys.path.insert(1, '~/programming/LET_OPT/CompareWithBaseline')
sys.path.insert(1, '~/workspace/LET_OPT/CompareWithBaseline')

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
    # task_set_number_range = [5, 10, 20, 30, 40]
    task_set_number_range = [5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21]

    # draw_RT_results(task_set_number_range)
    # draw_DA_results(task_set_number_range)
    # draw_DA_resultsOneChain(task_set_number_range)
    
    task_set_number_range = [2, 3, 4, 5, 6, 7, 8, 9]
    draw_SF_results(task_set_number_range, exclude_time_out=False)

