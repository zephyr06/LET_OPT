ROOT_PATH=/projects/rtss_let/LET_OPT
#ROOT_PATH=/home/dong/workspace/LET_OPT

backup_file_name=ARC_Results_Backup-$(date +"%Y%m%d%H%M%S")
echo "backup in $ROOT_PATH/Experiments/backups/${backup_file_name}.tar.gz"
cd $ROOT_PATH/Experiments/backups

mkdir $backup_file_name
cp $ROOT_PATH/TaskData/configs.log ./$backup_file_name/
cp -r $ROOT_PATH/TaskData/N* ./$backup_file_name/
rm ./$backup_file_name/*.zip

cp -r $ROOT_PATH/Experiments/dong ./$backup_file_name/slurm_outputs/
cp $ROOT_PATH/Experiments/*.out ./$backup_file_name/slurm_outputs/

tar -czf $backup_file_name.tar.gz $backup_file_name

rm $backup_file_name -r
