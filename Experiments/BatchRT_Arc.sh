#!/usr/bin/bash

task_number_list=( 5 10 20 30 )
files_per_node_list=( 1000 500 200 100 )
TOTAL_TASK_NUMBER=3 # remember to -1

MinFileIndex=0
MaxFileIndex=999


perform_optimization() {
	task_number=$1
	files_per_node=$2
	for file_index in $(seq $MinFileIndex $files_per_node $MaxFileIndex); do
	end_index=$((file_index + files_per_node))
	echo "Processing N$1: file_index=$file_index:$end_index"
	# ./RunSingleFile/BatchRunSingle.run --N "$1" --begin "$file_index" --end "$end_index" &
	# sbatch BatchRT_Approx.sh $1 $file_index $end_index
	echo "sbatch BatchRT_Approx.sh $1 $file_index $end_index"
	done
	wait
}


ROOT_PATh=/projects/rtss_let/LET_OPT
cd $ROOT_PATh/Experiments

for task_number_index in $(seq 0 1 $TOTAL_TASK_NUMBER); do
echo ${task_number_list[task_number_index]} ${files_per_node_list[task_number_index]}
echo "Processing N=$task_number:"
	perform_optimization ${task_number_list[task_number_index]} ${files_per_node_list[task_number_index]}
done
