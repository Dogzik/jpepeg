#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <istream>
#include <array>

#include "huffman/huffman-table.h"

#include "segments/SOS-segment.h"
#include "segments/SOF0-segment.h"

using block = std::array<int32_t, 64>;

std::vector<std::vector<block>> decode_huffman(std::istream& input, const SOF0_segment& sof, const SOS_segment& sos,
                                               const std::unordered_map<uint8_t, huffman_table>& tables);
