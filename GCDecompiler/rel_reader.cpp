
#include <limits.h>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cstring>
#include "utils.h"
#include "rel_reader.h"
#include "ppc_reader.h"
#include "types.h"

using std::string;
using std::ios;
using std::vector;
using std::endl;


DOL::DOL(string filename) {
	std::fstream file_r(filename, ios::binary | ios::in);
	std::fstream *file = &file_r;

	// Read in file Header
	uint offset, address, size;

	for (int i = 0; i < 7; i++) {
		file->seekg(0x0 + i*4, ios::beg);
		offset = next_int(file);
		file->seekg(0x48 + i*4, ios::beg);
		address = next_int(file);
		file->seekg(0x90 + i*4, ios::beg);
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

REL::REL(string filename) {
	std::fstream file_r(filename, ios::binary | ios::in);
	std::fstream *file = &file_r;

	// Read in file Header
	this->id = next_int(file);
	file->seekg(8, ios::cur); // Skip over the next and previous module values
	uint num_sections = next_int(file);
	uint section_offset = next_int(file);
	this->name_offset = next_int(file);
	this->name_size = next_int(file);
	this->version = next_int(file);
	this->bss_size = next_int(file);
	file->seekg(4, ios::cur); // Because we don't need to know the relocation offset
	uint import_offset = next_int(file);
	uint num_imports = next_int(file) / 8; // Convert length of imports to number of imports
	this->prolog_section = next_int(file, 1);
	this->epilog_section = next_int(file, 1);
	this->unresolved_section = next_int(file, 1);
	file->seekg(1, ios::cur); // Skip padding
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
	this->header_size = (int)file->tellg();

	// Read in Section table
	file->seekg(section_offset, ios::beg);
	for (uint i = 0; i < num_sections; i++) {
		uint offset = next_int(file);
		bool exec = offset & 1;
		offset = offset >> 1 << 1;
		uint length = next_int(file);
		this->sections.push_back(Section(i, offset, exec, length));
	}

	// Read in Section data
	for (vector<Section>::iterator section = this->sections.begin(); section != this->sections.end(); section++) {
		if (section->offset != 0) {
			file->seekg(section->offset, ios::beg);
			char *data = new char[section->length];
			file->read(data, section->length);
			section->data = data;
		}
	}

	// Read in Import table
	file->seekg(import_offset, ios::beg);
	for (uint i = 0; i < num_imports; i++) {
		uint module_id = next_int(file);
		uint offset = next_int(file);
		this->imports.push_back(Import(module_id, offset));
	}

	// Read Relocation table into Import
	for (vector<Import>::iterator imp = this->imports.begin(); imp != this->imports.end(); imp++) {
		file->seekg(imp->offset, ios::beg);
		RelType rel_type = RelType(0);
		while (rel_type != R_RVL_STOP) {
			uint position = (uint)file->tellg();
			uint prev_offset = next_int(file, 2);
			rel_type = RelType(next_int(file, 1));
			Section *section = &this->sections.at(next_int(file, 1));
			uint rel_offset = next_int(file);
			imp->add_relocation(rel_type, position, rel_offset, prev_offset, section);
		}
	}
}

uint REL::num_sections() {
	return (uint)this->sections.size();
}

uint REL::num_imports() {
	return (uint)this->imports.size();
}

uint REL::num_relocations() {
	uint out = 0;
	for (vector<Import>::iterator imp = this->imports.begin(); imp != imports.end(); imp++) {
		out += (uint)imp->instructions.size();
	}
	return out;
}

uint REL::section_offset() {
	return header_size;
}

uint REL::import_offset() {
	int out = this->relocation_offset();
	for (vector<Import>::iterator imp = this->imports.begin(); imp != imports.end(); imp++) {
		out += (uint)imp->instructions.size() * 8;
	}
	return out;
}

uint REL::relocation_offset() {
	int out = 0;
	out += this->header_size;
	out += this->num_sections() * 8;
	for (vector<Section>::iterator section = this->sections.begin(); section != this->sections.end(); section++) {
		if (section->offset != 0) {
			out += section->length;
		}
	}
	out += 16;
	return out;
}

void REL::compile(string filename) {
	std::cout << "Compiling REL file " << this->id << endl;
	std::fstream out_r(filename, ios::out | ios::binary);
	std::fstream *out = &out_r;
	// Recalculate any necessary numbers for offsets

	// Write Header to the file
	std::cout << "  Writing header" << endl;
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
	std::cout << "  Writing section table" << endl;
	out->seekp(this->section_offset(), ios::beg);
	for (uint i = 0; i < this->num_sections(); i++) {
		Section section = this->sections.at(i);
		write_int(out, section.offset | (int)section.exec); // Add exec bit back in
		write_int(out, section.length);
	}

	// Write actual sections to the file
	std::cout << "  Writing section data" << endl;
	for (vector<Section>::iterator section = this->sections.begin(); section != this->sections.end(); section++) {
		if (section->offset != 0) {
			out->seekp(section->offset, ios::beg);
			out->write(section->data, section->length);
		}
	}

	// Write the Import Table
	std::cout << "  Writing import table" << endl;
	out->seekp(this->import_offset(), ios::beg);
	for (uint i = 0; i < this->num_imports(); i++) {
		Import imp = this->imports.at(i);
		write_int(out, imp.module);
		write_int(out, imp.offset);
	}

	// Write the Relocation Instructions
	std::cout << "  Writing relocation instructions" << endl;
	for (vector<Import>::iterator imp = this->imports.begin(); imp != this->imports.end(); imp++) {
		out->seekp(imp->offset, ios::beg);
		for (vector<Relocation>::iterator reloc = imp->instructions.begin(); reloc != imp->instructions.end(); reloc++) {
			write_int(out, reloc->prev_offset, 2);
			write_int(out, reloc->type, 1);
			write_int(out, reloc->get_src_section().id, 1);
			write_int(out, reloc->relative_offset);
		}
	}
	std::cout << "REL compile complete" << endl;
}

string REL::dump_header(int pad_len) {
	string padding(pad_len, ' ');
	std::cout << padding << "Dumping Header" << endl;
	std::stringstream out;
	out << "REL Header:" << endl;
	out << "  ID: " << this->id << endl;
	out << "  Version: " << this->version << endl;
	out << "  Name Offset: " << itoh(this->name_offset) << endl;
	out << "  Name Size: " << itoh(this->name_size) << endl;
	out << "  .bss Size: " << itoh(this->bss_size) << endl;
	out << "  Sections Start: " << itoh(this->section_offset()) << endl;
	out << "  Num Sections: " << this->num_sections() << endl;
	out << "  Import Start: " << itoh(this->import_offset()) << endl;
	out << "  Num Imports: " << this->num_imports() << endl;
	out << "  Relocation Start: " << itoh(this->relocation_offset()) << endl;
	out << "  Num Relocations: " << this->num_relocations() << endl;
	out << "  Prolog Index: " << this->prolog_section << endl;
	out << "  Prolog Offset: " << itoh(this->prolog_offset) << endl;
	out << "  Epilog Index: " << this->epilog_section << endl;
	out << "  Epilog Offset: " << itoh(this->epilog_offset) << endl;
	out << "  Unresolved Index: " << this->unresolved_section << endl;
	out << "  Unresolved Offset: " << itoh(this->unresolved_offset) << endl;
	if (this->version >= 2) {
		out << "  Align: " << this->align << endl;
		out << "  .bss Align: " << this->bss_align << endl;
	}
	if (this->version >= 3) {
		out << "  Fix Size: " << this->fix_size << endl;
	}
	return out.str();
}

void REL::dump_header(string filename) {
	std::fstream out(filename, ios::out);
	out << this->dump_header();
}

string REL::dump_sections(int pad_len) {
	string padding(pad_len, ' ');
	std::cout << padding << "Dumping Sections" << endl;
	std::stringstream out;
	out << "Section Table:" << endl;
	for (vector<Section>::iterator section = this->sections.begin(); section != sections.end(); section++) {
		out << "  Section " << section->id << ":" << endl;
		out << "    Offset: " << itoh(section->offset) << endl;
		out << "    Length: " << itoh(section->length) << endl;
		if (section->offset > 0) {
			out << "    Range: " << itoh(section->offset) << " - " << itoh(section->offset + section->length) << endl;
		}
		out << "    Executable: " << section->exec << endl;
	}
	return out.str();
}

void REL::dump_sections(string filename) {
	std::fstream out(filename, ios::out);
	out << this->dump_sections();
}

string REL::dump_imports(int pad_len) {
	string padding(pad_len, ' ');
	std::cout << padding << "Dumping Imports" << endl;
	std::stringstream out;
	out << "Import Table:" << endl;
	for (vector<Import>::iterator imp = this->imports.begin(); imp != this->imports.end(); imp++) {
		std::cout << padding << "  Dumping Import " << imp->module << endl;
		out << "  Import:" << endl;
		out << "    Module: " << imp->module << endl;
		out << "    Offset: " << itoh(imp->offset) << endl;
		out << "    Relocation Table:" << endl;
		for (vector<Relocation>::iterator reloc = imp->instructions.begin(); reloc != imp->instructions.end(); reloc++) {
			out << "      Relocation:" << endl;
			out << "        Position: " << itoh(reloc->position) << endl;
			out << "        Type: " << RelNames.at(reloc->type) << endl;
			if (reloc->type == R_RVL_STOP) {
				continue;
			}
			if (reloc->type == R_RVL_SECT) {
				out << "        Destination Section: " << reloc->get_dest_section().id << endl;
				continue;
			}
			out << "        Offset from Prev: " << itoh(reloc->prev_offset) << endl;
			out << "        Source: " << reloc->get_src_section().id << " " << itoh(reloc->get_src_offset()) << endl;
			out << "        Destination: " << reloc->get_dest_section().id << " " << itoh(reloc->get_dest_offset())<< endl;
		}
	}
	return out.str();
}

void REL::dump_imports(string filename) {
	std::fstream out(filename, ios::out);
	out << this->dump_imports();
}

string REL::dump_all() {
	std::stringstream out;
	out << this->dump_header() << endl;
	out << this->dump_sections() << endl;
	out << this->dump_imports() << endl;
	return out.str();
}

void REL::dump_all(string filename) {
	std::cout << "Dumping REL file" << endl;
	std::fstream out(filename, ios::out);
	out << this->dump_header(2) << endl;
	out << this->dump_sections(2) << endl;
	out << this->dump_imports(2) << endl;
	std::cout << "REL dump complete" << endl;
}

int main(int argc, char *argv[]) {

	if (argc == 1) {
		std::cout << "Usage:" << endl;
		std::cout << "gcd dump <file in> [directory out]" << endl;
		std::cout << "gcd recompile <file in> [file out]" << endl;
		std::cout << "gcd dol <file in> [directory out]" << endl;
	} else if (argc == 2) {
		std::cout << "Missing file input parameter";
	} else {
		string output = "out.txt";
		if (argc > 3) {
			output = argv[3];
		} else {
			if (!std::strcmp(argv[1], "dump")) {
				output = "rel_dump";
			} else if (!std::strcmp(argv[1], "recompile")) {
				output = "recomp.rel";
			} else if (!std::strcmp(argv[1], "dol")) {
				output = "dol_dump";
			}
		}
		
		if (!std::strcmp(argv[1], "dump")) {
			REL rel(argv[2]);
			create_directory(output);
			rel.dump_header(output + "/header.txt");
			rel.dump_sections(output + "/sections.txt");
			rel.dump_imports(output + "/imports.txt");
			for (vector<Section>::iterator sect = rel.sections.begin(); sect != rel.sections.end(); sect++) {
				if (sect->exec && sect->offset) {
					std::stringstream name;
					name << output << "/Section" << sect->id << ".ppc";
					PPC::decompile(argv[2], name.str(), sect->offset, sect->offset + sect->length);
				}
			}
		} else if (!std::strcmp(argv[1], "recompile")) {
			REL rel(argv[2]);
			rel.compile(output);
		} else if (!std::strcmp(argv[1], "dol")) {
			DOL dol(argv[2]);
			create_directory(output);
			dol.dump_all(output + "/dol.txt");
			for (vector<Section>::iterator sect = dol.sections.begin(); sect != dol.sections.end(); sect++) {
				if (sect->exec && sect->offset) {
					std::stringstream name;
					name << output << "/Section" << sect->id << ".ppc";
					PPC::decompile(argv[2], name.str(), sect->offset, sect->offset + sect->length);
				}
			}
		} else {
			std::cout << "Unrecognized Operation" << endl;
		}
	}

	return 0;
}
