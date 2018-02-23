#pragma once

#include <vector>
#include "Section.h"
#include "Relocation.h"

#define int unsigned int

class Relocation;
enum RelType;

class Import {

	Section *section;

public:
	int module, offset, pointer;
	std::vector<Relocation> instructions;

	Import(int module, int offset);
	void add_relocation(RelType type, int position, int prev_offset, int relative_offset, Section *src_section);
	Section get_section();

};

#undef int
