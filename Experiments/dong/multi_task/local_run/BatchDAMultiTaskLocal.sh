#!/usr/bin/bash

task_number_list=( 50 45 40 35 30 25 20 15 10 5 )
files_per_task_list=( 125 125 125 125 125 125 125 125 125 125 )
TOTAL_TASK_NUMBER=9 # the max index start from 0

MinFileIndex=0
MaxFileIndex=999


## change directory and load modules ##
# ROOT_PATH=/projects/rtss_let/LET_OPT
ROOT_PATH=/home/dong/workspace/LET_OPT
cd $ROOT_PATH/Experiments/dong/multi_task/local_run

perform_optimization() {
	N=$1
	files_per_task=$2

	## Launch multiple threads for one task nunmber
	for file_index in $(seq $MinFileIndex $files_per_task $MaxFileIndex); do
		end_index=$((file_index + files_per_task))
		echo "$ROOT_PATH/release/RunSingleFile/BatchDAWithMartRunAll.run --N $N --begin $file_index --end $end_index &"
		$ROOT_PATH/release/RunSingleFile/BatchDAWithMartRunAll.run --N $N --begin $file_index --end $end_index &
	done

	## Wait for all of the background tasks to finish
	wait
}

for task_number_index in $(seq 0 1 $TOTAL_TASK_NUMBER); do
	echo "Processing N=${task_number_list[task_number_index]} with ${files_per_task_list[task_number_index]} files per thread:"
	perform_optimization ${task_number_list[task_number_index]} ${files_per_task_list[task_number_index]}
done

exit 0