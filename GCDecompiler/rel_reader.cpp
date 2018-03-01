
#include <limits.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <experimental/filesystem>
#include "types.h"
#include "utils.h"
#include "rel.h"
#include "dol.h"
#include "rel_reader.h"
#include "ppc_reader.h"

namespace fs = std::experimental::filesystem;
using std::string;
using std::vector;
using std::endl;

void process_rel(string filename, string output) {
	REL rel(filename);
	fs::create_directory(fs::path(output));
	rel.dump_header(output + "/header.txt");
	rel.dump_sections(output + "/sections.txt");
	rel.dump_imports(output + "/imports.txt");
	for (vector<Section>::iterator sect = rel.sections.begin(); sect != rel.sections.end(); sect++) {
		if (sect->exec && sect->offset) {
			std::stringstream name;
			name << output << "/Section" << sect->id << ".ppc";
			PPC::decompile(filename, name.str(), sect->offset, sect->offset + sect->length);
		}
	}
}

void process_dol(string filename, string output) {
	DOL dol(filename);
	fs::create_directory(fs::path(output));
	dol.dump_all(output + "/dol.txt");
	for (vector<Section>::iterator sect = dol.sections.begin(); sect != dol.sections.end(); sect++) {
		if (sect->exec && sect->offset) {
			std::stringstream name;
			name << output << "/Section" << sect->id << ".ppc";
			PPC::decompile(filename, name.str(), sect->offset, sect->offset + sect->length);
		}
	}
}

int main(int argc, char *argv[]) {

	if (argc == 1) {
		std::cout << "Usage:" << endl;
		std::cout << "gcd dump <path to root> [directory out]" << endl;
		std::cout << "gcd rel <file in> [directory out]" << endl;
		std::cout << "gcd dol <file in> [directory out]" << endl;
	} else if (argc == 2) {
		std::cout << "Missing file input parameter" << endl;
	} else {
		string output = "out.txt";
		if (argc > 3) {
			output = argv[3];
		} else {
			if (!std::strcmp(argv[1], "dump")) {
				output = "root_dump";
			} else if (!std::strcmp(argv[1], "recompile")) {
				output = "recomp.rel";
			} else if (!std::strcmp(argv[1], "dol")) {
				output = "dol_dump";
			} else if (!std::strcmp(argv[1], "rel")) {
				output = "rel_dump";
			}
		}
		
		if (!std::strcmp(argv[1], "dump")) {
			std::cout << "Beginnning Root Dump. This may take a while." << endl;
			fs::create_directory(output);
			for (auto dir : fs::recursive_directory_iterator(argv[2])) {
				if (ends_with(dir.path().string(), ".rel")) {
					string filename = dir.path().filename().string();
					process_rel(dir.path().string(), output + "/" + filename.substr(0, filename.length() - 4));
				} else if (ends_with(dir.path().string(), ".dol")) {
					string filename = dir.path().filename().string();
					process_dol(dir.path().string(), output + "/" + filename.substr(0, filename.length() - 4));
				}
			}
			std::cout << "Root Dump complete." << endl;
		} else if (!std::strcmp(argv[1], "recompile")) {
			REL rel(argv[2]);
			rel.compile(output);
		} else if (!std::strcmp(argv[1], "rel")) {
			process_rel(argv[2], output);
		} else if (!std::strcmp(argv[1], "dol")) {
			process_dol(argv[2], output);
		} else {
			std::cout << "Unrecognized Operation" << endl;
		}
	}

	return 0;
}
