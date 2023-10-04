ROOT_PATH=/projects/rtss_let/LET_OPT

cd $ROOT_PATH/release
module reset
export MODULEPATH="/projects/rtss_let/modules/tinkercliffs-rome/all:$MODULEPATH"
module --ignore_cache spider GTSAM
module load GTSAM/4.1.1-foss-2021b
cmake -DCMAKE_BUILD_TYPE=RELEASE ..
make BatchRunRT.run -j16 
make BatchRunDA.run -j16
make BatchRunDAWithMart.run -j16
make BatchRunSF.run -j16
