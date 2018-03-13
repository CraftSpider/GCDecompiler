#pragma once

#include <string>
#include <vector>
#include "rel.h"

namespace PPC {

	class Instruction {

		string name;
		char *instruction;

	public:

		Instruction();

		Instruction(int type, char *instruction);

		string code_name();
		string get_variables();

	};

	void disassemble(string file_in, string file_out, int start = 0, int end = -1);
	
	void read_data(string file_in, string file_out, int start = 0, int end = -1);
	void read_data(REL *to_read, Section *section, std::vector<REL*> knowns, string file_out);

}