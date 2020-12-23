#pragma once

#include "io/bit-input.h"

class arithmetic_decoder {
public:
    explicit arithmetic_decoder(std::istream &real_in) : in{real_in} {
      for (size_t i = 0; i < CODE_BITS; ++i) {
        value = (value << 1) + in.get_bit();
      }
    }

    math_t decode(model &cur_model, bool need_to_stop) {
      math_t range = high - low + 1;
      math_t denominator = cur_model.get_total_count();
      math_t scaled_value = ((value - low + 1) * denominator - 1) / range;
      auto [symbol, cumulative_frequencies] = cur_model.get_symbol(scaled_value);
      if (need_to_stop) {
        return symbol;
      }
      auto [bottom, top] = cumulative_frequencies;
      high = low + ((range * top) / denominator) - 1;
      low = low + ((range * bottom) / denominator);
      for (;;) {
        if (high < HALF) {
          // do nothing, we just skip common MSB = 0
        } else if (low >= HALF) {
          high -= HALF;
          low -= HALF;
          value -= HALF;
        } else if (ONE_FOURTH <= low && high < TREE_FOURTHS) {
          // move MSB to 2nd SB
          high -= ONE_FOURTH;
          low -= ONE_FOURTH;
          value -= ONE_FOURTH;
        } else {
          break;
        }
        high = (high << 1) | ONE;
        low = low << 1;
        value = (value << 1) + in.get_bit();
      }
      return symbol;
    }

private:
    bit_input in;
    math_t low{0};
    math_t high{MAX_CODE};
    math_t value{0};
};
