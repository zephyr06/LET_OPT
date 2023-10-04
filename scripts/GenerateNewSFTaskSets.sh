#!/usr/bin/bash

# ************** Adjust settings there **************
N=(5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21)
# numPerThread=(10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10)
numPerThread=(200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200)
# N=(4 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40 42 44 46 48 50)
# numPerThread=(200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200 200)
# ROOT_PATH="/home/zephyr/Programming/LET_OPT"
taskSetNumber=1000
per_core_utilization_min=0.3
per_core_utilization_max=0.9
outDir="generatedNewTaskset"
parallelismFactor=0.9
numberOfProcessor=4
SF_ForkNum=2 # negative means random number of forks
fork_sensor_num_min=2
fork_sensor_num_max=9
numCauseEffectChain=0 # negative means random number of chains
# ***************************************************
cd ../release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --target GenerateTaskSet -j4

rm $outDir -rf
mkdir $outDir
cd $outDir
echo "New TaskSet Configs:" > configs.log
echo "taskNumber_list: ${N[@]}" >> configs.log
echo "taskSetNumber: $taskSetNumber" >> configs.log
echo "per_core_utilization_min: $per_core_utilization_min" >> configs.log
echo "per_core_utilization_max: $per_core_utilization_max" >> configs.log
echo "parallelismFactor: $parallelismFactor" >> configs.log
echo "numberOfProcessor: $numberOfProcessor" >> configs.log
echo "SF_ForkNum: $SF_ForkNum" >> configs.log
echo "fork_sensor_num_min: $fork_sensor_num_min" >> configs.log
echo "fork_sensor_num_max: $fork_sensor_num_max" >> configs.log
echo "numCauseEffectChain: $numCauseEffectChain" >> configs.log

for (( idx = 0 ; idx < ${#N[@]}; idx++ )); do
    taskNumber=${N[idx]}
    echo "Generating task sets for N$taskNumber"
    mkdir N$taskNumber
    for taskSetStartNumber in $(seq 0 ${numPerThread[idx]} $((taskSetNumber - 1))); do
        ../tests/GenerateTaskSet --taskSetNameStartIndex $taskSetStartNumber \
            --taskSetNumber $(($taskSetStartNumber + ${numPerThread[idx]})) \
            --task_number_in_tasksets $taskNumber \
            --per_core_utilization_min $per_core_utilization_min \
            --per_core_utilization_max $per_core_utilization_max \
            --outDir "release/$outDir/N$taskNumber/" \
            --parallelismFactor $parallelismFactor \
            --numberOfProcessor $numberOfProcessor \
            --clearOutputDir 0 \
            --SF_ForkNum $SF_ForkNum \
            --fork_sensor_num_min $fork_sensor_num_min \
            --fork_sensor_num_max $fork_sensor_num_max \
            --numCauseEffectChain $numCauseEffectChain \
            &
        sleep 1.1
    done
    wait
done

echo "TaskSet Creation Time: $(date +"%Y%m%d")" | cat - configs.log > temp && mv temp configs.log

wait
exit 0