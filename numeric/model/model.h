#pragma once

#include <utility>

#include "constants.h"

struct model {
    virtual std::pair<math_t, math_t> cumulative_frequency(math_t symbol) = 0;

    virtual math_t get_total_count() = 0;

    virtual std::pair<math_t, std::pair<math_t, math_t>> get_symbol(math_t cumulative_frequency) = 0;

    virtual ~model() = default;
};
