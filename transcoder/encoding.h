#pragma once

#include <istream>
#include <ostream>
#include <vector>

#include "huffman/huffman-common.h"

void encode_component(std::ostream& output, const std::vector<block>& component);
