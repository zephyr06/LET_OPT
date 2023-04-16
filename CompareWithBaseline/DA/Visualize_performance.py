import argparse
import sys

sys.path.insert(1, '~/programming/LET_OPT/CompareWithBaseline')
from read_res import ReadOptResults
import baseline_methods_name

baseline_method_names = baseline_methods_name.baseline_method_names
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
    print(baseline_methods_name.baseline_method_names)
    res_vec = ReadOptResults()
    a = 1
