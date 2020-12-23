#pragma once

#include <vector>
#include <istream>

class multibit_input {
public:
  explicit multibit_input(std::istream& input)
      : real_input(input) {}

  int32_t peek_bits(uint16_t cnt) noexcept {
    while (buff_bits < cnt) {
      buff_bits += 8;
      buff = (buff << 8) | get_byte();
    }
    return (buff >> (buff_bits - cnt)) & ((1 << cnt) - 1);
  }

  void skip_bits(uint16_t cnt) {
    if (buff_bits < cnt) {
      peek_bits(cnt);
    }
    buff_bits -= cnt;
  }

private:
  int32_t buff{};
  size_t buff_bits{0};
  std::istream& real_input;

  [[nodiscard]] int32_t get_byte() {
    auto byte = real_input.get();
    if (!real_input.eof()) {
      return byte;
    } else {
      return 0xFF;
    }
  }
};
