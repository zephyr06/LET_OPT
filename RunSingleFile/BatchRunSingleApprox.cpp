#include "sources/Utils/argparse.hpp"
#include "sources/batchOptimize.h"

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("program name");
  program.add_argument("-v", "--verbose");  // parameter packing

  program.add_argument("--N")
      .default_value(5)
      .help("the folder of task sets to run experiments")
      .scan<'i', int>();

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    exit(0);
  }
  int N = program.get<int>("--N");

  std::vector<DAG_SPACE::BaselineMethods> baselineMethods = {
      DAG_SPACE::InitialMethod, DAG_SPACE::TOM};  // , DAG_SPACE::TOM_FAST
  // ObjReactionTime
  DAG_SPACE::BatchOptimizeOrder<DAG_SPACE::ObjReactionTimeApprox>(
      baselineMethods, N);
  std::cout << "N: " << N << "\n";
  PrintTimer();
}