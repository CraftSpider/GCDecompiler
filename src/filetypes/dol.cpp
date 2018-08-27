
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "at_logging"
#include "at_utils"
#include "types.h"
#include "dol.h"

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
		offset = util::next_uint(file);
		file.seekg(0x48 + i * 4, ios::beg);
		address = util::next_uint(file);
		file.seekg(0x90 + i * 4, ios::beg);
		size = util::next_uint(file);
		this->sections.push_back(Section(i, offset, true, size, address));
	}
	
	logger->trace("Reading data sections table");
	for (int i = 0; i < 11; i++) {
		file.seekg(0x1C + i * 4, ios::beg);
		offset = util::next_uint(file);
		file.seekg(0x64 + i * 4, ios::beg);
		address = util::next_uint(file);
		file.seekg(0xAC + i * 4, ios::beg);
		size = util::next_uint(file);
		this->sections.push_back(Section(i + 7, offset, false, size, address));
	}
 
	this->bss_address = util::next_uint(file);
	this->bss_size = util::next_uint(file);
	this->entry_offset = util::next_uint(file);
	
	logger->debug("Finished parsing DOL");
}

std::string DOL::dump_all() {
    logger->trace("Generating DOL dump string");
	std::stringstream out;

	out << "Header Data:\n";
	out << "  BSS Address: \n" << util::itoh(this->bss_address) << '\n';
	out << "  BSS Size: " << util::itoh(this->bss_size) << '\n';
	out << "  Entry Point Offset: " << util::itoh(this->entry_offset) << '\n';

	out << "Section Table:\n";
	for (auto section = this->sections.begin(); section != sections.end(); section++) {
		out << "  Section " << section->id << ":\n";
		out << "    Offset: " << util::itoh(section->offset) << '\n';
		out << "    Length: " << util::itoh(section->length) << '\n';
		if (section->offset > 0) {
			out << "    File Range: " << util::itoh(section->offset) << " - " << util::itoh(section->offset + section->length) << '\n';
		}
		out << "    Address: " << util::itoh(section->address) << '\n';
		if (section->offset > 0) {
			out << "    Mem Range: " << util::itoh(section->address) << " - " << util::itoh(section->address + section->length) << '\n';
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