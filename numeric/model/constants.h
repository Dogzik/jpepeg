#pragma once

#include <cstddef>
#include <cstdint>

using math_t = uint64_t;

constexpr inline math_t ONE = 1;
constexpr inline math_t FREQUENCY_BITS = 31;
constexpr inline math_t CODE_BITS = 33;
static_assert(CODE_BITS - FREQUENCY_BITS >= 2, "Don't have enough code precision");
static_assert(CODE_BITS + FREQUENCY_BITS <= 64, "Math wouldn't fit in underlying integer type");

constexpr inline math_t MAX_FREQUENCY = (ONE << FREQUENCY_BITS) - ONE;
constexpr inline math_t MAX_CODE = (ONE << CODE_BITS) - ONE;

constexpr inline math_t HALF = ONE << (CODE_BITS - ONE);
constexpr inline math_t ONE_FOURTH = HALF >> 1;
constexpr inline math_t TREE_FOURTHS = ONE_FOURTH * 3;

static constexpr std::byte TOP_BIT_BYTE{1 << 7};
static constexpr std::byte BYTE_ZERO{0};
static constexpr std::byte BYTE_ONE{1};
