#include "transcoder/processing.h"

#include <vector>
#include <memory>
#include <unordered_map>

#include "segments/magics.h"
#include "segments/segment.h"
#include "segments/SOS-segment.h"
#include "segments/SOF0-segment.h"
#include "segments/COM-segment.h"
#include "segments/DQT-segment.h"
#include "segments/DHT-segment.h"
#include "segments/APP-segment.h"

#include "huffman/huffman-decoding.h"
#include "huffman/huffman-encoding.h"

namespace {

std::vector<std::vector<block>> get_components(process_mode mode, std::istream& input,
                                               const SOF0_segment& sof, const SOS_segment& sos,
                                               const std::unordered_map<uint8_t, huffman_table>& tables) {
  switch (mode) {
    case process_mode::ENCODE:
      return decode_huffman(input, sof, sos, tables);
    case process_mode::DECODE:
      // TODO: change
      return decode_huffman(input, sof, sos, tables);
    default:
      return {};
  }
}

}

void process(std::istream& input, std::ostream& output, process_mode mode) {
  std::byte marker[2];
  input.read(reinterpret_cast<char*>(marker), 2);
  if ((marker[0] != MARKER_MAGIC) || (marker[1] != SOI_MAGIC)) {
    throw std::runtime_error("Expected SOI segment at the beginning");
  }
  std::vector<std::unique_ptr<segment>> segments;
  std::unordered_map<uint8_t, huffman_table> tables;
  SOS_segment* sos_ptr = nullptr;
  SOF0_segment* sof_ptr = nullptr;
  for (bool stop = false; !stop;) {
    input.read(reinterpret_cast<char*>(marker), 2);
    if (marker[0] != MARKER_MAGIC) {
      throw std::runtime_error("Expected marker magic");
    }
    switch (marker[1]) {
      case COM_MAGIC: {
        segments.emplace_back(new COM_segment(input));
        break;
      }
      case DQT_MAGIC: {
        segments.emplace_back(new DQT_segment(input));
        break;
      }
      case SOF0_MAGIC: {
        sof_ptr = new SOF0_segment(input);
        segments.emplace_back(sof_ptr);
        break;
      }
      case DHT_MAGIC: {
        auto* dht_ptr = new DHT_segment(input);
        for (const auto& table : dht_ptr->tables) {
          tables.insert_or_assign(table.id, table);
        }
        segments.emplace_back(dht_ptr);
        break;
      }
      case SOS_MAGIC: {
        sos_ptr = new SOS_segment(input);
        if (sos_ptr->components.size() != sof_ptr->components.size()) {
          throw std::runtime_error("SOS and SOF segments detonate different number of components");
        }
        for (size_t i = 0; i < sos_ptr->components.size(); ++i) {
          if (sos_ptr->components[i].id != sof_ptr->components[i].id) {
            throw std::runtime_error("SOS has component with different id");
          }
        }
        segments.emplace_back(sos_ptr);
        stop = true;
        break;
      }
      default: {
        if (APP_segment::is_valid_magic(marker[1])) {
          segments.emplace_back(new APP_segment(input, marker[1]));
        } else {
          throw std::runtime_error("Invalid marker magic");
        }
        break;
      }
    }
  }
  auto components = get_components(mode, input, *sof_ptr, *sos_ptr, tables);
  output.put(std::to_integer<char>(MARKER_MAGIC));
  output.put(std::to_integer<char>(SOI_MAGIC));
  for (const auto& cur_segment : segments) {
    cur_segment->write(output);
  }
  switch (mode) {
    case process_mode::ENCODE: {
      // TODO: change
      encode_huffman(output, *sof_ptr, *sos_ptr, tables, components);
      break;
    }
    case process_mode::DECODE: {
      encode_huffman(output, *sof_ptr, *sos_ptr, tables, components);
      break;
    }
  }
}
