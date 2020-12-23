#include "transcoder/decoding.h"

#include <sstream>

#include "common-utils.h"

#include "numeric/numeric-decoding.h"

#include "io/multibit-input.h"

namespace {

uint32_t decode_size32(std::istream& input) {
  uint32_t size = 0;
  for (int i = 0; i < 4; ++i) {
    uint8_t cur_byte = input.get();
    size = (size << 8) | cur_byte;
  }
  return size;
}

std::vector<uint8_t> decode_run_levels(std::istream& input, size_t cnt) {
  auto numeric_input_size = decode_size32(input);
  std::string numeric_input_str(numeric_input_size, 0);
  input.read(numeric_input_str.data(), numeric_input_size);
  std::stringstream numeric_input(std::move(numeric_input_str), std::ios_base::in | std::ios_base::binary);
  return decode_numeric(numeric_input, cnt);
}

void decode_dc(std::istream& input, std::vector<block>& component) {
  auto sizes = decode_run_levels(input, component.size());
  int32_t prev_dc = 0;
  multibit_input multibit_in(input);
  for (size_t i = 0; i < component.size(); ++i) {
    int32_t diff = 0;
    if (sizes[i] != 0) {
      auto encoded_diff = multibit_in.peek_bits(sizes[i]);
      multibit_in.skip_bits(sizes[i]);
      diff = convert_value(encoded_diff, sizes[i]);
    }
    component[i][0] = prev_dc + diff;
    prev_dc = component[i][0];
  }
}

void decode_ac(std::istream& input, std::vector<block>& component) {
  auto cnt = decode_size32(input);
  auto run_levels = decode_run_levels(input, cnt);
  multibit_input multibit_in(input);
  size_t block_idx = 0;
  size_t idx = 0;
  for (auto run_level : run_levels) {
    if (run_level == 0) {
      idx = 0;
      ++block_idx;
      continue;
    }
    auto run = run_level >> 4;
    auto size = run_level & 0xF;
    int32_t ac = 0;
    if (size > 0) {
      auto encoded_ac = multibit_in.peek_bits(size);
      multibit_in.skip_bits(size);
      ac = convert_value(encoded_ac, size);
    }
    idx += run + 1;
    component[block_idx][idx] = ac;
    if (idx == 63) {
      idx = 0;
      ++block_idx;
    }
  }
}

std::vector<block> decode_component(std::istream& input, int32_t macroblocks_cnt, int32_t microblocks_cnt) {
  std::vector<block> res(macroblocks_cnt * microblocks_cnt);
  decode_dc(input, res);
  decode_ac(input, res);
  return res;
}

}

std::vector<std::vector<block>> decode_components(std::istream& input, const SOF0_segment& sof, const SOS_segment& sos) {
  auto[macroblock_width, macroblock_height] = get_macroblock_dimensions(sof);
  auto macroblocks_cnt = macroblock_width * macroblock_height;
  std::vector<std::vector<block>> res;
  res.reserve(3);
  for (size_t idx = 0; idx < sos.components.size(); ++idx) {
    int microblocks_cnt = sof.components[idx].horizontal_sampling * sof.components[idx].vertical_sampling;
    res.push_back(decode_component(input, macroblocks_cnt, microblocks_cnt));
  }
  return res;
}
