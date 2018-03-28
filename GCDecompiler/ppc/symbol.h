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
		std::set<char> r_input, fr_input;

		Symbol(int start, int end, string name);

		void add_source(Register reg);

	};

}