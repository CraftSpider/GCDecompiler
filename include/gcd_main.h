#pragma once

#include <vector>
#include <string>
#include <argparser.h>

#include "filetypes/rel.h"
#include "filetypes/dol.h"

void process_rel(types::REL *rel, const std::vector<types::REL*>& knowns, const std::string& output, bool info);
void process_dol(types::DOL *dol, const std::string& output, bool info);

int command_decomp(const std::string& input, const std::string& output);
int command_dump(const std::string& input, const std::string& output, ArgParser& parser);
int command_recomp(const std::string& input, const std::string& output);

int command_rel(const std::string& input, const std::string& output, ArgParser& parser);
int command_dol(const std::string& input, const std::string& output, ArgParser& parser);

int command_tpl(const std::string& input, const std::string& output, ArgParser& parser);
