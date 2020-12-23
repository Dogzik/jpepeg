#include "huffman/huffman-encoding.h"

#include <cmath>

#include "io/multibit-output.h"
#include "huffman/huffman-common.h"

#include "common-utils.h"

namespace {

int32_t encode_block(multibit_output& output, int32_t prev_dc, const block& cur_block,
                     const huffman_table& dc_table, const huffman_table& ac_table) {
  auto EOB_info = ac_table.encode_table[0x00];
  auto AC16zeros_info = ac_table.encode_table[0xF0];
  int32_t dc_diff = cur_block[0] - prev_dc;
  if (dc_diff == 0) {
    auto dc_info = dc_table.encode_table[0];
    output.write_bits(dc_info.code, dc_info.size);
  } else {
    auto[code, size] = get_bin_code(dc_diff);
    auto dc_info = dc_table.encode_table[size];
    output.write_bits(dc_info.code, dc_info.size);
    output.write_bits(code, size);
  }
  size_t last_ac = 63;
  while ((last_ac > 0) && (cur_block[last_ac] == 0)) {
    --last_ac;
  }
  if (last_ac == 0) {
    output.write_bits(EOB_info.code, EOB_info.size);
    return cur_block[0];
  }
  for (size_t i = 1; i <= last_ac; ++i) {
    auto start_pos = i;
    while ((cur_block[i] == 0) && (i <= last_ac)) {
      ++i;
    }
    auto zeros_cnt = i - start_pos;
    for (int j = 0; j <  (zeros_cnt >> 4); ++j) {
      output.write_bits(AC16zeros_info.code, AC16zeros_info.size);
    }
    zeros_cnt &= 0xF;
    auto[code, size] = get_bin_code(cur_block[i]);
    auto ac_info = ac_table.encode_table[(zeros_cnt << 4) + size];
    output.write_bits(ac_info.code, ac_info.size);
    output.write_bits(code, size);
  }
  if (last_ac != 63) {
    output.write_bits(EOB_info.code, EOB_info.size);
  }
  return cur_block[0];
}

void encode_segments(std::ostream& output, const SOF0_segment& sof, const SOS_segment& sos,
                     const std::unordered_map<uint8_t, huffman_table>& tables,
                     const std::vector<std::vector<block>>& components) {
  multibit_output huff_output(output, true);
  std::vector<int32_t> prev_dc(sos.components.size(), 0);
  auto[macroblock_width, macroblock_height] = get_macroblock_dimensions(sof);
  for (int i = 0; i < macroblock_height * macroblock_width; ++i) {
    for (size_t idx = 0; idx < sos.components.size(); ++idx) {
      auto component = sos.components[idx];
      int microblocks_cnt = sof.components[idx].horizontal_sampling * sof.components[idx].vertical_sampling;
      for (int j = 0; j < microblocks_cnt; ++j) {
        prev_dc[idx] = encode_block(huff_output, prev_dc[idx], components[idx][i * microblocks_cnt + j],
                                    tables.at(component.dc_table_id), tables.at(component.ac_table_id));
      }
    }
  }
}

}

void encode_huffman(std::ostream& output, const SOF0_segment& sof, const SOS_segment& sos,
                    const std::unordered_map<uint8_t, huffman_table>& tables,
                    const std::vector<std::vector<block>>& components) {
  encode_segments(output, sof, sos, tables, components);
  output.put(std::to_integer<char>(MARKER_MAGIC));
  output.put(std::to_integer<char>(EOI_MAGIC));
}
