#pragma once

#include <string>
#include <vector>
#include <set>
#include "rel.h"
#include "ppc/register.h"
#include "ppc/instruction.h"

namespace PPC {

	static Instruction* create_instruction(char *instruction);

	void disassemble(string file_in, string file_out, int start = 0, int end = -1);
	
	void read_data(string file_in, string file_out, int start = 0, int end = -1);
	void read_data(REL *to_read, Section *section, std::vector<REL*> knowns, string file_out);

	void decompile(string file_in, string file_out, int start, int end);

}