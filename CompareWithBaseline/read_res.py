import os

ROOT_PATH = "/home/zephyr/Programming/LET_OPT/"


class ScheduleResult:
    def __init__(self, schedulable, obj, runtime):
        self.schedulable = schedulable
        self.obj = obj
        self.runtime = runtime

    def print(self):
        print(str(self.schedulable) + ", " + str(self.obj) + ", " + str(self.runtime))


def ReadScheduleResult(file_path):
    file = open(file_path, 'r')
    lines = file.readlines()
    return ScheduleResult(int(lines[0]), int(lines[1]), float(lines[2]))


def PrintResultVec(res_vec):
    for res in res_vec:
        res.print()


# return 1D array of the results
def ReadOptResults(obj_type="DataAge", method_name="TOM_Sort", task_set_number=5, root_path=ROOT_PATH):
    data_set_folder = root_path + "TaskData/N" + str(task_set_number)
    res_vec = []
    files_all = os.listdir(data_set_folder)
    files_all.sort()
    for filename in files_all:
        if (filename.find(obj_type) != -1 and filename.find(method_name) != -1):
            file_path = os.path.join(data_set_folder, filename)
            res_vec.append(ReadScheduleResult(file_path))
    return res_vec
