#pragma once

#include "types.h"

class Section {

	char *data;

public:

	uint id, offset, length, address;
	bool exec;

	Section(uint id, uint offset, bool exec, uint length);
	Section(uint id, uint offset, bool exec, uint length, uint address);
	Section(const Section &sect);
	~Section();
	void set_data(char *data);
	void set_data_at(uint pos, char data);
	uint get_start();
	uint get_end();
	uint *get_range();
	char *get_data() const;

};
