
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "at_logging"
#include "at_utils"
#include "filetypes/rel.h"

namespace types {

using std::ios;

static logging::Logger *logger = logging::get_logger("rel");

REL::REL(const std::string& filename) {
	logger->debug("Parsing REL");
	
	std::fstream file(filename, ios::binary | ios::in | ios::ate);
	this->filename = filename;
	this->file_size = (uint)file.tellg();

	logger->trace("Reading file header");
	file.seekg(0, ios::beg);
	this->id = util::next_uint(file);
	file.seekg(8, ios::cur); // Skip over the next and previous module values
	uint num_sections = util::next_uint(file);
	uint section_offset = util::next_uint(file);
	this->name_offset = util::next_uint(file);
	this->name_size = util::next_uint(file);
	this->version = util::next_uint(file);
	this->bss_size = util::next_uint(file);
	file.seekg(4, ios::cur); // Because we don't need to know the relocation offset
	uint import_offset = util::next_uint(file);
	uint num_imports = util::next_uint(file) / 8; // Convert length of imports to number of imports
	this->prolog_section = util::next_uchar(file);
	this->epilog_section = util::next_uchar(file);
	this->unresolved_section = util::next_uchar(file);
	file.seekg(1, ios::cur); // Skip padding
	this->prolog_offset = util::next_uint(file);
	this->epilog_offset = util::next_uint(file);
	this->unresolved_offset = util::next_uint(file);
	if (this->version >= 2) {
		this->align = util::next_uint(file);
		this->bss_align = util::next_uint(file);
	}
	if (this->version >= 3) {
		this->fix_size = util::next_uint(file);
	}
	this->header_size = (uint)file.tellg();

	logger->trace("Reading section table");
	file.seekg(section_offset, ios::beg);
	for (uint i = 0; i < num_sections; i++) {
		uint offset = util::next_uint(file);
		bool exec = offset & 1u;
		offset = offset >> 1u << 1u;
		uint length = util::next_uint(file);
		this->sections.emplace_back(Section(i, offset, exec, length));
	}

	logger->trace("Reading section data");
	for (auto& section : this->sections) {
		if (section.offset != 0) {
			file.seekg(section.offset, ios::beg);
			char *data = new char[section.length];
			file.read(data, section.length);
			section.set_data(data);
		}
	}

	logger->trace("Reading import table");
	file.seekg(import_offset, ios::beg);
	for (uint i = 0; i < num_imports; i++) {
		uint module_id = util::next_uint(file);
		uint offset = util::next_uint(file);
		this->imports.emplace_back(Import(module_id, offset));
	}
	
	logger->trace("Reading relocation table");
	for (auto& imp : this->imports) {
		file.seekg(imp.offset, ios::beg);
		RelType rel_type = RelType(0);
		while (rel_type != R_RVL_STOP) {
			uint position = (uint)file.tellg();
			ushort prev_offset = util::next_ushort(file);
			rel_type = RelType(util::next_uchar(file));
			Section *section = &this->sections.at(util::next_uchar(file));
			uint rel_offset = util::next_uint(file);
			imp.add_relocation(rel_type, position, rel_offset, prev_offset, section);
		}
	}
	
	logger->debug("Finished parsing REL");
}

uint REL::num_sections() {
	return (uint)this->sections.size();
}

uint REL::num_imports() {
	return (uint)this->imports.size();
}

uint REL::num_relocations() {
	uint out = 0;
	for (auto imp = this->imports.begin(); imp != imports.end(); imp++) {
		out += (uint)imp->instructions.size();
	}
	return out;
}

uint REL::section_offset() {
	return header_size;
}

uint REL::import_offset() {
	uint out = this->relocation_offset();
	for (auto& imp : this->imports) {
		out += (uint)imp.instructions.size() * 8;
	}
	return out;
}

uint REL::relocation_offset() {
	uint out = 0;
	out += this->header_size;
	out += this->num_sections() * 8;
	for (auto& section : this->sections) {
		if (section.offset != 0) {
			out += section.length;
		}
	}
	out += 16;
	return out;
}

void REL::compile(const std::string& file_out) {
	logger->info("Compiling REL file " + std::to_string(this->id));
	std::fstream out(file_out, ios::out | ios::binary);
	// Recalculate any necessary numbers for offsets

	// Write Header to the file
	logger->debug("Writing header");
	util::write_uint(out, this->id);
    util::write_uint(out, 0, 8); // Padding for Prev and Next module addresses.
    util::write_uint(out, this->num_sections());
    util::write_uint(out, this->section_offset());
    util::write_uint(out, this->name_offset);
    util::write_uint(out, this->name_size);
    util::write_uint(out, this->version);
    util::write_uint(out, this->bss_size);
    util::write_uint(out, this->relocation_offset());
    util::write_uint(out, this->import_offset());
    util::write_uint(out, this->num_imports() * 8); // Convert number of imports to length of imports
    util::write_uint(out, this->prolog_section, 1);
    util::write_uint(out, this->epilog_section, 1);
    util::write_uint(out, this->unresolved_section, 1);
    util::write_uint(out, 0, 1); // Padding for 8 byte alignment
    util::write_uint(out, this->prolog_offset);
    util::write_uint(out, this->epilog_offset);
    util::write_uint(out, this->unresolved_offset);
	if (this->version >= 2) {
        util::write_uint(out, this->align);
        util::write_uint(out, this->bss_align);
	}
	if (this->version >= 3) {
        util::write_uint(out, this->fix_size);
	}

	// Write Section Table to the file
	logger->debug("Writing section table");
	out.seekp(this->section_offset(), ios::beg);
	for (uint i = 0; i < this->num_sections(); i++) {
		Section section = this->sections.at(i);
        util::write_uint(out, section.offset | (int)section.exec); // Add exec bit back in
        util::write_uint(out, section.length);
	}

	// Write actual sections to the file
	logger->debug("Writing section data");
	for (auto& section : this->sections) {
		if (section.offset != 0) {
			out.seekp(section.offset, ios::beg);
			out.write(section.get_data(), section.length);
		}
	}

	// Write the Import Table
	logger->debug("Writing import table");
	out.seekp(this->import_offset(), ios::beg);
	for (uint i = 0; i < this->num_imports(); i++) {
		Import imp = this->imports.at(i);
        util::write_uint(out, imp.module);
        util::write_uint(out, imp.offset);
	}

	// Write the Relocation Instructions
	logger->debug("Writing relocation instructions");
	for (auto& imp : this->imports) {
		out.seekp(imp.offset, ios::beg);
		for (auto& reloc : imp.instructions) {
            util::write_uint(out, reloc.prev_offset, 2);
            util::write_uint(out, reloc.type, 1);
            util::write_uint(out, reloc.get_src_section().id, 1);
            util::write_uint(out, reloc.relative_offset);
		}
	}
	
	logger->info("REL compile complete");
}

std::string REL::dump_header(uint pad_len) {
	logger->trace("Generating REL header dump string");
	std::string padding(pad_len, ' ');
	std::stringstream out;
	out << "REL Header:" << '\n';
	out << "  ID: " << this->id << '\n';
	out << "  Version: " << this->version << '\n';
	out << "  Name Offset: " << util::itoh(this->name_offset) << '\n';
	out << "  Name Size: " << util::itoh(this->name_size) << '\n';
	out << "  .bss Size: " << util::itoh(this->bss_size) << '\n';
	out << "  Sections Start: " << util::itoh(this->section_offset()) << '\n';
	out << "  Num Sections: " << this->num_sections() << '\n';
	out << "  Import Start: " << util::itoh(this->import_offset()) << '\n';
	out << "  Num Imports: " << this->num_imports() << '\n';
	out << "  Relocation Start: " << util::itoh(this->relocation_offset()) << '\n';
	out << "  Num Relocations: " << this->num_relocations() << '\n';
	out << "  Prolog Index: " << this->prolog_section << '\n';
	out << "  Prolog Offset: " << util::itoh(this->prolog_offset) << '\n';
	out << "  Epilog Index: " << this->epilog_section << '\n';
	out << "  Epilog Offset: " << util::itoh(this->epilog_offset) << '\n';
	out << "  Unresolved Index: " << this->unresolved_section << '\n';
	out << "  Unresolved Offset: " << util::itoh(this->unresolved_offset) << '\n';
	if (this->version >= 2) {
		out << "  Align: " << this->align << '\n';
		out << "  .bss Align: " << this->bss_align << '\n';
	}
	if (this->version >= 3) {
		out << "  Fix Size: " << this->fix_size << '\n';
	}
	return out.str();
}

void REL::dump_header(const std::string& filename) {
	logger->debug("Dumping REL header to " + filename);
	std::fstream out(filename, ios::out);
	out << this->dump_header();
	logger->debug("REL header dump complete");
}

std::string REL::dump_sections(uint pad_len) {
	logger->trace("Generating REL section dump string");
	std::string padding(pad_len, ' ');
	logger->info(padding + "Dumping REL Sections");
	std::stringstream out;
	out << "Section Table:" << '\n';
	for (auto section = this->sections.begin(); section != sections.end(); section++) {
		out << "  Section " << section->id << ":" << '\n';
		out << "    Offset: " << util::itoh(section->offset) << '\n';
		out << "    Length: " << util::itoh(section->length) << '\n';
		if (section->offset > 0) {
			out << "    Range: " << util::itoh(section->offset) << " - " << util::itoh(section->offset + section->length) << '\n';
		}
		out << "    Executable: " << section->exec << '\n';
	}
	return out.str();
}

void REL::dump_sections(const std::string& filename) {
	logger->debug("Dumping REL sections to " + filename);
	std::fstream out(filename, ios::out);
	out << this->dump_sections();
	logger->debug("REL section dump complete");
}

std::string REL::dump_imports(uint pad_len) {
	logger->trace("Generating REL import dump string");
	std::string padding(pad_len, ' ');
	std::stringstream out;
	out << "Import Table:" << '\n';
	for (auto& imp : this->imports) {
	    std::stringstream temp;
	    temp << padding << "  Dumping Import " << imp.module;
		logger->info(temp.str());
		out << "  Import:" << '\n';
		out << "    Module: " << imp.module << '\n';
		out << "    Offset: " << util::itoh(imp.offset) << '\n';
		out << "    Relocation Table:" << '\n';
		for (auto& reloc : imp.instructions) {
			out << "      Relocation:" << '\n';
			out << "        Position: " << util::itoh(reloc.position) << '\n';
			out << "        Type: " << RelNames.at(reloc.type) << '\n';
			if (reloc.type == R_RVL_STOP) {
				continue;
			}
			if (reloc.type == R_RVL_SECT) {
				out << "        Destination Section: " << reloc.get_dest_section().id << '\n';
				continue;
			}
			out << "        Offset from Prev: " << util::itoh(reloc.prev_offset) << '\n';
			out << "        Source: " << reloc.get_src_section().id << " " << util::itoh(reloc.get_src_offset()) << '\n';
			out << "        Destination: " << reloc.get_dest_section().id << " " << util::itoh(reloc.get_dest_offset()) << '\n';
		}
	}
	return out.str();
}

void REL::dump_imports(const std::string& filename) {
	logger->debug("Dumping REL imports to " + filename);
	std::fstream out(filename, ios::out);
	out << this->dump_imports();
	logger->debug("REL imports dump complete");
}

std::string REL::dump_all() {
	logger->trace("Generating REL complete dump string");
	std::stringstream out;
	out << dump_header() << '\n';
	out << dump_sections() << '\n';
	out << dump_imports() << '\n';
	return out.str();
}

void REL::dump_all(const std::string& filename) {
	logger->debug("Dumping complete REL to " + filename);
	std::fstream out(filename, ios::out);
	out << dump_header(2) << '\n';
	out << dump_sections(2) << '\n';
	out << dump_imports(2) << '\n';
	logger->debug("Complete REL dump complete");
}

}