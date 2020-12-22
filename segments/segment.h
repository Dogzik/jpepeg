#pragma once

#include <cstddef>
#include <cstdint>
#include <istream>

struct segment {
  [[nodiscard]] virtual std::byte get_magic() const noexcept = 0;

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

  void write(std::ostream& output) const {
    output.put(std::to_integer<char>(MARKER_MAGIC));
    output.put(std::to_integer<char>(get_magic()));
    write_internals(output);
  }

  virtual ~segment() = default;

private:
  virtual void write_internals(std::ostream& output) const = 0;
};
