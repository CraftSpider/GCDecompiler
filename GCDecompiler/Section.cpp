#include "Section.h"

#include "types.h"

Section::Section(uint id, uint offset, bool exec, uint length) {
	this->id = id;
	this->offset = offset;
	this->exec = exec;
	this->length = length;
	this->data = new char[length]();
}

uint Section::get_start() {
	return this->offset;
}

uint Section::get_end() {
	return this->offset + this->length;
}

uint* Section::get_range() {
	return new uint[2] { this->get_start(), this->get_end() };
}
