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
    # task_set_number_range = [5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21]
    task_set_number_range = [5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 
                             31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50]
    # task_set_number_range = [5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 
    #                          31, 32, 33, 34, 35, 36, 37, 38, 39, 40]
    # task_set_number_range = [5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39]
    # task_set_number_range = [5, 10, 15, 20, 25, 30, 35, 40]
    # task_set_number_range = [15, 16, 17, 18, 19, 20, 21]
    # task_set_number_range = [5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17]
    # task_set_number_range = [4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50]

    # draw_RT_results(task_set_number_range)
    # draw_DA_results(task_set_number_range)
    # draw_DA_resultsOneChain(task_set_number_range)
    
    # task_set_number_range = [2, 3, 4, 5, 6, 7, 8, 9]
    # task_set_number_range = [4, 5, 6, 7, 8, 9]
    task_set_number_range = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    draw_SF_results(task_set_number_range, exclude_time_out=False)

    # max_source_number_range = [2, 3, 4, 5]
    # print_SF_table_results(max_source_number_range, exclude_time_out=False)
