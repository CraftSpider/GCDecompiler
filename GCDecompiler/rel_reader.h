#pragma once

#include <vector>
#include <string>
#include "Section.h"
#include "Import.h"
#include "Relocation.h"
#include "types.h"

using std::string;

class REL {

public:
	uint id, name_offset, name_size, version, bss_size, prolog_section, epilog_section, unresolved_section,
		prolog_offset, epilog_offset, unresolved_offset, align, bss_align, fix_size, header_size;

	std::vector<Section> sections;
	std::vector<Import> imports;

	REL(string filename);
	std::vector<Section> used_sections();
	uint num_sections();
	uint num_imports();
	uint num_relocations();
	uint section_offset();
	uint import_offset();
	uint relocation_offset();

	void compile(string filename);

	string dump_header(int pad_len = 0);
	void dump_header(string filename);
	string dump_sections(int pad_len = 0);
	void dump_sections(string filename);
	string dump_imports(int pad_len = 0);
	void dump_imports(string filename);
	string dump_all();
	void dump_all(string filename);

};

class DOL {

public:
	uint entry_offset, bss_address, bss_size;

	std::vector<Section> sections;

	DOL(string filename);

	string dump_all();
	void dump_all(string filename);

};
