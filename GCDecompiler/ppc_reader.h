#pragma once

#include <string>

namespace PPC {

	void decompile(std::string file_in, string file_out, int start = 0, int end = -1);

}