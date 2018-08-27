#pragma once

#include <vector>
#include <string>
#include "filetypes/rel.h"
#include "filetypes/dol.h"

void process_rel(types::REL *rel, const std::string& output);
void process_rel(types::REL *rel, const std::vector<types::REL*>& knowns, const std::string& output);
void process_dol(types::DOL *dol, const std::string& output);
