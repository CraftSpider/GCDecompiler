#pragma once

#include <string>
#include <vector>
#include <set>
#include "filetypes/rel.h"
#include "ppc/register.h"
#include "ppc/instruction.h"

namespace PPC {

	Instruction* create_instruction(const char *instruction);

	void relocate(types::REL *input, const uint& bss_pos, const string& file_out);
	void relocate(types::REL *input, string file_out);

	void disassemble(const string& file_in, const string& file_out, int start = 0, int end = -1);
	
	void read_data(const string& file_in, const string& file_out, int start = 0, int end = -1);
	void read_data(types::REL *to_read, Section *section, const std::vector<types::REL*>& knowns, const string& file_out);

	void decompile(const string& file_in, const string& file_out, int start, int end);

}