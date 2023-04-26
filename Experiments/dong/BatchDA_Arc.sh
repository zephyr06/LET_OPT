#!/usr/bin/bash

task_number_list=( 5 10 15 20 25 30 35 40 45 50 )
files_per_task_list=( 500 50 12 12 12 12 12 12 12 12 )
TOTAL_TASK_NUMBER=9 # remember to -1

MinFileIndex=0
MaxFileIndex=999


perform_optimization() {
	task_number=$1
	files_per_node=$2
	for file_index in $(seq $MinFileIndex $files_per_node $MaxFileIndex); do
	end_index=$((file_index + files_per_node))
	output_file_name=log/BatchDA_${task_number}_${file_index}_${end_index}-$(date +"%Y%m%d%H%M%S").out
	
    echo "Processing N$1: file_index=$file_index:$end_index"
	# ./RunSingleFile/BatchRunSingle.run --N "$1" --begin "$file_index" --end "$end_index" &
	# sbatch BatchRT_Approx.sh $1 $file_index $end_index
	echo "sbatch -J BatchDA_${task_number}_${file_index}_${end_index} --output=${output_file_name} SBatchDA.sh $1 $file_index $end_index"
    sbatch -J BatchDA_${task_number}_${file_index}_${end_index} --output=${output_file_name} SBatchDA.sh $1 $file_index $end_index
	done
	wait
}


ROOT_PATH=/projects/rtss_let/LET_OPT
cd $ROOT_PATH/Experiments/dong
midir log

for task_number_index in $(seq 0 1 $TOTAL_TASK_NUMBER); do
	echo ${task_number_list[task_number_index]} ${files_per_task_list[task_number_index]}
	echo "Processing N=${task_number_list[task_number_index]}:"
	perform_optimization ${task_number_list[task_number_index]} ${files_per_task_list[task_number_index]}
done
