
#include <fstream>
#include <iostream>
#include "tpl.h"
#include "utils.h"

using std::ios;

TPL::TPL(string filename) {

	std::fstream input_r = std::fstream(filename, ios::in | ios::binary);
	std::fstream *input = &input_r;

	this->identifier = next_int(input, 4);
	this->num_images = next_int(input, 4);
	this->table_offset = next_int(input, 4);

	input->seekg(this->table_offset);

	this->image_table = std::vector<ImageTableEntry>();
	for (int i = 0; i < this->num_images; ++i) {
		uint image_header = next_int(input, 4);
		uint palette_header = next_int(input, 4);
		ImageTableEntry entry = {image_header, palette_header};
		this->image_table.push_back(entry);
	}

	
}