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
      .default_value(std::string("DA"))
      .help("the type of objective function,  DA");

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

  DAG_SPACE::BatchSettings batch_test_settings(
      N, begin_index, end_index, "TaskData/N" + std::to_string(N) + "/");
  batch_test_settings.chainNum = 1;

  std::vector<DAG_SPACE::BASELINEMETHODS> baselineMethods = {
      DAG_SPACE::InitialMethod, DAG_SPACE::ImplicitCommunication,
      // DAG_SPACE::TOM_BF,          DAG_SPACE::TOM_WSkip,
      // DAG_SPACE::TOM_Sort,
      DAG_SPACE::Martinez18, DAG_SPACE::TOM_Sort_Offset,
      DAG_SPACE::Bardatsch16};

  DAG_SPACE::BatchOptimizeOrder<DAG_SPACE::ObjDataAge>(baselineMethods,
                                                       batch_test_settings);

  std::cout << "N: " << N << "\n";
  PrintTimer();
}