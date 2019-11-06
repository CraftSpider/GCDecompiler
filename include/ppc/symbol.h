#pragma once

#include <string>
#include <vector>
#include <set>
#include "types.h"
#include "ppc/instruction.h"
#include "ppc/register.h"

namespace PPC {

class Symbol {
    
    bool inputs_made;
    std::set<Register> r_input, fr_input;
    
    void gen_inputs();

public:

	ulong start, end;
	std::string name;
	std::vector<Instruction*> instructions;

	Symbol(ulong start, ulong end, const std::string& name);
	
	const std::set<Register>& get_input_regular();
	const std::set<Register>& get_input_float();

};

std::vector<Symbol> generate_symbols(const std::string& file_in, int start = 0, int end = -1);
void generate_inputs(std::vector<Symbol>& symbols);
std::vector<Symbol> load_symbols(const std::string& file_in);
void write_symbols(const std::vector<Symbol>& symbols, const std::string& file_out);

}