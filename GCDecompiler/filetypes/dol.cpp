
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "types.h"
#include "dol.h"
#include "utils.h"
#include "a_logging"

namespace types {

using std::ios;

static logging::Logger *logger = logging::get_logger("dol");

DOL::DOL(const std::string& filename) {
    logger->debug("Parsing DOL");
    
	std::fstream file(filename, ios::binary | ios::in);
	this->filename = filename;

	// Read in file Header
	uint offset, address, size;
 
	logger->trace("Reading executable sections table");
	for (int i = 0; i < 7; i++) {
		file.seekg(0x0 + i * 4, ios::beg);
		offset = next_int(file);
		file.seekg(0x48 + i * 4, ios::beg);
		address = next_int(file);
		file.seekg(0x90 + i * 4, ios::beg);
		size = next_int(file);
		this->sections.push_back(Section(i, offset, true, size, address));
	}
	
	logger->trace("Reading data sections table");
	for (int i = 0; i < 11; i++) {
		file.seekg(0x1C + i * 4, ios::beg);
		offset = next_int(file);
		file.seekg(0x64 + i * 4, ios::beg);
		address = next_int(file);
		file.seekg(0xAC + i * 4, ios::beg);
		size = next_int(file);
		this->sections.push_back(Section(i + 7, offset, false, size, address));
	}
 
	this->bss_address = next_int(file);
	this->bss_size = next_int(file);
	this->entry_offset = next_int(file);
	
	logger->debug("Finished parsing DOL");
}

std::string DOL::dump_all() {
    logger->trace("Generating DOL dump string");
	std::stringstream out;

	out << "Header Data:\n";
	out << "  BSS Address: \n" << itoh(this->bss_address) << '\n';
	out << "  BSS Size: " << itoh(this->bss_size) << '\n';
	out << "  Entry Point Offset: " << itoh(this->entry_offset) << '\n';

	out << "Section Table:\n";
	for (auto section = this->sections.begin(); section != sections.end(); section++) {
		out << "  Section " << section->id << ":\n";
		out << "    Offset: " << itoh(section->offset) << '\n';
		out << "    Length: " << itoh(section->length) << '\n';
		if (section->offset > 0) {
			out << "    File Range: " << itoh(section->offset) << " - " << itoh(section->offset + section->length) << '\n';
		}
		out << "    Address: " << itoh(section->address) << '\n';
		if (section->offset > 0) {
			out << "    Mem Range: " << itoh(section->address) << " - " << itoh(section->address + section->length) << '\n';
		}
		out << "    Executable: " << section->exec << '\n';
	}
	return out.str();
}

void DOL::dump_all(const std::string& filename) {
    logger->debug("Dumping DOL to " + filename);
	std::fstream out(filename, ios::out);
	out << this->dump_all();
	logger->debug("DOL Dump complete");
}

}