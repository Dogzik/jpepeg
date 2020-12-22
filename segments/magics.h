#pragma once

#include <cstddef>

constexpr inline std::byte MARKER_MAGIC{0xFF};

constexpr inline std::byte COM_MAGIC{0xFE};
constexpr inline std::byte DQT_MAGIC{0xDB};
constexpr inline std::byte SOF0_MAGIC{0xC0};
constexpr inline std::byte DHT_MAGIC{0xC4};
constexpr inline std::byte SOI_MAGIC{0xD8};
constexpr inline std::byte EOI_MAGIC{0xD9};
constexpr inline std::byte SOS_MAGIC{0xDA};
