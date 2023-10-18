#!/usr/bin/bash

# ************** Adjust settings there **************
fork_sensor_num_list=(2 3 4 5 6 7 8 9)
numPerThread=(200 200 200 200 200 200 200 200)
# ROOT_PATH="/home/zephyr/Programming/LET_OPT"
taskSetNumber=1000
per_core_utilization_min=0.9
per_core_utilization_max=0.9
outDir="generatedNewTaskset"
parallelismFactor=0.9
numberOfProcessor=4
SF_ForkNum=-1 # negative means random number of forks
numCauseEffectChain=0 # negative means random number of chains

taskNumber=21
# ***************************************************
cd ../release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --target GenerateTaskSet -j4

rm $outDir -rf
mkdir $outDir
cd $outDir
echo "Sensor Fusion Only" > configs.log
echo "New TaskSet Configs:" >> configs.log
echo "taskNumber: $taskNumber" >> configs.log
echo "taskSetNumber: $taskSetNumber" >> configs.log
echo "per_core_utilization_min: $per_core_utilization_min" >> configs.log
echo "per_core_utilization_max: $per_core_utilization_max" >> configs.log
echo "parallelismFactor: $parallelismFactor" >> configs.log
echo "numberOfProcessor: $numberOfProcessor" >> configs.log
echo "SF_ForkNum: $SF_ForkNum" >> configs.log
echo "numCauseEffectChain: $numCauseEffectChain" >> configs.log
echo "list of fork_sensor_num: ${fork_sensor_num_list[@]}" >> configs.log

for (( idx = 0 ; idx < ${#fork_sensor_num_list[@]}; idx++ )); do
    fork_sensor_num=${fork_sensor_num_list[idx]}
    echo "Generating task sets for N$taskNumber with max sensor $fork_sensor_num"
    mkdir N$fork_sensor_num
    for taskSetStartNumber in $(seq 0 ${numPerThread[idx]} $((taskSetNumber - 1))); do
        ../tests/GenerateTaskSet --taskSetNameStartIndex $taskSetStartNumber \
            --taskSetNumber $(($taskSetStartNumber + ${numPerThread[idx]})) \
            --task_number_in_tasksets $taskNumber \
            --per_core_utilization_min $per_core_utilization_min \
            --per_core_utilization_max $per_core_utilization_max \
            --outDir "release/$outDir/N$fork_sensor_num/" \
            --parallelismFactor $parallelismFactor \
            --numberOfProcessor $numberOfProcessor \
            --clearOutputDir 0 \
            --numCauseEffectChain $numCauseEffectChain \
            --SF_ForkNum $SF_ForkNum \
            --fork_sensor_num_min $fork_sensor_num \
            --fork_sensor_num_max $fork_sensor_num \
            &
        sleep 1.1
    done
    wait
done

echo "TaskSet Creation Time: $(date +%Y%m%d)" | cat - configs.log > temp && mv temp configs.log

echo "Rename files name to have correct N number"
for (( idx = 0 ; idx < ${#fork_sensor_num_list[@]}; idx++ )); do
    fork_sensor_num=${fork_sensor_num_list[idx]}
    for file in N$fork_sensor_num/*.csv; do 
        mv $file $(echo $file | sed s/N${taskNumber}/N${fork_sensor_num}/); 
    done
done
wait
exit 0