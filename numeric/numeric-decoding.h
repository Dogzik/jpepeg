#pragma once

#include <vector>
#include <istream>

#include "numeric/model/constants.h"

std::vector<uint8_t> decode_numeric(std::istream& input, math_t data_size);
