#pragma once

#include <vector>

class huffman_input {
public:
  explicit huffman_input(std::vector<std::byte>&& data)
      : data_(std::move(data)) {}

  int32_t peek_bits(uint16_t cnt) noexcept {
    while (buff_bits < cnt) {
      buff_bits += 8;
      buff = (buff << 8) | std::to_integer<int32_t>(get_byte());
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
  size_t idx{0};
  size_t buff_bits{0};
  std::vector<std::byte> data_;

  [[nodiscard]] std::byte get_byte() {
    if (idx < data_.size()) {
      return data_[idx++];
    } else {
      return std::byte{0xFF};
    }
  }
};
