# LET_OPT
implementation plan:
- describe permutaion
    - 
- perform optimization given a permutation


# Some scripts
'''
N=5
./tests/GenerateTaskSet --taskSetNumber 1000 --task_number_in_tasksets $N --per_core_utilization 0.4 --outDir "TaskData/N$N/" --parallelismFactor 0.4
make BatchRunSingle.run
./RunSingleFile/BatchLETRunSingle --N 5
'''