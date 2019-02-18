#pragma once

#include <string>

namespace PPC {

void disassemble(const std::string& input, const std::string& output, int start = 0, int end = -1, bool info = true);

}
