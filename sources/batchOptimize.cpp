#include "sources/batchOptimize.h"

namespace DAG_SPACE {

void ClearResultFiles(std::string dataSetFolder) {
    std::string dirStr = dataSetFolder;
    const char *pathDataset = (dirStr).c_str();
    std::vector<std::string> files = ReadFilesInDirectory(pathDataset);
    for (const auto &file : files) {
        std::string delimiter = "-";
        if (file.find("Res") != std::string::npos) {
            std::string path = dataSetFolder + file;
            if (remove(path.c_str()) != 0) {
                perror("Error deleting file!");
            }
        }
    }
}

}  // namespace DAG_SPACE