# LET_OPT
implementation plan:
- describe permutaion
    - 
- perform optimization given a permutation


# Some scripts
```
make GenerateTaskSet
N=5
./tests/GenerateTaskSet --taskSetNumber 1000 --task_number_in_tasksets $N --per_core_utilization_min 0.1 --per_core_utilization_max 0.9 --outDir "TaskData/N$N/" --parallelismFactor 0.4 --chainLength 2

make BatchRunSingle -j8

./RunSingleFile/BatchRunSingle --N 5

./RunSingleFile/BatchLETRunSingle --N 5
```