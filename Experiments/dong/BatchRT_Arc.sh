#!/usr/bin/bash

task_number_list=( 5 10 20 30 )
files_per_node_list=( 200 500 200 100 )
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
	echo "sbatch -J BatchRT_${task_number}_${file_index}_${end_index} SBatchRT.sh $1 $file_index $end_index"
    sbatch -J BatchRT_${task_number}_${file_index}_${end_index} SBatchRT.sh $1 $file_index $end_index
	done
	wait
}


ROOT_PATH=/projects/rtss_let/LET_OPT
cd $ROOT_PATH/Experiments/dong

for task_number_index in $(seq 0 1 $TOTAL_TASK_NUMBER); do
echo ${task_number_list[task_number_index]} ${files_per_node_list[task_number_index]}
echo "Processing N=${task_number_list[task_number_index]}:"
	perform_optimization ${task_number_list[task_number_index]} ${files_per_node_list[task_number_index]}
done
