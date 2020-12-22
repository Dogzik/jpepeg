#include "huffman-common.h"

std::pair<int32_t, int32_t> get_macroblock_dimensions(const SOF0_segment& sof) {
  int32_t max_h_sampling = 0;
  int32_t max_v_sampling = 0;
  for (auto component: sof.components) {
    max_h_sampling = std::max(max_h_sampling, static_cast<int32_t>(component.horizontal_sampling));
    max_v_sampling = std::max(max_v_sampling, static_cast<int32_t>(component.vertical_sampling));
  }
  int32_t macroblock_width = (sof.width + (max_h_sampling * 8) - 1) / (max_h_sampling * 8);
  int32_t macroblock_height = (sof.height + (max_v_sampling * 8) - 1) / (max_v_sampling * 8);
  return {macroblock_width, macroblock_height};
}
