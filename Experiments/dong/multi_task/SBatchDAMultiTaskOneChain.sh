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

echo "Current job id is: $SLURM_JOB_ID"

## change directory and load modules ##
ROOT_PATH=/projects/rtss_let/LET_OPT
# ROOT_PATH=/projects/rtss_let/LET_OPT_DAMultiChains
# ROOT_PATH=/home/dong/workspace/LET_OPT
cd $ROOT_PATH/release
module reset
export MODULEPATH="/projects/rtss_let/modules/tinkercliffs-rome/all:$MODULEPATH"
module --ignore_cache spider GTSAM
module load GTSAM/4.1.1-foss-2021b

## No more needed after success compilation ##
# cmake -DCMAKE_BUILD_TYPE=RELEASE ..
# make BatchRunDAWithMart.run 

N=$1
MinFileIndex=$2
files_per_task=$3
MaxFileIndex=$4

## Launch multiple tasks in one node, make sure applied for adequate ntasks-per-node
for file_index in $(seq $MinFileIndex $files_per_task $MaxFileIndex); do
	end_index=$((file_index + files_per_task))
    echo "./RunSingleFile/BatchRunDAWithMart.run --N $N --begin $file_index --end $end_index &"
    ./RunSingleFile/BatchRunDAWithMart.run --N $N --begin $file_index --end $end_index &
done

## Wait for all of the background tasks to finish
wait
exit 0