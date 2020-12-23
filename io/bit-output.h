#pragma once

#include <string>
#include <ostream>
#include <cstddef>
#include <iostream>

class bit_output {
public:
  explicit bit_output(std::ostream& out_)
      : out{out_} {}

  void put_bit(bool bit) {
    if (bit) {
      buff |= cur_mask;
    }
    cur_mask >>= 1;
    if (cur_mask == BYTE_ZERO) {
      flush_buff();
      buff = BYTE_ZERO;
      cur_mask = TOP_BIT_BYTE;
    }
  }

  ~bit_output() {
    if (cur_mask != TOP_BIT_BYTE) {
      flush_buff();
    }
  }

private:
  std::byte buff{0};
  std::byte cur_mask{TOP_BIT_BYTE};
  std::ostream& out;

  void flush_buff() {
    out.put(std::to_integer<char>(buff));
  }
};
