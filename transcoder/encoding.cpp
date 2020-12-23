#include "transcoder/encoding.h"

#include <vector>
#include <memory>
#include <sstream>

#include "common-utils.h"

#include "segments/magics.h"

#include "io/multibit-output.h"

#include "numeric/numeric-encoding.h"

namespace {

void encode_size32(std::ostream& output, uint32_t size) {
  for (int i = 32 - 8; i >= 0; i -= 8) {
    output.put((size >> i) & 0xFF);
  }
}

void encode_coefficients(std::ostream& output, const std::vector<uint8_t>& run_levels, const std::vector<uint64_t>& encoded_coefficients) {
  std::stringstream numeric_output(std::ios_base::out | std::ios_base::binary);
  encode_numeric(run_levels, numeric_output);
  auto numeric_str = numeric_output.str();
  encode_size32(output, numeric_str.size());
  output.write(numeric_str.data(), numeric_str.size());
  multibit_output multibit_out(output, false);
  for (size_t i = 0; i < run_levels.size(); ++i) {
    auto size = run_levels[i] & 0x0F; // DC size is always <= 15
    if (size != 0) {
      multibit_out.write_bits(encoded_coefficients[i], size);
    }
  }
}

void encode_dc(std::ostream& output, const std::vector<block>& component) {
  int32_t prev_dc = 0;
  std::vector<uint8_t> dc_sizes;
  std::vector<uint64_t> encoded_dcs;
  dc_sizes.reserve(component.size());
  encoded_dcs.reserve(component.size());
  for (const auto& cur_block : component) {
    auto dc_diff = cur_block[0] - prev_dc;
    auto[code, size] = (dc_diff != 0) ? get_bin_code(cur_block[0] - prev_dc) : std::pair<uint64_t, uint64_t>{0, 0};
    dc_sizes.push_back(size);
    encoded_dcs.push_back(code);
    prev_dc = cur_block[0];
  }
  encode_coefficients(output, dc_sizes, encoded_dcs);
}

void transform_block(std::vector<uint8_t>& run_level, std::vector<uint64_t>& encoded_ac, const block& cur_block) {
  size_t last_ac = 63;
  while ((last_ac > 0) && (cur_block[last_ac] == 0)) {
    --last_ac;
  }
  if (last_ac == 0) {
    run_level.push_back(0);
    encoded_ac.push_back(0);
    return;
  }
  for (size_t i = 1; i <= last_ac; ++i) {
    auto start_pos = i;
    while ((cur_block[i] == 0) && (i <= last_ac)) {
      ++i;
    }
    auto zeros_cnt = i - start_pos;
    for (int j = 0; j < (zeros_cnt >> 4); ++j) {
      run_level.push_back(0xF0);
      encoded_ac.push_back(0);
    }
    zeros_cnt &= 0xF;
    auto[code, size] = get_bin_code(cur_block[i]);
    run_level.push_back((zeros_cnt << 4) + size);
    encoded_ac.push_back(code);
  }
  if (last_ac != 63) {
    run_level.push_back(0);
    encoded_ac.push_back(0);
  }
}

void encode_ac(std::ostream& output, const std::vector<block>& component) {
  std::vector<uint8_t> run_level;
  std::vector<uint64_t> encoded_ac;
  run_level.reserve(component.size());
  encoded_ac.reserve(component.size());
  for (auto const& cur_block : component) {
    transform_block(run_level, encoded_ac, cur_block);
  }
  encode_size32(output, run_level.size());
  encode_coefficients(output, run_level, encoded_ac);
}

}

void encode_component(std::ostream& output, const std::vector<block>& component) {
  encode_dc(output, component);
  encode_ac(output, component);
}
