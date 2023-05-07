#!/usr/bin/bash

# ************** Adjust settings there **************
N=(5 10 15 20 25 30 35 40 45 50)
# N=(4 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40 42 44 46 48 50)
# ROOT_PATH="/home/zephyr/Programming/LET_OPT"
# ***************************************************
cd ../release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --target BatchDAWithMartRunAll.run -j4

for (( idx = 0 ; idx < ${#N[@]}; idx++ )); do
    taskNumber=${N[idx]}
    echo "Run Bardatsch16 of DA for N$taskNumber"
    echo "./RunSingleFile/BatchDAWithMartRunAll.run --N $taskNumber &"
    ./RunSingleFile/BatchDAWithMartRunAll.run --N $taskNumber &
done

wait

cd ../Experiments/backups/
./backupresults.sh
rm ../../TaskData/N* -r
rm ../../TaskData/configs.log

wait
exit 0