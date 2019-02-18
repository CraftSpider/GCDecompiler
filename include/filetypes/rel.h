
#pragma once

#include <vector>
#include <string>
#include "types.h"
#include "section.h"
#include "import.h"

namespace types {

class REL {

public:
	uint id, name_offset, name_size, version, bss_size, prolog_section, epilog_section, unresolved_section,
		prolog_offset, epilog_offset, unresolved_offset, align, bss_align, fix_size, header_size, file_size;
	std::string filename;

	std::vector<Section> sections;
	std::vector<Import> imports;

	REL(const std::string& filename);
	uint num_sections();
	uint num_imports();
	uint num_relocations();
	uint section_offset();
	uint import_offset();
	uint relocation_offset();

	void compile(const std::string& file_out);

	std::string dump_header(uint pad_len = 0);
	void dump_header(const std::string& filename);
	std::string dump_sections(uint pad_len = 0);
	void dump_sections(const std::string& filename);
	std::string dump_imports(uint pad_len = 0);
	void dump_imports(const std::string& filename);
	std::string dump_all();
	void dump_all(const std::string& filename);

};

}