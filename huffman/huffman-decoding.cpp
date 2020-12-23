#include "huffman/huffman-decoding.h"

#include <vector>
#include <istream>
#include <unordered_map>
#include <cstdio>

#include "common-utils.h"

#include "huffman/huffman-input.h"
#include "huffman/huffman-table.h"
#include "huffman/huffman-common.h"

#include "segments/magics.h"
#include "segments/SOS-segment.h"
#include "segments/SOF0-segment.h"


namespace {

std::vector<std::byte> get_decoding_data(std::istream& input) {
  std::vector<std::byte> res;
  for (auto cur_byte = static_cast<std::byte>(input.get());; cur_byte = static_cast<std::byte>(input.get())) {
    if (input.eof()) {
      throw std::runtime_error("Unexpected end of input for huffman");
    }
    if (cur_byte == MARKER_MAGIC) {
      auto next_byte = static_cast<std::byte>(input.get());
      if (input.eof()) {
        throw std::runtime_error("No bytes after control sequence");
      }
      if (next_byte == EOI_MAGIC) {
        break;
      }
      if (next_byte != std::byte{0}) {
        throw std::runtime_error("Expected 0x00 or EOI after control sequence");
      }
    }
    res.push_back(cur_byte);
  }
  return res;
}

std::pair<huffman_table::decode_info, int32_t> decode_value(huffman_input& input, const huffman_table& table) {
  auto input_bits = input.peek_bits(16);
  auto info = table.decode_table[input_bits];
  input.skip_bits(info.code_size);
  auto value_size = info.symbol & 0x0F; // DC len <= 15, and AC len is stored in 4 LSB
  if (value_size > 0) {
    auto value = convert_value(input.peek_bits(value_size), value_size);
    input.skip_bits(value_size);
    return {info, value};
  } else {
    return {info, 0};
  }
}

block decode_block(huffman_input& input, int32_t prev_dc,
                   const huffman_table& dc_table, const huffman_table& ac_table) {
  block res{};
  res[0] = prev_dc + decode_value(input, dc_table).second;
  for (int idx = 0; idx < 63;) {
    auto[info, value] = decode_value(input, ac_table);
    if (info.symbol == 0) { // EOB
      break;
    }
    auto run = info.symbol >> 4;
    idx += run + 1;
    res[idx] = value;
  }
  return res;
}

}

std::vector<std::vector<block>> decode_huffman(std::istream& input, const SOF0_segment& sof, const SOS_segment& sos,
                                               const std::unordered_map<uint8_t, huffman_table>& tables) {
  huffman_input huff_input(get_decoding_data(input));
  std::vector<std::vector<block>> res(sos.components.size());
  std::vector<int32_t> prev_dc(sos.components.size(), 0);
  auto[macroblock_width, macroblock_height] = get_macroblock_dimensions(sof);
  for (int i = 0; i < macroblock_height * macroblock_width; ++i) {
    for (size_t idx = 0; idx < sos.components.size(); ++idx) {
      auto component = sos.components[idx];
      int microblocks_cnt = sof.components[idx].horizontal_sampling * sof.components[idx].vertical_sampling;
      for (int j = 0; j < microblocks_cnt; ++j) {
        auto block = decode_block(huff_input, prev_dc[idx], tables.at(component.dc_table_id),
                                  tables.at(component.ac_table_id));
        prev_dc[idx] = block[0];
        res[idx].push_back(block);
      }
    }
  }
  return res;
}
