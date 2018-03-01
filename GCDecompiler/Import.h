#pragma once

#include <vector>
#include "Section.h"
#include "relocation.h"
#include "enums.h"
#include "types.h"

class Relocation;

class Import {

	Section *section;

public:
	uint module, offset, pointer;
	std::vector<Relocation> instructions;

	Import(uint module, uint offset);
	void add_relocation(RelType type, uint position, uint prev_offset, uint relative_offset, Section *src_section);
	Section get_section();

};
