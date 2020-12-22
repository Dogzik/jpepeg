#pragma once

#include <utility>
#include <cstdint>
#include <array>

#include "segments/SOF0-segment.h"

using block = std::array<int32_t, 64>;

std::pair<int32_t, int32_t> get_macroblock_dimensions(const SOF0_segment& sof);
