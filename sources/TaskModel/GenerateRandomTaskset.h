#include <time.h>
#include <cmath>

#include <boost/program_options/options_description.hpp>

#include "sources/TaskModel/RegularTasks.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Parameters.h"

using namespace RegularTaskSystem;
namespace po = boost::program_options;

//std::vector<int> PeriodSetAM = {1, 2, 5, 10, 20, 50, 100, 200, 1000};
//std::vector<int> PeriodSetAM = {2, 5, 10, 20, 50, 100, 200};
static std::vector<int> PeriodSetAM = {100, 200, 300};
//std::vector<int> PeriodSetAM = {100, 200, 300, 400, 500, 600, 800, 1000, 1200};
static const std::vector<int> PeriodSetWaters = {1, 2, 5, 10, 20, 50, 100, 200, 1000};
static const std::vector<int> PeriodPDFWaters = {3, 2, 2, 25, 25, 3, 20, 1, 4};
static const std::vector<int> PeriodCDFWaters = {3, 5, 7, 32, 57, 60, 80, 81, 85};

std::vector<double> Uunifast(int N, double utilAll, bool boundU = true);

TaskSet GenerateTaskSet(int N, double totalUtilization,
                        int numberOfProcessor, int periodMin,
                        int periodMax, int coreRequireMax,
                        int taskSetType = 1, int deadlineType = 0);

void WriteTaskSets(std::ofstream &file, TaskSet &tasks);

using namespace OrderOptDAG_SPACE;
DAG_Model GenerateDAG(int N, double totalUtilization,
                      int numberOfProcessor, int periodMin,
                      int periodMax, int coreRequireMax,
                      int taskSetType = 1, int deadlineType = 0);

void WriteDAG(std::ofstream &file, DAG_Model &tasksDAG);