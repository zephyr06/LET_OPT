#!/usr/bin/bash

# ************** Adjust settings there **************
N=30
ROOT_PATH="/home/zephyr/Programming/LET_OPT"
# ***************************************************
cd $ROOT_PATH/release
make GenerateTaskSet -j4
./tests/GenerateTaskSet --taskSetNumber 1000 --task_number_in_tasksets $N --per_core_utilization 0.4 --outDir "TaskData/N$N/" --parallelismFactor 0.2
./RunSingleFile/BatchLETRunSingle --N $N
