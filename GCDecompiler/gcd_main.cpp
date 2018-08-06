
#include <limits.h>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <experimental/filesystem>
#include "types.h"
#include "utils.h"
#include "gcd_main.h"
#include "ppc_reader.h"
#include "logging.h"
#include "filetypes/lz.h"
#include "filetypes/tpl.h"

namespace fs = std::experimental::filesystem;
using std::string;
using std::vector;
using std::endl;

void process_rel(types::REL *rel, const string& output) {
	fs::create_directory(fs::path(output));
	rel->dump_header(output + "/header.txt");
	rel->dump_sections(output + "/sections.txt");
	rel->dump_imports(output + "/imports.txt");
	for (auto sect = rel->sections.begin(); sect != rel->sections.end(); ++sect) {
		if (sect->exec && sect->offset) {
			std::stringstream name;
			name << output << "/Section" << sect->id << ".ppc";
			PPC::disassemble(rel->filename, name.str(), sect->offset, sect->offset + sect->length);
		}
	}
}

void process_rel(types::REL *rel, const vector<types::REL*>& knowns, const string& output) {
	process_rel(rel, output);
	for (auto sect = rel->sections.begin(); sect != rel->sections.end(); ++sect) {
		if (!sect->exec && sect->offset != 0 && sect->length > 4 && rel->id == 1) {
			std::stringstream name;
			name << output << "/Section" << sect->id << ".ppd";
			PPC::read_data(rel, &*sect, knowns, name.str());
		}
	}
}

void process_dol(types::DOL *dol, const string& output) {
	fs::create_directory(fs::path(output));
	dol->dump_all(output + "/dol.txt");
	for (auto sect = dol->sections.begin(); sect != dol->sections.end(); ++sect) {
		if (sect->exec && sect->offset) {
			std::stringstream name;
			name << output << "/Section" << sect->id << ".ppc";
			PPC::disassemble(dol->filename, name.str(), sect->offset, sect->offset + sect->length);
		}
	}
}

void decomp_game(types::DOL *dol, const std::vector<types::REL*>& rels, const string& output) {
	for (auto sect = dol->sections.begin(); sect != dol->sections.end(); ++sect) {
		if (sect->exec && sect->offset) {
			std::stringstream name;
			name << output << "/Section" << sect->id << ".c";
			PPC::decompile(dol->filename, name.str(), sect->offset, sect->offset + sect->length);
		}
	}
}

int main(int argc, char *argv[]) {
    
    logging::set_default_level(logging::TRACE);
    logging::Logger *log = logging::get_logger("main");

	/*string output = "decomp_dump";
	fs::create_directory(output);
	std::vector<REL*> knowns;
	DOL *main = nullptr;
	// Form list of files to process. Mostly RELs and DOL file.
	for (auto dir : fs::recursive_directory_iterator("./root/")) {
		if (ends_with(dir.path().string(), ".rel")) {
			string filename = dir.path().filename().string();
			REL *rel = new REL(dir.path().string());
			knowns.push_back(rel);
		} else if (ends_with(dir.path().string(), ".dol")) {
			string filename = dir.path().filename().string();
			main = new DOL(dir.path().string());
		}
	}
	// Process list of files.
	decomp_game(main, knowns, output);

	return 0;//*/

	//string temp = "C:/ProgrammingFiles/GCDecompiler/GCDecompiler/root/mkb2.main_loop.rel";
	//REL rel(temp);
	//process_rel(&rel, "C:/ProgrammingFiles/GCDecompiler/Debug/root_dump");

	/*LZ lz = LZ("C:/ProgrammingFiles/GCDecompiler/Debug/STAGE001.lz");
	lz.write_decompressed("C:/ProgrammingFiles/GCDecompiler/Debug/STAGE001.dz");
	return 0;*/

	if (argc == 1) {
		std::cout << "Usage:" << endl;
		std::cout << "gcd decomp <path to root> [directory out]" << endl;
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
			if (!std::strcmp(argv[1], "decomp")) {
				output = "root_decomp";
			} else if (!std::strcmp(argv[1], "dump")) {
				output = "root_dump";
			} else if (!std::strcmp(argv[1], "recompile")) {
				output = "recomp.rel";
			} else if (!std::strcmp(argv[1], "dol")) {
				output = "dol_dump";
			} else if (!std::strcmp(argv[1], "rel")) {
				output = "rel_dump";
			}
		}
		
		if (!std::strcmp(argv[1], "decomp")) {
		    log->info("Beginning root decompile. This will take a while.");
//			std::cout << "Beginning root decompile. This will take a while." << endl;
			fs::create_directory(output);
			std::vector<types::REL*> knowns;
			types::DOL *main = nullptr;
			// Form list of files to process. Mostly RELs and DOL file.
			for (auto dir : fs::recursive_directory_iterator(argv[2])) {
				if (ends_with(dir.path().string(), ".rel")) {
					string filename = dir.path().filename().string();
					types::REL *rel = new types::REL(dir.path().string());
					knowns.push_back(rel);
				} else if (ends_with(dir.path().string(), ".dol")) {
					string filename = dir.path().filename().string();
					main = new types::DOL(dir.path().string());
				}
			}
			// Process list of files.
			decomp_game(main, knowns, output);
			std::cout << "Root decompile complete." << endl;
		} else if (!std::strcmp(argv[1], "dump")) {
			std::cout << "Beginnning Root Dump. This may take a while." << endl;
			fs::create_directory(output);
			std::vector<types::REL*> knowns;
			types::DOL *main = nullptr;
			// Form list of files to process. Mostly RELs and DOL file.
			for (auto dir : fs::recursive_directory_iterator(argv[2])) {
				if (ends_with(dir.path().string(), ".rel")) {
					string filename = dir.path().filename().string();
					types::REL *rel = new types::REL(dir.path().string());
					knowns.push_back(rel); 
				} else if (ends_with(dir.path().string(), ".dol")) {
					string filename = dir.path().filename().string();
					main = new types::DOL(dir.path().string());
				}
			}
            
            // Process list of files. Disassemble, Form data lists, dump info.
			if (main == nullptr) {
			    std::cout << "No DOL file found" << endl;
			} else {
                fs::path path(main->filename.c_str());
                process_dol(main,
                            output + "/" + path.filename().string().substr(0, path.filename().string().length() - 4));
            }
			for (auto rel : knowns) {
				fs::path path(rel->filename.c_str());
				string filename = path.filename().string();
				process_rel(rel, knowns, output + "/" + filename.substr(0, filename.length() - 4));
			}
			
			// Clean up memory
			for (auto rel : knowns) {
				delete rel;
			}
			delete main;
			std::cout << "Root Dump complete." << endl;
		} else if (!std::strcmp(argv[1], "recompile")) {
			types::REL rel(argv[2]);
			rel.compile(output);
		} else if (!std::strcmp(argv[1], "rel")) {
			types::REL rel(argv[2]);
			process_rel(&rel, output);
		} else if (!std::strcmp(argv[1], "dol")) {
            types::DOL dol(argv[2]);
            process_dol(&dol, output);
        } else if (!std::strcmp(argv[1], "tpl")) {
		    if (!std::strcmp(argv[2], "e")) {
		        log->info("Extracting TPL " + std::string(argv[3]));
		        types::TPL *tpl = new types::GCTPL(argv[3]);
		        fs::create_directory(fs::path("out"));
		        for (uint i = 0; i < tpl->get_num_images(); ++i) {
		            std::stringstream outname = std::stringstream();
		            outname << "out/" << i << ".png";
		            tpl->to_png(i, outname.str());
		        }
		        log->info("Completed TPL extraction");
		    } else if (!std::strcmp(argv[2], "i")) {
		    
		    }
		} else {
			std::cout << "Unrecognized Operation" << endl;
		}
	}

	return 0;
}
