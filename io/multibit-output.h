#pragma once

#include <ostream>
#include <cstddef>

#include "segments/magics.h"

class multibit_output {
public:
  explicit multibit_output(std::ostream& output, bool check_marker_magic)
      : real_output{output}
      , check_marker_magic_{check_marker_magic} {}

  void write_bits(uint64_t bits, int32_t bits_cnt) {
    buff_bits_cnt += bits_cnt;
    buff |= bits << (64 - buff_bits_cnt);
    while (buff_bits_cnt >= 8) {
      auto cur_byte = static_cast<std::byte>(buff >> (64 - 8));
      real_output.put(std::to_integer<char>(cur_byte));
      if (check_marker_magic_ && (cur_byte == MARKER_MAGIC)) {
        real_output.put(0x00);
      }
      buff_bits_cnt -= 8;
      buff <<= 8;
    }
  }

  ~multibit_output() {
    write_bits(0x7F, 7);
  }

private:
  std::ostream& real_output;
  uint64_t buff{};
  int32_t buff_bits_cnt{};
  bool check_marker_magic_;
};
