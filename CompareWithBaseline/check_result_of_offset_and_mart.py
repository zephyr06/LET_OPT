from ScheduleResult import *

# return two float vectors, which save the average obj and run-time


def CheckOptResultsOffsetMart(task_set_number_range=[5], root_path=ROOT_PATH):
    method_name1 = "TOM_Sort_Offset"
    method_name2 = "Martinez18"
    obj_type = "DataAge"

    for task_set_number in task_set_number_range:
        data_set_folder = root_path + "TaskData/N" + str(task_set_number)
        res_vec = []
        files_all = os.listdir(data_set_folder)
        files_all.sort()
        for filename in files_all:
            file_signature1 = method_name1 + "_" + obj_type
            file_signature2 = method_name2 + "_" + obj_type
            if (filename.find(file_signature1) != -1):
                file_path = os.path.join(data_set_folder, filename)
                res1 = ReadScheduleResult(file_path)
                filename = filename.replace(file_signature1, file_signature2)
                file_path = os.path.join(data_set_folder, filename)
                res2 = ReadScheduleResult(file_path)
                if res1.runtime < 600 and res2.runtime < 600:
                    if res1.obj != res2.obj:
                        print("Check this file's Mart and Offset: ", filename)
CheckOptResultsOffsetMart([5, 10, 15, 20, 25, 30, 35, 40, 45, 50])
