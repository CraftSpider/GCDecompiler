
#include <iostream>
#include <iterator>
#include "section.h"
#include "types.h"

Section::Section(uint id, uint offset, bool exec, uint length) {
	this->id = id;
	this->offset = offset;
	this->exec = exec;
	this->length = length;
	this->data = new char[length]();
}

Section::Section(uint id, uint offset, bool exec, uint length, uint address) : Section(id, offset, exec, length) {
	this->address = address;
}

Section::Section(const Section &sect) {
	this->id = sect.id;
	this->offset = sect.offset;
	this->exec = sect.exec;
	this->length = sect.length;
	this->data = new char[this->length];
	for (uint i = 0; i < this->length; i++) {
		this->data[i] = sect.data[i];
	}
	this->address = sect.address;
}

Section::~Section() {
	delete[] this->data;
}

void Section::set_data(char *data) {
	delete[] this->data;
	this->data = data;
}

void Section::set_data_at(uint pos, char data) {
	this->data[pos] = data;
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

char* Section::get_data() {
	return this->data;
}
