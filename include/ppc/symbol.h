#pragma once

#include <string>
#include <vector>
#include <set>
#include "types.h"
#include "ppc/register.h"

namespace PPC {

class Symbol {

public:

	ulong start, end;
	std::string name;
	std::set<uchar> r_input, fr_input;

	Symbol(ulong start, ulong end, const std::string& name);
	
	void add_input(const Register& reg);

};

std::vector<Symbol> generate_symbols(const std::string& file_in, int start = 0, int end = -1);
void generate_inputs(const std::string& file_in, std::vector<Symbol>& symbols);
std::vector<Symbol> load_symbols(const std::string& file_in);
void write_symbols(const std::vector<Symbol>& symbols, const std::string& file_out);

}