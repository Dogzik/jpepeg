#pragma once

#include <cstdint>
#include <utility>
#include <cmath>

std::pair<uint64_t, uint64_t> get_bin_code(int32_t value);

int32_t convert_value(int32_t encoded_value, uint8_t size);
