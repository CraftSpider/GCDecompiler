#pragma once

#include <string>
#include <unordered_map>
#include "section.h"
#include "import.h"
#include "enums.h"
#include "types.h"

class Import;

const std::unordered_map<int, std::string> RelNames({
	{R_PPC_NONE, "R_PPC_NONE"}, {R_PPC_ADDR32, "R_PPC_ADDR32"}, {R_PPC_ADDR24, "R_PPC_ADDR24"},
	{R_PPC_ADDR16, "R_PPC_ADDR16"}, {R_PPC_ADDR16_LO, "R_PPC_ADDR16_LO"}, {R_PPC_ADDR16_HI, "R_PPC_ADDR16_HI"},
	{R_PPC_ADDR16_HA, "R_PPC_ADDR16_HA"}, {R_PPC_ADDR14, "R_PPC_ADDR14"}, {R_PPC_REL24, "R_PPC_REL24"},
	{R_PPC_REL14, "R_PPC_REL14"}, {R_RVL_NONE, "R_RVL_NONE"}, {R_RVL_SECT, "R_RVL_SECT"}, {R_RVL_STOP, "R_RVL_STOP"}
});

class Relocation {
	uint dest_offset;
	Section *src_section, *dest_section;
	Import *import;

public:
	uint position, prev_offset, relative_offset;
	RelType type;

	Relocation(Import *import, uint position, uint relative_offset, uint prev_offset, uint dest_offset, RelType type, Section *src_section, Section *dest_section);
	uint get_src_offset();
	uint get_dest_offset();
	Section get_src_section();
	Section get_dest_section();
	Import get_import();

};
