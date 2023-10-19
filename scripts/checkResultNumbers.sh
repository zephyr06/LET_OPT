#!/usr/bin/bash
COUNT_PATH="/home/dong/workspace/LET_OPT/TaskData"
# COUNT_PATH=/home/zephyr/Programming/LET_OPT/TaskData
# COUNT_PATH="/home/dong/workspace/LET_OPT/release/generatedNewTaskset"
# COUNT_PATH="/projects/rtss_let/LET_OPT/TaskData"

# echo current results count in dir: $COUNT_PATH


for N in {40..40}; do
    for idx in {000..999}; do
        file_prefix="${COUNT_PATH}/N${N}/dag-set-N$N-$idx*"
        cnt=$(ls $file_prefix | wc -l)
        if [[ cnt -lt 15 ]]; then
            # rm ${COUNT_PATH}/N${N}/dag-set-N$N-$idx*
            echo $N-$idx: $cnt
        fi
    done
done

