#include "numeric/numeric-encoding.h"

#include <vector>
#include <array>
#include <algorithm>

#include "numeric/model/constants.h"
#include "numeric/model/uniform-model.h"
#include "numeric/model/byte-model.h"
#include "numeric/arithmetic-encoder.h"

namespace {

std::vector<std::pair<math_t, math_t>> count_frequencies(const std::vector<uint8_t>& data) {
  std::array<math_t, 256> frequencies{};
  for (auto elem : data) {
    ++frequencies[elem];
  }
  std::vector<std::pair<math_t, math_t>> res;
  res.reserve(256);
  for (math_t i = 0; i < 256; ++i) {
    if (frequencies[i] != 0) {
      res.emplace_back(frequencies[i], i);
    }
  }
  std::sort(res.begin(), res.end(), std::greater<>());
  return res;
}

void encode_frequencies(arithmetic_encoder& encoder, math_t data_size, const std::vector<std::pair<math_t, math_t>>& frequencies) {
  uniform_model cur_model(data_size);
  for (auto[frequency, symbol] : frequencies) {
    encoder.encode(frequency - 1, cur_model);
    cur_model.set_n(frequency);
    data_size -= frequency;
  }
}

void encode_letters(arithmetic_encoder& encoder, const std::vector<std::pair<math_t, math_t>>& frequencies) {
  byte_model cur_model{};
  for (auto[frequency, symbol] : frequencies) {
    encoder.encode(symbol, cur_model);
  }
}

void encode_data(arithmetic_encoder& encoder, byte_model& data_model, const std::vector<uint8_t>& data) {
  for (auto elem : data) {
    encoder.encode(elem, data_model);
  }
}

}

void encode_numeric(const std::vector<uint8_t>& data, std::ostream &output) {
  if (data.size() > MAX_FREQUENCY) {
    throw std::runtime_error("Stream too big to encode_numeric. Its size exceeds MAX_FREQUENCY");
  }
  auto frequencies = count_frequencies(data);
  arithmetic_encoder encoder(output);
  encode_frequencies(encoder, data.size(), frequencies);
  encode_letters(encoder, frequencies);
  byte_model text_model(frequencies);
  encode_data(encoder, text_model, data);
}

