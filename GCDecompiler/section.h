#pragma once

#include "types.h"

class Section {
public:
	uint id, offset, length, address;
	bool exec;
	char *data;

	Section(uint id, uint offset, bool exec, uint length);
	Section(uint id, uint offset, bool exec, uint length, uint address);
	uint get_start();
	uint get_end();
	uint *get_range();
};
