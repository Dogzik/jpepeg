#pragma once

#include <vector>

#include "segments/magics.h"
#include "segments/segment.h"
#include "huffman/huffman-table.h"

struct DHT_segment final : public segment {
  std::vector<huffman_table> tables;

  explicit DHT_segment(std::istream& input) {
    auto length = decode_length(input) - 2;
    while (length > 0) {
      tables.emplace_back(input);
      length -= tables.back().serialized_length();
    }
  }

  [[nodiscard]] std::byte get_magic() const noexcept override {
    return DHT_MAGIC;
  }

private:
  void write_internals(std::ostream& output) const override {
    uint16_t total_tables_size = 0;
    for (auto const& table : tables) {
      total_tables_size += table.serialized_length();
    }
    encode_length(output, total_tables_size + 2);
    for (const auto& table : tables) {
      table.write_internals(output);
    }
  }
};
