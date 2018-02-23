
#include "Import.h"

#define int unsigned int

Import::Import(int module, int offset) {
	this->module = module;
	this->offset = offset;
	this->section = nullptr;
}

void Import::add_relocation(RelType type, int position, int relative_offset, int prev_offset, Section *src_section) {
	if (type == R_RVL_SECT) {
		this->pointer = src_section->offset;
		this->section = src_section;
	}
	this->pointer += prev_offset;

	Relocation reloc = Relocation(this, position, relative_offset, prev_offset, this->pointer, type, src_section, this->section);
	this->instructions.push_back(reloc);
}

Section Import::get_section() {
	return *(this->section);
}