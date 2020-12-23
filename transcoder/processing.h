#pragma once

#include <istream>
#include <ostream>

enum class process_mode {
  ENCODE,
  DECODE,
};

void process(std::istream& input, std::ostream& output, process_mode mode);
