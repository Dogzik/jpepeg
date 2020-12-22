#pragma once

#include <vector>

#include "segments/magics.h"
#include "segments/segment.h"

struct SOF0_segment : segment {
  struct component {
    uint8_t id;
    uint8_t horizontal_sampling;
    uint8_t vertical_sampling;
    uint8_t quantization_table;

    explicit component(std::istream& input) {
      id = input.get();
      uint8_t samplings = input.get();
      horizontal_sampling = samplings >> 4;
      vertical_sampling = samplings & 0x0F;
      quantization_table = input.get();
    }

    void write(std::ostream& output) const {
      output.put(id);
      output.put((horizontal_sampling << 4) + vertical_sampling);
      output.put(quantization_table);
    }
  };

  uint8_t precision;
  uint16_t height;
  uint16_t width;
  std::vector<component> components;

  explicit SOF0_segment(std::istream& input) {
    decode_length(input);
    precision = input.get();
    height = decode_length(input);
    width = decode_length(input);
    uint8_t components_cnt = input.get();
    components.reserve(components_cnt);
    for (uint8_t i = 0; i < components_cnt; ++i) {
      components.emplace_back(input);
    }
  }

  [[nodiscard]] std::byte get_magic() const noexcept override {
    return SOF0_MAGIC;
  }

private:
  void write_internals(std::ostream& output) const override {
    encode_length(output, 8 + 3 * components.size());
    output.put(precision);
    encode_length(output, height);
    encode_length(output, width);
    output.put(components.size());
    for (auto component : components) {
      component.write(output);
    }
  }
};
