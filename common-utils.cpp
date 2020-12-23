#include "common-utils.h"

std::pair<uint64_t, uint64_t> get_bin_code(int32_t value) {
  uint64_t abs_value = std::abs(value);
  value = (value < 0) ? value - 1 : value;
  uint64_t bits_cnt = 0;
  while (abs_value != 0) {
    bits_cnt += 1;
    abs_value >>= 1;
  }
  return {value & ((1 << bits_cnt) - 1), bits_cnt};
}

int32_t convert_value(int32_t encoded_value, uint8_t size) {
  if (encoded_value & (1 << (size - 1))) {
    return encoded_value;
  } else {
    return encoded_value + (((-1) << size) + 1);
  }
}
