#!/usr/bin/bash

task_number_list=( 5 10 20 30 )
files_per_node_list=( 200 500 200 100 )
TOTAL_TASK_NUMBER=3 # remember to -1

MinFileIndex=0
MaxFileIndex=999


perform_optimization() {
	task_number=$1
	files_per_node=$2
	echo "sbatch -J BatchRT_${task_number} --nodes=1 --ntasks-per-node=$(((MaxFileIndex+1)/files_per_node)) --cpus-per-task=1 SBatchRTMultiTask.sh $1 $MinFileIndex $files_per_node $MaxFileIndex"
	## test in local
	# bash SBatchRTMultiTask.sh $1 $MinFileIndex $files_per_node $MaxFileIndex
	## launc on ARC
	sbatch -J BatchRT_${task_number} --nodes=1 --ntasks-per-node=$(((MaxFileIndex+1)/files_per_node)) --cpus-per-task=1 SBatchRTMultiTask.sh $1 $MinFileIndex $files_per_node $MaxFileIndex
}


ROOT_PATH=/projects/rtss_let/LET_OPT
# ROOT_PATH=/home/dong/workspace/LET_OPT
cd $ROOT_PATH/Experiments/dong/multi_task

for task_number_index in $(seq 0 1 $TOTAL_TASK_NUMBER); do
	echo ${task_number_list[task_number_index]} ${files_per_node_list[task_number_index]}
	echo "Processing N=${task_number_list[task_number_index]}:"
	perform_optimization ${task_number_list[task_number_index]} ${files_per_node_list[task_number_index]}
done