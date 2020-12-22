#pragma once

#include "segments/magics.h"
#include "segments/simple-segment.h"

class COM_segment final : public simple_segment {
public:
  explicit COM_segment(std::istream& input)
      : simple_segment(input, false) {}

  [[nodiscard]] std::byte get_magic() const noexcept override {
    return COM_MAGIC;
  }
};
