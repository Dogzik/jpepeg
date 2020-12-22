#pragma once

#include "segments/magics.h"
#include "segments/simple-segment.h"

class DQT_segment final : public simple_segment {
public:
  explicit DQT_segment(std::istream& input)
      : simple_segment(input, true) {}

  [[nodiscard]] std::byte get_magic() const noexcept override {
    return DQT_MAGIC;
  }
};
