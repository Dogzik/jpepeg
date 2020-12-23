#pragma once

#include <stdexcept>
#include <utility>

#include "model.h"

/**
 * Uniform model for where elements are symbols from 0 to N-1
 */
class uniform_model final : public model {
public:
    explicit uniform_model(math_t N_) : N{N_} {
      if (N > MAX_FREQUENCY) {
        throw std::runtime_error("Number of elements for uniform model exceeds MAX_FREQUENCY");
      }
    }

    std::pair<math_t, math_t> cumulative_frequency(math_t symbol) override {
      if (symbol >= N) {
        throw std::runtime_error("Asking symbol greater than N for uniform model");
      }
      return {symbol, symbol + 1};
    }

    std::pair<math_t, std::pair<math_t, math_t>> get_symbol(math_t cumulative_frequency) override {
      return {cumulative_frequency, {cumulative_frequency, cumulative_frequency + 1}};
    }

    math_t get_total_count() override {
      return N;
    }

    void set_n(math_t new_N) {
      if (new_N > MAX_FREQUENCY) {
        throw std::runtime_error("Number of elements for uniform model exceeds MAX_FREQUENCY");
      }
      N = new_N;
    }

private:
    math_t N;
};
