#!/usr/bin/bash

# ************** Adjust settings there **************
N=(5 10 15 20 25 30 35 40 45 50)
# ROOT_PATH="/home/zephyr/Programming/LET_OPT"
taskSetNumber=1000
per_core_utilization_min=0.1
per_core_utilization_max=0.9
outDir="generatedNewTaskset"
parallelismFactor=0.4
chainLengthRatio=0.25
# ***************************************************
cd ../release
make GenerateTaskSet -j4

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

for taskNumber in ${N[@]}; do
    mkdir N$taskNumber
    ../tests/GenerateTaskSet --taskSetNumber $taskSetNumber --task_number_in_tasksets $taskNumber \
        --per_core_utilization_min $per_core_utilization_min --per_core_utilization_max $per_core_utilization_max \
        --outDir "release/$outDir/N$taskNumber/" --parallelismFactor $parallelismFactor --chainLengthRatio $chainLengthRatio &
done

wait
exit 0