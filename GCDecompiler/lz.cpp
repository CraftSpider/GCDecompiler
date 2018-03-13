
#include <string>
#include <fstream>
#include "utils.h"
#include "lz.h"

using std::string;
using std::ios;

LZ::LZ(string filename) {
	std::fstream input_r(filename, ios::in | ios::binary);
	std::fstream *input = &input_r;

	if (next_int(input, 8) == 0x447A0000) {
		// Uncompressed LZ
	} else {
		// Compressed LZ
	}
}