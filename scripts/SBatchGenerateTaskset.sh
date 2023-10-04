#!/bin/bash
#SBATCH -J GenerateTaskSet_19
#SBATCH --account=ev_charging
#SBATCH --partition=normal_q
#SBATCH --nodes=1 --ntasks-per-node=128 --cpus-per-task=1
#SBATCH --time=0-144:00:00 #

#SBATCH --mail-user=dongli@vt.edu
#SBATCH --mail-type=BEGIN  # send email when job begins
#SBATCH --mail-type=END    # send email when job ends
#SBATCH --mail-type=FAIL   # send email when job aborts

echo "Current job id is: $SLURM_JOB_ID"

## change directory and load modules ##
ROOT_PATH=/projects/rtss_let/LET_OPT
# ROOT_PATH=/home/dong/workspace/LET_OPT
cd $ROOT_PATH/release
module reset
export MODULEPATH="/projects/rtss_let/modules/tinkercliffs-rome/all:$MODULEPATH"
module --ignore_cache spider GTSAM
module load GTSAM/4.1.1-foss-2021b

cd ../scripts
./GenerateHighUtilizationTasksets.sh

wait
exit 0
