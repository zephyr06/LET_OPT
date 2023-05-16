# ROOT_PATH=/projects/rtss_let/LET_OPT
ROOT_PATH=/home/dong/workspace/LET_OPT

cd $ROOT_PATH/release
cmake -DCMAKE_BUILD_TYPE=RELEASE ..
make BatchRunAll.run -j8
make BatchRunSF.run -j8
make BatchDAWithMartRunAll.run -j8
