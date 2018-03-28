
#include <set>
#include "symbol.h"

namespace PPC {

	Symbol::Symbol(int start, int end, string name) {
		this->start = start;
		this->end = end;
		this->name = name;
		this->r_input = std::set<char>();
	}

	void Symbol::add_source(Register reg) {
		if (reg.type == Register::REGULAR) {
			r_input.insert(reg.number);
		} else if (reg.type == Register::FLOAT) {
			fr_input.insert(reg.number);
		}
	}

}