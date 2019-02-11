
#include <set>

#include "ppc/symbol.h"

namespace PPC {

Symbol::Symbol(uint start, uint end, std::string name) {
	this->start = start;
	this->end = end;
	this->name = name;
	this->r_input = std::set<uchar>();
}

void Symbol::add_input(const Register& reg) {
	if (reg.type == Register::REGULAR) {
		r_input.insert(reg.number);
	} else if (reg.type == Register::FLOAT) {
		fr_input.insert(reg.number);
	}
}

}