#pragma once

#include <vector>
#include <string>
#include "Section.h"
#include "Import.h"
#include "Relocation.h"

#define int unsigned int

using std::string;

class REL {

public:
	int id, section_offset, name_offset, name_size, version, bss_size, relocation_offset,
		import_offset, prolog_section, epilog_section, unresolved_section, prolog_offset,
		epilog_offset, unresolved_offset, align, bss_align, fix_size, header_size;

	std::vector<Section> sections;
	std::vector<Import> imports;

	REL(string filename);
	std::vector<Section> used_sections();
	void compile(string filename);
	int num_sections();
	int num_imports();
	int num_relocations();
	string dump_header();
	void dump_header(string filename);
	string dump_sections();
	void dump_sections(string filename);
	string dump_imports();
	void dump_imports(string filename);
	string dump_all();
	void dump_all(string filename);

};

#undef int
