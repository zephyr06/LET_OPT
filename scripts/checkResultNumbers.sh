#!/usr/bin/bash
# COUNT_PATH="/home/dong/workspace/DAG_NLP/TaskData"
# COUNT_PATH=/home/zephyr/Programming/DAG_NLP/TaskData
# COUNT_PATH="/home/dong/workspace/DAG_NLP/release/generatedNewTaskset"
COUNT_PATH="/projects/rtss_let/LET_OPT/TaskData"

# echo current results count in dir: $COUNT_PATH


for N in {5..21}; do
    for idx in {000..999}; do
        file_prefix="${COUNT_PATH}/N${N}/dag-set-N$N-$idx*"
        cnt=$(ls $file_prefix | wc -l)
        if [[ cnt -lt 5 ]]; then
            echo $N-$idx: $cnt
        fi
    done
done

