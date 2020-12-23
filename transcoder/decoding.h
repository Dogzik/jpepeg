#pragma once

#include <vector>
#include <istream>

#include "huffman/huffman-common.h"

#include "segments/SOF0-segment.h"
#include "segments/SOS-segment.h"

std::vector<std::vector<block>> decode_components(std::istream& input, const SOF0_segment& sof, const SOS_segment& sos);
