#!/usr/bin/bash

task_number_list=( 50 48 46 44 42 40 38 36 34 32 30 28 26 24 22 20 18 16 14 12 10 8 6 4 )
files_per_task_list=( 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 25 25 25 100)
time_per_node_list=( 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 ) # in hour
TOTAL_TASK_NUMBER=23 # the max index start from 0

MinFileIndex=0
MaxFileIndex=999


perform_optimization() {
	task_number=$1
	files_per_task=$2
	time_per_node=$3
	output_file_name=log/BatchDA_${task_number}-$(date +"%Y%m%d%H%M%S").out
	echo "sbatch -J BatchDA_${task_number} --nodes=1 --ntasks-per-node=$(((MaxFileIndex+files_per_task)/files_per_task)) --cpus-per-task=1 --time=${time_per_node}:0:0 --output=${output_file_name} SBatchDAMultiTask.sh $1 $MinFileIndex $files_per_task $MaxFileIndex"
	## test in local
	# bash SBatchDAMultiTask.sh $1 $MinFileIndex $files_per_task $MaxFileIndex
	## launc on ARC
	sbatch -J BatchDA_${task_number} --nodes=1 --ntasks-per-node=$(((MaxFileIndex+files_per_task)/files_per_task)) --cpus-per-task=1 --time=${time_per_node}:0:0 --output=${output_file_name} SBatchDAMultiTask.sh $1 $MinFileIndex $files_per_task $MaxFileIndex
}


ROOT_PATH=/projects/rtss_let/LET_OPT
# ROOT_PATH=/home/dong/workspace/LET_OPT
cd $ROOT_PATH/Experiments/dong/multi_task
mkdir log

for task_number_index in $(seq 0 1 $TOTAL_TASK_NUMBER); do
	echo "Processing N=${task_number_list[task_number_index]} with ${files_per_task_list[task_number_index]} files per task:"
	perform_optimization ${task_number_list[task_number_index]} ${files_per_task_list[task_number_index]} ${time_per_node_list[task_number_index]}
done
