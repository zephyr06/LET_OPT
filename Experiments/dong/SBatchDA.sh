#!/bin/bash
# SBATCH -J BatchRunAll
#SBATCH --account=ev_charging
#SBATCH --partition=normal_q
#SBATCH --nodes=1 --ntasks-per-node=1 --cpus-per-task=1
#SBATCH --time=0-06:00:00 # 6 hours

#SBATCH --mail-user=dongli@vt.edu
#SBATCH --mail-type=BEGIN  # send email when job begins
#SBATCH --mail-type=END    # send email when job ends
#SBATCH --mail-type=FAIL   # send email when job aborts

module reset
# module load CMake/3.15.3-GCCcore-8.3.0
# module load boost/1.68.0.a 
# module load Eigen/3.3.7-GCCcore-8.3.0
module load yaml-cpp/0.6.3-GCCcore-8.3.0


ROOT_PATH=/projects/rtss_let/LET_OPT

cd $ROOT_PATH/release
# cmake -DCMAKE_BUILD_TYPE=RELEASE ..
# make BatchRunAll.run 
N=$1
file_index=$2
end_index=$3
./RunSingleFile/BatchDAWithMartRunAll.run --N "$N" --begin "$file_index" --end "$end_index" 
