#include <iostream>
#include <fstream>
#include <stdexcept>

#include "segments/COM-segment.h"
#include "segments/DQT-segment.h"
#include "segments/APP-segment.h"
#include "segments/SOF0-segment.h"
#include "segments/DHT-segment.h"
#include "segments/SOS-segment.h"

#include "huffman/huffman-decoding.h"
#include "coder/encoding.h"

void print_usage() {
  std::cout << "Expected 3 arguments: <mode> <input-file> <output-file>\n";
  std::cout << "mode = -e|--encode|-d|-decompress\n";
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
  if (!input.is_open()) {
    std::cout << "Couldn't open input file: " << argv[2] << "\n";
    return 0;
  }
  std::string_view mode(argv[1]);
  try {
    if ((mode == "-e") || (mode == "--encode")) {
      encode_jpg(input);
    } else if ((mode == "-d") || (mode == "--decode")) {
      throw std::runtime_error("Not supported yet");
    } else {
      std::cout << "Wrong mode: " << mode << "\n";
      print_usage();
    }
  } catch (std::runtime_error const& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
