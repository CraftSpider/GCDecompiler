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
	int id, name_offset, name_size, version, bss_size, prolog_section, epilog_section, unresolved_section,
		prolog_offset, epilog_offset, unresolved_offset, align, bss_align, fix_size, header_size;

	std::vector<Section> sections;
	std::vector<Import> imports;

	REL(string filename);
	std::vector<Section> used_sections();
	int num_sections();
	int num_imports();
	int num_relocations();
	int section_offset();
	int import_offset();
	int relocation_offset();

	void compile(string filename);

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
