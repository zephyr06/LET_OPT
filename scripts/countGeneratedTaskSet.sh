#!/usr/bin/bash
COUNT_PATH="/home/dong/workspace/LET_OPT/TaskData"
#COUNT_PATH="/home/dong/workspace/LET_OPT/release/generatedNewTaskset"

echo current results count in dir: $COUNT_PATH

for dir in `ls $COUNT_PATH`; do
    if [[ $dir =~ N.* ]]; then
        echo $dir: `ls $COUNT_PATH/$dir | wc -l`
    fi
done
