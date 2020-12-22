#pragma once

#include <array>
#include <istream>
#include <vector>

#include "segments/magics.h"
#include "segments/segment.h"

struct SOS_segment final : segment {
  struct component {
    uint8_t id;
    uint8_t dc_table_id;
    uint8_t ac_table_id;

    explicit component(std::istream& input) {
      id = input.get();
      uint8_t tables = input.get();
      dc_table_id = tables >> 4;
      ac_table_id = 0x10 + (tables & 0x0F);
    }

    void write_internals(std::ostream& output) const {
      output.put(id);
      output.put(((dc_table_id & 0x0F) << 4) + (ac_table_id & 0x0F));
    }
  };

  std::vector<component> components;
  std::array<std::byte, 3> skip_bytes;

  explicit SOS_segment(std::istream& input) {
    decode_length(input);
    uint8_t components_cnt = input.get();
    components.reserve(components_cnt);
    for (uint8_t i = 0; i < components_cnt; ++i) {
      components.emplace_back(input);
    }
    input.read(reinterpret_cast<char*>(skip_bytes.data()), skip_bytes.size());
  }

  void write_internals(std::ostream& output) const {
    encode_length(output, 6 + 2 * components.size());
    output.put(components.size());
    for (auto component : components) {
      component.write_internals(output);
    }
    output.write(reinterpret_cast<const char*>(skip_bytes.data()), skip_bytes.size());
  }

  [[nodiscard]] std::byte get_magic() const noexcept override {
    return SOS_MAGIC;
  }
};
