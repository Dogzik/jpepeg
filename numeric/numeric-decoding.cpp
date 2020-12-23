#include "numeric/numeric-decoding.h"

#include "numeric/model/uniform-model.h"
#include "numeric/model/byte-model.h"
#include "numeric/arithmetic-decoder.h"

namespace {

std::vector<math_t> decode_frequencies(arithmetic_decoder& decoder, math_t data_size) {
  std::vector<math_t> res;
  uniform_model cur_model(data_size);
  while (data_size > 0) {
    math_t new_frequency = decoder.decode(cur_model, false) + 1;
    res.push_back(new_frequency);
    cur_model.set_n(new_frequency);
    data_size -= new_frequency;
  }
  return res;
}

std::vector<uint8_t> decode_letters(arithmetic_decoder& decoder, size_t cnt) {
  std::vector<uint8_t> res(cnt);
  byte_model cur_model{};
  for (size_t i = 0; i < cnt; ++i) {
    math_t raw_symbol = decoder.decode(cur_model, false);
    res[i] = raw_symbol;
  }
  return res;
}

std::vector<uint8_t> decode_data(arithmetic_decoder& decoder, byte_model& data_model, math_t data_size) {
  std::vector<uint8_t> res(data_size);
  for (math_t i = 0; i < data_size; ++i) {
    math_t raw_symbol = decoder.decode(data_model, i == (data_size - 1));
    res[i] = raw_symbol;
  }
  return res;
}

}

std::vector<uint8_t> decode_numeric(std::istream& input, math_t data_size) {
  arithmetic_decoder decoder(input);
  auto frequencies = decode_frequencies(decoder, data_size);
  auto letters = decode_letters(decoder, frequencies.size());
  byte_model data_model(letters, frequencies);
  return decode_data(decoder, data_model, data_size);
}
