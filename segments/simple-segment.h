#pragma once

#include <vector>
#include <istream>

#include "segments/segment.h"

class simple_segment : public segment {
public:
  simple_segment(std::istream& input, bool include_marker)
      : include_marker_{include_marker} {
    auto size = decode_length(input) - include_marker_ * 2;
    data.resize(size);
    input.read(reinterpret_cast<char*>(data.data()), data.size());
  }

  void write_internals(std::ostream& output) const {
    encode_length(output, data.size() + include_marker_ * 2);
    output.write(reinterpret_cast<const char*>(data.data()), data.size());
  }

protected:
  bool include_marker_;
  std::vector<std::byte> data;
};
