#pragma once

#include <cstddef>
#include <cstdint>
#include <istream>

struct segment {
  virtual std::byte get_magic() const noexcept = 0;

  static uint16_t decode_length(std::istream& input) {
    uint16_t high = input.get();
    uint16_t low = input.get();
    return (high << 8) + low;
  }

  static void encode_length(std::ostream& output, uint16_t length) {
    uint16_t high = length >> 8;
    uint16_t low = length & 0xFF;
    output.put(high);
    output.put(low);
  }

  virtual ~segment() = default;
};
