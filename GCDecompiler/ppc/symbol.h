#pragma once

#include <string>
#include <set>
#include "../types.h"
#include "register.h"

using std::string;

namespace PPC {

	class Symbol {

	public:

		uint start, end;
		string name;
		std::set<uchar> r_input, fr_input;

		Symbol(uint start, uint end, string name);

		void add_source(const Register& reg);

	};

}