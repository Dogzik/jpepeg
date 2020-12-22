#pragma once

#include <ostream>
#include <cstddef>

#include "segments/magics.h"

class huffman_output {
public:
  explicit huffman_output(std::ostream& output)
      : real_output{output} {}

  void write_bits(uint64_t bits, int32_t bits_cnt) {
    buff_bits_cnt += bits_cnt;
    buff |= bits << (64 - buff_bits_cnt);
    while (buff_bits_cnt >= 8) {
      auto cur_byte = static_cast<std::byte>(buff >> (64 - 8));
      real_output.put(std::to_integer<char>(cur_byte));
      if (cur_byte == MARKER_MAGIC) {
        real_output.put(0x00);
      }
      buff_bits_cnt -= 8;
      buff <<= 8;
    }
  }
  
  ~huffman_output() {
    write_bits(0x7F, 7);
  }

private:
  std::ostream& real_output;
  uint64_t buff{};
  int32_t buff_bits_cnt{};
};
