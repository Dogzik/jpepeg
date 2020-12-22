#pragma once

#include "segments/simple-segment.h"

class APP_segment final : public simple_segment {
public:
  APP_segment(std::istream& input, std::byte magic)
      : simple_segment(input, true), magic_{magic} {}

  static bool is_valid_magic(std::byte magic) {
    return (magic & std::byte{0xF0}) == std::byte{0xE0};
  }

  [[nodiscard]] std::byte get_magic() const noexcept override {
    return magic_;
  }

private:
  std::byte magic_;
};
