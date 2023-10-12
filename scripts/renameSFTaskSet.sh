#!/usr/bin/bash
ROOT_PATH=/home/dong/workspace/LET_OPT/TaskData/batchTaskSetsBackup/SFwithJitter/20231011/generatedNewTaskset
fork_sensor_num_list=(2 3 4 5 6 7 8 9)

echo rename files in dir : $ROOT_PATH


for (( idx = 0 ; idx < ${#fork_sensor_num_list[@]}; idx++ )); do
    fork_sensor_num=${fork_sensor_num_list[idx]}
    for file in $ROOT_PATH/N$fork_sensor_num/*.csv; do 
        mv $file $(echo $file | sed s/N40/N${fork_sensor_num}/); 
    done
done
