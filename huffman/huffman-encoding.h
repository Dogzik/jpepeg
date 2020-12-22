#pragma once

#include <ostream>
#include <unordered_map>

#include "huffman/huffman-common.h"
#include "huffman-table.h"

#include "segments/SOF0-segment.h"
#include "segments/SOS-segment.h"

void encode_huffman(std::ostream& output, const SOF0_segment& sof, const SOS_segment& sos,
                    const std::unordered_map<uint8_t, huffman_table>& tables,
                    const std::vector<std::vector<block>>& components);
