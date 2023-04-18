#include "sources/BatchOptimize/batchOptimize.h"
#include "sources/Utils/argparse.hpp"

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("program name");
  program.add_argument("-v", "--verbose");  // parameter packing

  program.add_argument("--N")
      .default_value(5)
      .help("the folder of task sets to run experiments")
      .scan<'i', int>();
  program.add_argument("--begin")
      .default_value(0)
      .help("the first file index that's going to be optimized")
      .scan<'i', int>();
  program.add_argument("--end")
      .default_value(1000)
      .help("the last file index that's going to be optimized (Not INCLUSIVE)")
      .scan<'i', int>();
  program.add_argument("--obj")
      .default_value(std::string("RT"))
      .help("the type of objective function, RT or DA");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    exit(0);
  }
  int N = program.get<int>("--N");
  int begin_index = program.get<int>("--begin");
  int end_index = program.get<int>("--end");
  std::string obj_type = program.get<std::string>("--obj");

  std::vector<DAG_SPACE::BASELINEMETHODS> baselineMethods = {
      DAG_SPACE::InitialMethod, DAG_SPACE::TOM_WSkip, DAG_SPACE::TOM_Sort,
      DAG_SPACE::TOM_Sort_Bound};
  DAG_SPACE::BatchSettings batch_test_settings(
      N, begin_index, end_index, "TaskData/N" + std::to_string(N) + "/");

  if (obj_type == "RT")
    DAG_SPACE::BatchOptimizeOrder<DAG_SPACE::ObjReactionTime>(
        baselineMethods, batch_test_settings);
  else if (obj_type == "DA")
    DAG_SPACE::BatchOptimizeOrder<DAG_SPACE::ObjDataAge>(baselineMethods,
                                                         batch_test_settings);
  else
    CoutError("Please provide recognized --obj");

  std::cout << "N: " << N << "\n";
  PrintTimer();
}