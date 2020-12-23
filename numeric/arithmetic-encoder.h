#pragma once

#include "io/bit-output.h"
#include "model/model.h"

class arithmetic_encoder {
public:
    explicit arithmetic_encoder(std::ostream &real_out) noexcept: out(real_out) {};

    void encode(math_t symbol, model &cur_model) {
      math_t denominator = cur_model.get_total_count();
      auto[bottom, top] = cur_model.cumulative_frequency(symbol);
      math_t range = high - low + 1;
      high = low + ((range * top) / denominator) - 1;
      low = low + ((range * bottom) / denominator);
      for (;;) {
        if (high < HALF) { // output and erase common MSB = 0
          output_with_pending(false);
        } else if (low >= HALF) { // output and erase common MSB = 1
          output_with_pending(true);
          high -= HALF;
          low -= HALF;
        } else if (ONE_FOURTH <= low && high < TREE_FOURTHS) { // high = 10xxx, low = 01yyy, erase 2nd SB
          ++pending_bits;
          high -= ONE_FOURTH;
          low -= ONE_FOURTH;
        } else {
          break;
        }
        high = (high << 1) | ONE;
        low = low << 1;
      }
    }

    /**
     * At the end we have one of three states:
     * 1) high=11xxx low=01yyy
     * 2) high=11xxx low=00yyy
     * 3) high=10xxx low=00yyy
     * So we have to output some bits, to make output value between low and high
     * In cases 2 and 3 we could just output "01", and in case 1 - "10"
     * However there may be some pending bits that we have to flush,
     * so we simply increase "pending_bits" to ensure output of at least one
     * extra by "output_with_pending"
     */
    ~arithmetic_encoder() {
      ++pending_bits;
      if (low < ONE_FOURTH) {
        output_with_pending(false);
      } else {
        output_with_pending(true);
      }
    }

private:
    bit_output out;
    size_t pending_bits{0};
    math_t high{MAX_CODE};
    math_t low{0};

    void output_with_pending(bool bit) {
      out.put_bit(bit);
      for (size_t i = 0; i < pending_bits; ++i) {
        out.put_bit(!bit);
      }
      pending_bits = 0;
    }
};
