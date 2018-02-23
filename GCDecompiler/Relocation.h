#pragma once

#include <string>
#include <unordered_map>
#include "Section.h"
#include "Import.h"

#define int unsigned int

class Import;

enum RelType {
	R_PPC_NONE = 0, R_PPC_ADDR32 = 1, R_PPC_ADDR24 = 2, R_PPC_ADDR16 = 3, R_PPC_ADDR16_LO = 4,
	R_PPC_ADDR16_HI = 5, R_PPC_ADDR16_HA = 6, R_PPC_ADDR14 = 7, R_PPC_REL24 = 10, R_PPC_REL14 = 11,
	R_RVL_NONE = 201, R_RVL_SECT = 202, R_RVL_STOP = 203
};

const std::unordered_map<RelType, std::string> RelNames({
	{R_PPC_NONE, "R_PPC_NONE"}, {R_PPC_ADDR32, "R_PPC_ADDR32"}, {R_PPC_ADDR24, "R_PPC_ADDR24"},
	{R_PPC_ADDR16, "R_PPC_ADDR16"}, {R_PPC_ADDR16_LO, "R_PPC_ADDR16_LO"}, {R_PPC_ADDR16_HI, "R_PPC_ADDR16_HI"},
	{R_PPC_ADDR16_HA, "R_PPC_ADDR16_HA"}, {R_PPC_ADDR14, "R_PPC_ADDR14"}, {R_PPC_REL24, "R_PPC_REL24"},
	{R_PPC_REL14, "R_PPC_REL14"}, {R_RVL_NONE, "R_RVL_NONE"}, {R_RVL_SECT, "R_RVL_SECT"}, {R_RVL_STOP, "R_RVL_STOP"}
});

class Relocation {
	int dest_offset;
	Section *src_section, *dest_section;
	Import *import;

public:
	int position, prev_offset, relative_offset;
	RelType type;

	Relocation(Import *import, int position, int relative_offset, int prev_offset, int dest_offset, RelType type, Section *src_section, Section *dest_section);
	int get_src_offset();
	int get_dest_offset();
	Section get_src_section();
	Section get_dest_section();
	Import get_import();

};

#undef int
