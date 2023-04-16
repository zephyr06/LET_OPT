import os

ROOT_PATH = "/home/zephyr/Programming/LET_OPT/"


class ScheduleResult:
    def __init__(self, schedulable, obj, runtime, file_path):
        self.schedulable = schedulable
        self.obj = obj
        self.runtime = runtime
        self.file_path = file_path

    def print(self):
        print(str(self.schedulable) + ", " + str(self.obj) + ", " + str(self.runtime))


def ReadScheduleResult(file_path):
    file = open(file_path, 'r')
    lines = file.readlines()
    return ScheduleResult(int(lines[0]), int(lines[1]), float(lines[2]), file_path)


def PrintResultVec(res_vec):
    for res in res_vec:
        res.print()


# return 1D array of the results
def ReadOptResults(method_name="TOM_Sort", obj_type="DataAge", task_set_number=5, root_path=ROOT_PATH):
    data_set_folder = root_path + "TaskData/N" + str(task_set_number)
    res_vec = []
    files_all = os.listdir(data_set_folder)
    files_all.sort()
    for filename in files_all:
        if (filename.find(obj_type) != -1 and filename.find(method_name) != -1):
            file_path = os.path.join(data_set_folder, filename)
            res_vec.append(ReadScheduleResult(file_path))
    if (len(res_vec) == 0):
        print('\033[93m' + method_name + " not found!")
    return res_vec


def Average(res_vec, base_vec):
    average_obj = 0
    average_runtime = 0
    total_case = len(res_vec)
    # for res in res_vec:
    for i in range(len(res_vec)):
        average_obj += float(res_vec[i].obj) / base_vec[i].obj
        average_runtime += res_vec[i].runtime
    return average_obj / total_case, average_runtime / total_case
