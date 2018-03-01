
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "types.h"
#include "dol.h"
#include "utils.h"

using std::string;
using std::vector;
using std::ios;
using std::endl;

DOL::DOL(string filename) {
	std::fstream file_r(filename, ios::binary | ios::in);
	std::fstream *file = &file_r;

	// Read in file Header
	uint offset, address, size;

	for (int i = 0; i < 7; i++) {
		file->seekg(0x0 + i * 4, ios::beg);
		offset = next_int(file);
		file->seekg(0x48 + i * 4, ios::beg);
		address = next_int(file);
		file->seekg(0x90 + i * 4, ios::beg);
		size = next_int(file);
		this->sections.push_back(Section(i, offset, true, size, address));
	}

	for (int i = 0; i < 11; i++) {
		file->seekg(0x1C + i * 4, ios::beg);
		offset = next_int(file);
		file->seekg(0x64 + i * 4, ios::beg);
		address = next_int(file);
		file->seekg(0xAC + i * 4, ios::beg);
		size = next_int(file);
		this->sections.push_back(Section(i + 7, offset, false, size, address));
	}

	this->bss_address = next_int(file);
	this->bss_size = next_int(file);
	this->entry_offset = next_int(file);

}

string DOL::dump_all() {
	std::cout << "Dumping DOL" << endl;
	std::stringstream out;

	out << "Header Data:" << endl;
	out << "  BSS Address: " << itoh(this->bss_address) << endl;
	out << "  BSS Size: " << itoh(this->bss_size) << endl;
	out << "  Entry Point Offset: " << itoh(this->entry_offset) << endl;

	out << "Section Table:" << endl;
	for (vector<Section>::iterator section = this->sections.begin(); section != sections.end(); section++) {
		out << "  Section " << section->id << ":" << endl;
		out << "    Offset: " << itoh(section->offset) << endl;
		out << "    Length: " << itoh(section->length) << endl;
		if (section->offset > 0) {
			out << "    Range: " << itoh(section->offset) << " - " << itoh(section->offset + section->length) << endl;
		}
		out << "    Address: " << itoh(section->address) << endl;
		out << "    Executable: " << section->exec << endl;
	}
	return out.str();
}

void DOL::dump_all(string filename) {
	std::fstream out(filename, ios::out);
	out << this->dump_all();
}