#pragma once

#include <vector>
#include <string>
#include "section.h"
#include "import.h"
#include "relocation.h"
#include "types.h"

using std::string;

void process_rel(REL *rel, string output);
void process_rel(REL *rel, std::vector<REL> knowns, string output);
void process_dol(DOL *dol, string output);
