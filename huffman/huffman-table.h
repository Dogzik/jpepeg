#pragma once

#include <array>
#include <istream>
#include <vector>

struct huffman_table {
  static constexpr uint8_t MAX_CODE_SIZE = 16;

  struct encode_info {
    uint16_t code{};
    uint8_t size{};
  };

  struct decode_info {
    uint8_t symbol{};
    uint8_t code_size{};
  };

  uint8_t id;
  std::array<uint8_t, MAX_CODE_SIZE> counts{};
  std::vector<uint8_t> symbols;

  std::array<decode_info, 1 << MAX_CODE_SIZE> decode_table{};
  std::array<encode_info, 256> encode_table{};

  void build_decode_table() {
    size_t symbol_idx = 0;
    uint16_t cur_code = 0;
    for (uint8_t code_size = 1; code_size <= MAX_CODE_SIZE; ++code_size) {
      int suffixes_cnt = 1 << (MAX_CODE_SIZE - code_size);
      for (int i = 0; i < counts[code_size - 1]; ++i) {
        for (int j = 0; j < suffixes_cnt; ++j) {
          decode_table[cur_code] = {.symbol = symbols[symbol_idx], .code_size = code_size};
          ++cur_code;
        }
        ++symbol_idx;
      }
    }
  }

  void build_encode_table() {
    size_t symbol_idx = 0;
    uint16_t cur_code = 0;
    for (uint8_t code_size = 1; code_size <= MAX_CODE_SIZE; ++code_size) {
      int suffixes_cnt = 1 << (MAX_CODE_SIZE - code_size);
      for (int i = 0; i < counts[code_size - 1]; ++i) {
        uint16_t shifted_code = cur_code >> (MAX_CODE_SIZE - code_size);
        encode_table[symbols[symbol_idx]] = {.code = shifted_code, .size = code_size};
        cur_code += suffixes_cnt;
        ++symbol_idx;
      }
    }
  }

  explicit huffman_table(std::istream& input) {
    id = input.get();
    size_t total_count = 0;
    for (auto& count : counts) {
      count = input.get();
      total_count += count;
    }
    symbols.resize(total_count);
    input.read(reinterpret_cast<char*>(symbols.data()), total_count);
    build_decode_table();
    build_encode_table();
  }

  void write_internals(std::ostream& output) const {
    output.put(id);
    output.write(reinterpret_cast<char const*>(counts.data()), counts.size());
    output.write(reinterpret_cast<char const*>(symbols.data()), symbols.size());
  }

  [[nodiscard]] uint16_t serialized_length() const noexcept {
    return 1 + counts.size() + symbols.size();
  }
};
