ROOT_PATH=/projects/rtss_let/LET_OPT

cd $ROOT_PATH/release
module reset
module load yaml-cpp/0.6.3-GCCcore-8.3.0

module load CMake/3.15.3-GCCcore-8.3.0
module load boost/1.68.0.a 
module load Eigen/3.3.7-GCCcore-8.3.0
cmake -DCMAKE_BUILD_TYPE=RELEASE ..
make BatchRunAll.run -j16 
make BatchRunSF.run -j16
make BatchDAWithMartRunAll.run -j16
make BatchDARunMultiChain.run -j16
