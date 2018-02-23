
#include "Relocation.h"

#define int unsigned int

Relocation::Relocation(Import *import, int position, int relative_offset, int prev_offset, int dest_offset, RelType type, Section *src_section, Section *dest_section) {
	this->import = import;
	this->position = position;
	this->relative_offset = relative_offset;
	this->prev_offset = prev_offset;
	this->dest_offset = dest_offset;
	this->type = type;
	this->src_section = src_section;
	this->dest_section = dest_section;
}

int Relocation::get_dest_offset() {
	return this->dest_offset;
}

int Relocation::get_src_offset() {
	if (this->import->module == 0) {
		return this->relative_offset;
	}
	else if (this->get_src_section().offset == 0) {
		return -1;
	}
	else {
		return this->relative_offset + this->get_src_section().offset;
	}
}

Section Relocation::get_src_section() {
	return *(this->src_section);
}

Section Relocation::get_dest_section() {
	return *(this->dest_section);
}

Import Relocation::get_import() {
	return *(this->import);
}
