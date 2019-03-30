#pragma once

#include <string>
#include <vector>
#include <set>
#include "filetypes/rel.h"
#include "ppc/register.h"
#include "ppc/instruction.h"

namespace PPC {

void relocate(types::REL *input, const uint& bss_pos, const std::string& file_out);
void relocate(types::REL *input, const std::string& file_out);

void read_data(const std::string& file_in, const std::string& file_out, int start = 0, int end = -1);
void read_data(types::REL *to_read, const Section *section, const std::vector<types::REL*>& knowns, const std::string& file_out);

}