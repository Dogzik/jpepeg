#pragma once

#include <istream>
#include <cstddef>

class bit_input {
public:
  explicit bit_input(std::istream& real_in)
      : in{real_in} {}

  bool get_bit() {
    if (cur_mask == BYTE_ONE) {
      auto raw_byte = in.get();
      if (raw_byte == std::char_traits<char>::eof()) {
        if (extra_zeros < 0) {
          throw std::runtime_error("Reading too much past eof");
        }
        buff = BYTE_ZERO;
        extra_zeros -= 8;
      } else {
        buff = static_cast<std::byte>(raw_byte);
      }
      cur_mask = TOP_BIT_BYTE;
    } else {
      cur_mask >>= 1;
    }
    return (buff & cur_mask) != BYTE_ZERO;
  }

private:
  std::istream& in;
  std::byte buff{};
  std::byte cur_mask{BYTE_ONE};
  int64_t extra_zeros{CODE_BITS};
};
