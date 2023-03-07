#include "sources/Utils/DeclareDAG.h"

LLint IndexTran_Instance2Overall(LLint i, LLint instance_i,
                                 const std::vector<LLint> &sizeOfVariables) {
    if (instance_i < 0 || instance_i > sizeOfVariables[i])
        CoutError(
            "Instance Index out of boundary in IndexTran_Instance2Overall");
    if (i < 0 || i > (LLint)sizeOfVariables.size())
        CoutError("Task Index out of boundary in IndexTran_Instance2Overall");
    LLint index = 0;
    for (size_t k = 0; k < (size_t)i; k++) index += sizeOfVariables[k];
    return index + instance_i;
}

int BigIndex2TaskIndex(LLint index, const std::vector<LLint> &sizeOfVariables) {
    int taskIndex = 0;
    int N = sizeOfVariables.size();
    while (index >= 0 && taskIndex < N) {
        index -= sizeOfVariables[taskIndex];
        taskIndex++;
    }
    return taskIndex - 1;
}

double ExtractVariable(const VectorDynamic &startTimeVector,
                       const std::vector<LLint> &sizeOfVariables, int taskIndex,
                       int instanceIndex) {
    if (taskIndex < 0 || instanceIndex < 0 ||
        instanceIndex > sizeOfVariables[taskIndex] - 1) {
        std::cout << Color::red << "Index Error in ExtractVariable!"
                  << Color::def << std::endl;
        throw;
    }

    LLint firstTaskInstance = 0;
    for (int i = 0; i < taskIndex; i++) {
        firstTaskInstance += sizeOfVariables[i];
    }
    return startTimeVector(firstTaskInstance + instanceIndex, 0);
}

double GetSingleElement(LLint index, VectorDynamic &actual,
                        const MAP_Index2Data &mapIndex,
                        std::vector<bool> &filledTable) {
    if (filledTable[index]) return actual(index, 0);
    auto it = mapIndex.find(index);

    if (it != mapIndex.end()) {
        MappingDataStruct curr = it->second;
        actual(index, 0) =
            GetSingleElement(curr.getIndex(), actual, mapIndex, filledTable) +
            curr.getDistance();
        filledTable[index] = true;
    } else {
        CoutError(
            "Out of boundary in GetSingleElement, RecoverStartTimeVector!");
    }
    return actual(index, 0);
}

double RandRange(double a, double b) {
    if (b < a) {
        // CoutError("Range Error in RandRange");
        return a + (b - a) * double(rand()) / RAND_MAX;
    }
    return a + (b - a) * double(rand()) / RAND_MAX;
}

std::string ResizeStr(std::string s, uint size) {
    if (s.size() > size) {
        return s.substr(0, size);
    } else {
        std::string res = "";
        for (size_t i = 0; i < size - s.size(); i++) {
            res += " ";
        }
        return res + s;
    }
}
