#!/usr/bin/bash

# task_number_list=( 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 )
task_number_list=( 9 8 7 6 5 4 3 2 )
files_per_task_list=( 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 )
time_per_node_list=( 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 ) # in hour
TOTAL_TASK_NUMBER=7 # the max index start from 0
ThreadPerTask=1
MinFileIndex=0
MaxFileIndex=999


perform_optimization() {
	task_number=$1
	files_per_task=$2
	time_per_node=$3
	output_file_name=log/BatchSF_${task_number}-$(date +"%Y%m%d%H%M%S").out
	echo "sbatch -J SF_${task_number}_LET_Batch --nodes=1 --ntasks-per-node=1 --cpus-per-task=$(((MaxFileIndex+files_per_task)/files_per_task*ThreadPerTask)) --time ${time_per_node}:0:0 --output=${output_file_name} SBatchSFMultiTask.sh $1 $MinFileIndex $files_per_task $MaxFileIndex"
	## test in local
	# bash SBatchRTMultiTask.sh $1 $MinFileIndex $files_per_task $MaxFileIndex
	## launc on ARC
	sbatch -J SF_${task_number}_LET_Batch --nodes=1 --ntasks-per-node=1 --cpus-per-task=$(((MaxFileIndex+files_per_task)/files_per_task*ThreadPerTask)) --time ${time_per_node}:0:0 --output=${output_file_name} SBatchSFMultiTask.sh $1 $MinFileIndex $files_per_task $MaxFileIndex
}


# ROOT_PATH=/projects/rtss_let/LET_OPT
ROOT_PATH=/home/dongli/workspace/LET_OPT
cd $ROOT_PATH/Experiments/dong/multi_task
mkdir log

for task_number_index in $(seq 0 1 $TOTAL_TASK_NUMBER); do
	echo "Processing N=${task_number_list[task_number_index]} with $((files_per_task_list[task_number_index]*ThreadPerTask)) files per task:"
	perform_optimization ${task_number_list[task_number_index]} $((files_per_task_list[task_number_index]*ThreadPerTask)) $((time_per_node_list[task_number_index]*ThreadPerTask))
done
