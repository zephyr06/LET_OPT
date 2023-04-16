#!/bin/bash
#SBATCH -J BatchRunAll
#SBATCH --account=ev_charging
#SBATCH --partition=normal_q
#SBATCH --nodes=1 --ntasks-per-node=1 --cpus-per-task=1
#SBATCH --time=0-06:00:00 # 6 hours

#SBATCH --mail-user=dongli@vt.edu
#SBATCH --mail-type=BEGIN  # send email when job begins
#SBATCH --mail-type=END    # send email when job ends
#SBATCH --mail-type=FAIL   # send email when job aborts

## change directory and load modules ##
ROOT_PATH=/projects/rtss_let/LET_OPT
# ROOT_PATH=/home/dong/workspace/LET_OPT
cd $ROOT_PATH/release
module reset
module load yaml-cpp/0.6.3-GCCcore-8.3.0

## No more needed after success compilation ##
# module load CMake/3.15.3-GCCcore-8.3.0
# module load boost/1.68.0.a 
# module load Eigen/3.3.7-GCCcore-8.3.0
# cmake -DCMAKE_BUILD_TYPE=RELEASE ..
# make BatchRunAll.run 

N=$1
MinFileIndex=$2
files_per_task=$3
MaxFileIndex=$4

## Launch multiple tasks in one node, make sure applied for adequate ntasks-per-node
for file_index in $(seq $MinFileIndex $files_per_task $MaxFileIndex); do
	end_index=$((file_index + files_per_task))
    echo "./RunSingleFile/BatchRunAll.run --N $N --begin $file_index --end $end_index &"
    ./RunSingleFile/BatchRunAll.run --N $N --begin $file_index --end $end_index &
done

## Wait for all of the background tasks to finish
wait
exit 0