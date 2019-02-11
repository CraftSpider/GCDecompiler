#pragma once

#include <string>
#include <set>
#include "types.h"
#include "ppc/register.h"

namespace PPC {

class Symbol {

public:

	uint start, end;
	std::string name;
	std::set<uchar> r_input, fr_input;

	Symbol(uint start, uint end, std::string name);
	
	void add_input(const Register& reg);

};

}