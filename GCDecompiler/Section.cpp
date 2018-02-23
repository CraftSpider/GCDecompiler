#include "Section.h"

#define int unsigned int

Section::Section(int id, int offset, bool exec, int length) {
	this->id = id;
	this->offset = offset;
	this->exec = exec;
	this->length = length;
	this->data = new char[length]();
}

int Section::get_start() {
	return this->offset;
}

int Section::get_end() {
	return this->offset + this->length;
}

int* Section::get_range() {
	int temp[2] = { this->get_start(), this->get_end() };
	return temp;
}
