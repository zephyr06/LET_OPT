#!/bin/bash
#SBATCH -J hello-world
#SBATCH --account=ev_charging
#SBATCH --partition=normal_q
#SBATCH --nodes=1 --ntasks-per-node=1 --cpus-per-task=2  
#SBATCH --time=0-00:20:00 # 20 minutes

#SBATCH --mail-user=swang666@vt.edu
#SBATCH --mail-type=BEGIN  # send email when job begins
#SBATCH --mail-type=END    # send email when job ends
#SBATCH --mail-type=FAIL   # send email when job aborts

module reset
module load CMake/3.15.3-GCCcore-8.3.0
module load boost/1.79 
module load Eigen/3.3.7
module load yaml-cpp/0.6.3-GCCcore-8.3.0

cd ~/Programming/LET_OPT/release
N=$1
file_index=$2
end_index=$3
./RunSingleFile/BatchRunSingle.run --N "$N" --begin "$file_index" --end "$end_index" 
