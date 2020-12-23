#pragma once

#include <array>
#include <stdexcept>

#include "model.h"

class byte_model final : public model {
    static constexpr size_t SIZE = 256;

public:
    byte_model() {
      for (math_t i = 0; i < SIZE; ++i) {
        add(i + 1, 1);
      }
    }

    explicit byte_model(const std::vector<std::pair<math_t, math_t>> &frequencies) {
      for (auto[frequency, symbol] : frequencies) {
        add(symbol + 1, frequency);
      }
    }

    byte_model(const std::vector<uint8_t> &symbols, const std::vector<math_t> &frequencies) {
      for (size_t i = 0; i < symbols.size(); ++i) {
        add(static_cast<math_t>(symbols[i]) + 1, frequencies[i]);
      }
    }

    std::pair<math_t, math_t> cumulative_frequency(math_t symbol) override {
      if (symbol >= SIZE) {
        throw std::runtime_error("Asking symbol greater than 255 from byte model");
      }
      auto bottom = get(symbol);
      auto top = get(symbol + 1);
      if (top == bottom) {
        throw std::runtime_error("Symbol has zero frequency");
      }
      add(symbol + 1, -1);
      return {bottom, top};
    }

    math_t get_total_count() override {
      return get(SIZE);
    }

    std::pair<math_t, std::pair<math_t, math_t>> get_symbol(math_t cumulative_frequency) override {
      for (math_t i = 0; i < SIZE; ++i) {
        if (cumulative_frequency < get(i + 1)) {
          math_t symbol = i;
          math_t bottom = get(i);
          math_t top = get(i + 1);
          add(i + 1, -1);
          return {symbol, {bottom, top}};
        }
      }
      throw std::runtime_error("Model couldn't find symbol");
    }

private:
    std::array<int64_t, SIZE + 1> fenv{};

    void add(math_t pos, int64_t value) {
      for (; pos < fenv.size(); pos |= (pos + 1)) {
        fenv[pos] += value;
      }
    }

    [[nodiscard]] math_t get(math_t pos) const {
      int64_t res = 0;
      for (int64_t cur = pos; cur >= 0; cur = (cur & (cur + 1)) - 1) {
        res += fenv[cur];
      }
      return res;
    }
};
