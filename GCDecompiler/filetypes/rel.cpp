
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "logging.h"
#include "rel.h"
#include "utils.h"

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
	this->id = next_int(file);
	file.seekg(8, ios::cur); // Skip over the next and previous module values
	uint num_sections = next_int(file);
	uint section_offset = next_int(file);
	this->name_offset = next_int(file);
	this->name_size = next_int(file);
	this->version = next_int(file);
	this->bss_size = next_int(file);
	file.seekg(4, ios::cur); // Because we don't need to know the relocation offset
	uint import_offset = next_int(file);
	uint num_imports = next_int(file) / 8; // Convert length of imports to number of imports
	this->prolog_section = next_char(file);
	this->epilog_section = next_char(file);
	this->unresolved_section = next_char(file);
	file.seekg(1, ios::cur); // Skip padding
	this->prolog_offset = next_int(file);
	this->epilog_offset = next_int(file);
	this->unresolved_offset = next_int(file);
	if (this->version >= 2) {
		this->align = next_int(file);
		this->bss_align = next_int(file);
	}
	if (this->version >= 3) {
		this->fix_size = next_int(file);
	}
	this->header_size = (uint)file.tellg();

	logger->trace("Reading section table");
	file.seekg(section_offset, ios::beg);
	for (uint i = 0; i < num_sections; i++) {
		uint offset = next_int(file);
		bool exec = offset & 1;
		offset = offset >> 1 << 1;
		uint length = next_int(file);
		this->sections.push_back(Section(i, offset, exec, length));
	}

	logger->trace("Reading section data");
	for (auto section = this->sections.begin(); section != this->sections.end(); section++) {
		if (section->offset != 0) {
			file.seekg(section->offset, ios::beg);
			char *data = new char[section->length];
			file.read(data, section->length);
			section->set_data(data);
		}
	}

	logger->trace("Reading import table");
	file.seekg(import_offset, ios::beg);
	for (uint i = 0; i < num_imports; i++) {
		uint module_id = next_int(file);
		uint offset = next_int(file);
		this->imports.push_back(Import(module_id, offset));
	}
	
	logger->trace("Reading relocation table");
	for (auto imp = this->imports.begin(); imp != this->imports.end(); imp++) {
		file.seekg(imp->offset, ios::beg);
		RelType rel_type = RelType(0);
		while (rel_type != R_RVL_STOP) {
			uint position = (uint)file.tellg();
			ushort prev_offset = next_short(file);
			rel_type = RelType(next_char(file));
			Section *section = &this->sections.at(next_char(file));
			uint rel_offset = next_int(file);
			imp->add_relocation(rel_type, position, rel_offset, prev_offset, section);
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
	for (auto imp = this->imports.begin(); imp != imports.end(); imp++) {
		out += (uint)imp->instructions.size() * 8;
	}
	return out;
}

uint REL::relocation_offset() {
	uint out = 0;
	out += this->header_size;
	out += this->num_sections() * 8;
	for (auto section = this->sections.begin(); section != this->sections.end(); section++) {
		if (section->offset != 0) {
			out += section->length;
		}
	}
	out += 16;
	return out;
}

void REL::compile(const std::string& filename) {
	logger->info("Compiling REL file " + this->id);
	std::fstream out(filename, ios::out | ios::binary);
	// Recalculate any necessary numbers for offsets

	// Write Header to the file
	logger->debug("Writing header");
	write_int(out, this->id);
	write_int(out, 0, 8); // Padding for Prev and Next module addresses.
	write_int(out, this->num_sections());
	write_int(out, this->section_offset());
	write_int(out, this->name_offset);
	write_int(out, this->name_size);
	write_int(out, this->version);
	write_int(out, this->bss_size);
	write_int(out, this->relocation_offset());
	write_int(out, this->import_offset());
	write_int(out, this->num_imports() * 8); // Convert number of imports to length of imports
	write_int(out, this->prolog_section, 1);
	write_int(out, this->epilog_section, 1);
	write_int(out, this->unresolved_section, 1);
	write_int(out, 0, 1); // Padding for 8 byte alignment
	write_int(out, this->prolog_offset);
	write_int(out, this->epilog_offset);
	write_int(out, this->unresolved_offset);
	if (this->version >= 2) {
		write_int(out, this->align);
		write_int(out, this->bss_align);
	}
	if (this->version >= 3) {
		write_int(out, this->fix_size);
	}

	// Write Section Table to the file
	logger->debug("Writing section table");
	out.seekp(this->section_offset(), ios::beg);
	for (uint i = 0; i < this->num_sections(); i++) {
		Section section = this->sections.at(i);
		write_int(out, section.offset | (int)section.exec); // Add exec bit back in
		write_int(out, section.length);
	}

	// Write actual sections to the file
	logger->debug("Writing section data");
	for (auto section = this->sections.begin(); section != this->sections.end(); section++) {
		if (section->offset != 0) {
			out.seekp(section->offset, ios::beg);
			out.write(section->get_data(), section->length);
		}
	}

	// Write the Import Table
	logger->debug("Writing import table");
	out.seekp(this->import_offset(), ios::beg);
	for (uint i = 0; i < this->num_imports(); i++) {
		Import imp = this->imports.at(i);
		write_int(out, imp.module);
		write_int(out, imp.offset);
	}

	// Write the Relocation Instructions
	logger->debug("Writing relocation instructions");
	for (auto imp = this->imports.begin(); imp != this->imports.end(); imp++) {
		out.seekp(imp->offset, ios::beg);
		for (auto reloc = imp->instructions.begin(); reloc != imp->instructions.end(); reloc++) {
			write_int(out, reloc->prev_offset, 2);
			write_int(out, reloc->type, 1);
			write_int(out, reloc->get_src_section().id, 1);
			write_int(out, reloc->relative_offset);
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
	out << "  Name Offset: " << itoh(this->name_offset) << '\n';
	out << "  Name Size: " << itoh(this->name_size) << '\n';
	out << "  .bss Size: " << itoh(this->bss_size) << '\n';
	out << "  Sections Start: " << itoh(this->section_offset()) << '\n';
	out << "  Num Sections: " << this->num_sections() << '\n';
	out << "  Import Start: " << itoh(this->import_offset()) << '\n';
	out << "  Num Imports: " << this->num_imports() << '\n';
	out << "  Relocation Start: " << itoh(this->relocation_offset()) << '\n';
	out << "  Num Relocations: " << this->num_relocations() << '\n';
	out << "  Prolog Index: " << this->prolog_section << '\n';
	out << "  Prolog Offset: " << itoh(this->prolog_offset) << '\n';
	out << "  Epilog Index: " << this->epilog_section << '\n';
	out << "  Epilog Offset: " << itoh(this->epilog_offset) << '\n';
	out << "  Unresolved Index: " << this->unresolved_section << '\n';
	out << "  Unresolved Offset: " << itoh(this->unresolved_offset) << '\n';
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
	std::cout << padding << "Dumping REL Sections" << '\n';
	std::stringstream out;
	out << "Section Table:" << '\n';
	for (auto section = this->sections.begin(); section != sections.end(); section++) {
		out << "  Section " << section->id << ":" << '\n';
		out << "    Offset: " << itoh(section->offset) << '\n';
		out << "    Length: " << itoh(section->length) << '\n';
		if (section->offset > 0) {
			out << "    Range: " << itoh(section->offset) << " - " << itoh(section->offset + section->length) << '\n';
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
	for (auto imp = this->imports.begin(); imp != this->imports.end(); imp++) {
		std::cout << padding << "  Dumping Import " << imp->module << '\n';
		out << "  Import:" << '\n';
		out << "    Module: " << imp->module << '\n';
		out << "    Offset: " << itoh(imp->offset) << '\n';
		out << "    Relocation Table:" << '\n';
		for (auto reloc = imp->instructions.begin(); reloc != imp->instructions.end(); reloc++) {
			out << "      Relocation:" << '\n';
			out << "        Position: " << itoh(reloc->position) << '\n';
			out << "        Type: " << RelNames.at(reloc->type) << '\n';
			if (reloc->type == R_RVL_STOP) {
				continue;
			}
			if (reloc->type == R_RVL_SECT) {
				out << "        Destination Section: " << reloc->get_dest_section().id << '\n';
				continue;
			}
			out << "        Offset from Prev: " << itoh(reloc->prev_offset) << '\n';
			out << "        Source: " << reloc->get_src_section().id << " " << itoh(reloc->get_src_offset()) << '\n';
			out << "        Destination: " << reloc->get_dest_section().id << " " << itoh(reloc->get_dest_offset()) << '\n';
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