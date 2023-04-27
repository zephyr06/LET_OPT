#!/usr/bin/bash

# ************** Adjust settings there **************
N=(5 10 15 20 25 30 35 40 45 50)
numPerThread=(1000 1000 1000 200 200 200 200 200 200 200)
# ROOT_PATH="/home/zephyr/Programming/LET_OPT"
taskSetNumber=1000
per_core_utilization_min=0.1
per_core_utilization_max=0.9
outDir="generatedNewTaskset"
parallelismFactor=0.4
chainLengthRatio=0.25
numCauseEffectChain=4
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
echo "chainLengthRatio: $chainLengthRatio" >> configs.log
echo "numCauseEffectChain: $numCauseEffectChain" >> configs.log

for (( idx = 0 ; idx < ${#N[@]}; idx++ )); do
    taskNumber=${N[idx]}
    echo "Generating task sets for N$taskNumber"
    mkdir N$taskNumber
    for taskSetStartNumber in $(seq 0 ${numPerThread[idx]} $((taskSetNumber - 1))); do
        ../tests/GenerateTaskSet --taskSetNameStartIndex $taskSetStartNumber --taskSetNumber $(($taskSetStartNumber + ${numPerThread[idx]})) --task_number_in_tasksets $taskNumber \
            --per_core_utilization_min $per_core_utilization_min --per_core_utilization_max $per_core_utilization_max \
            --outDir "release/$outDir/N$taskNumber/" --parallelismFactor $parallelismFactor --chainLengthRatio $chainLengthRatio \
            --numCauseEffectChain $numCauseEffectChain &
    done
    wait
done

wait
exit 0