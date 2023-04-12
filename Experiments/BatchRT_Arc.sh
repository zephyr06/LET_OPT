#!/usr/bin/bash

MinTaskNumber=5
MaxTaskNumber=5

MinFileIndex=0
MaxFileIndex=999
FILES_PER_CPU=500


cd ~/Programming/LET_OPT/release
cmake -DCMAKE_BUILD_TYPE=RELEASE ..
make BatchRunSingle.run 
cd ~/Programming/LET_OPT/Experiments

perform_optimization() {
	# Optimize energy consumption
	# for
	# ./RunSingle/BatchRunSingle.run --N $1
	# ./tests/BatchControl $1
	# cd $ROOT_PATH/CompareWithBaseline
	for file_index in $(seq $MinFileIndex $FILES_PER_CPU $MaxFileIndex); do
	end_index=$((file_index + FILES_PER_CPU))
	echo "Processing N$1: file_index=$file_index:$end_index"
	# ./RunSingleFile/BatchRunSingle.run --N "$1" --begin "$file_index" --end "$end_index" &
	sbatch BatchRT_Approx.sh $1 $file_index $end_index
	done
	wait
}

for (( task_number=$MinTaskNumber; task_number<=$MaxTaskNumber; task_number++ ))
do
echo "Processing N=$task_number:"
	perform_optimization $task_number
done

