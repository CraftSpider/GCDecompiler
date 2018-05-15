#pragma once

#include <string>
#include <vector>
#include "types.h"

using std::string;

struct ImageTableEntry {
	uint image_header, palette_header;
};

struct PaletteHeader {
	char unpacked;
	uint entry_count, format, address;
};

struct ImageHeader {
	uint height, width, format, address, wrap_s, wrap_t, min_filter, mag_filter, lod_bias, edge_lod_enable, min_lod, max_lod, unpacked;
};

class TPL {

	uint identifier, num_images, table_offset;
	std::vector<ImageTableEntry> image_table;

public:

	TPL(string filename);

};
