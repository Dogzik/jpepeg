#include <iostream>
#include <fstream>
#include <stdexcept>

#include "transcoder/processing.h"

void print_usage() {
  std::cout << "Expected 3 arguments: <mode> <input-file> <output-file>\n";
  std::cout << "mode = -e|--encode_numeric|-d|-decompress\n";
}

/**
 * So far jpepeg support only vanilla non-progressive JPEG without DRI segments
 */
int main(int argc, char* argv[]) {
  if (argc != 4) {
    print_usage();
    return 0;
  }
  std::ifstream input(argv[2], std::ios_base::in | std::ios_base::binary);
  std::ofstream output(argv[3], std::ios_base::out | std::ios_base::binary);
  if (!input.is_open()) {
    std::cout << "Couldn't open input file: " << argv[2] << "\n";
    return 0;
  }
  if (!output.is_open()) {
    std::cout << "Couldn't open output file: " << argv[3] << "\n";
    return 0;
  }
  std::string_view mode(argv[1]);
  try {
    if ((mode == "-e") || (mode == "--encode_numeric")) {
      process(input, output, process_mode::ENCODE);
    } else if ((mode == "-d") || (mode == "--decode")) {
      process(input, output, process_mode::DECODE);
    } else {
      std::cout << "Wrong mode: " << mode << "\n";
      print_usage();
    }
  } catch (std::runtime_error const& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
