#pragma once

#define int unsigned int

class Section {
public:
	int id, offset, length;
	bool exec;
	char *data;

	Section(int id, int offset, bool exec, int length);
	int get_start();
	int get_end();
	int *get_range();
};

#undef int
