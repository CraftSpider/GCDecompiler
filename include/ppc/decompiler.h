#pragma once

#include "types.h"

#include <string>

namespace PPC {

void decompile(const std::string& file_in, const std::string& file_out, int start, int end);

}
