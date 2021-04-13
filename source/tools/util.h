#pragma once

#include <vector>
#include <string>
#include <utility>

const float ERROR = 0.001f;

namespace util {

std::vector<std::string> split(const std::string& str, const std::string& delim);
bool equals(float f0, float f1);

} // util

