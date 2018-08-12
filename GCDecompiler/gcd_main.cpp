
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
#include "argparser.h"
#include "png.h"

namespace fs = std::experimental::filesystem;

void process_rel(types::REL *rel, const std::string& output) {
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

void process_rel(types::REL *rel, const std::vector<types::REL*>& knowns, const std::string& output) {
	process_rel(rel, output);
	for (auto sect = rel->sections.begin(); sect != rel->sections.end(); ++sect) {
		if (!sect->exec && sect->offset != 0 && sect->length > 4 && rel->id == 1) {
			std::stringstream name;
			name << output << "/Section" << sect->id << ".ppd";
			PPC::read_data(rel, &*sect, knowns, name.str());
		}
	}
}

void process_dol(types::DOL *dol, const std::string& output) {
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

void decomp_game(types::DOL *dol, const std::vector<types::REL*>& rels, const std::string& output) {
	for (auto sect = dol->sections.begin(); sect != dol->sections.end(); ++sect) {
		if (sect->exec && sect->offset) {
			std::stringstream name;
			name << output << "/Section" << sect->id << ".c";
			PPC::decompile(dol->filename, name.str(), sect->offset, sect->offset + sect->length);
		}
	}
}

static std::map<std::string, std::string> default_outs = {
        {"decomp", "root_decomp"}, {"dump", "root_dump"}, {"recompile", "recompile.rel"}, {"dol", "dol_dump"},
        {"rel", "rel_dump"}, {"tpl", "tpl_out"}
};

int main(int argc, char **argv) {
    ArgParser parser = ArgParser(argc, argv);
    
    if (parser.flag_count("v") == 1) {
        logging::set_default_level(logging::DEBUG);
    } else if (parser.flag_count("v") == 2) {
        logging::set_default_level(logging::TRACE);
    } else if (parser.has_flag("q")) {
        logging::set_default_level(logging::WARN);
    } else {
        logging::set_default_level(logging::INFO);
    }
    logging::Logger *log = logging::get_logger("main");

	if (parser.num_arguments() == 0) {
		std::cout << "Usage:" << '\n';
		std::cout << "gcd decomp <path to root> [directory out]" << '\n';
		std::cout << "gcd dump <path to root> [directory out]" << '\n';
		std::cout << "gcd rel <file in> [directory out]" << '\n';
		std::cout << "gcd dol <file in> [directory out]" << '\n';
		std::cout << "gcd tpl <--<extract|build>|-<e|b>> <path in> <path out>" << std::endl;
		return 0;
	}
    std::string input, output = "out";
    if (parser.num_arguments() >= 2) {
        input = parser.get_argument(1);
        if (parser.num_arguments() >= 3) {
            output = parser.get_argument(2);
        } else if (default_outs.count(parser.get_argument(0))) {
            output = default_outs[parser.get_argument(0)];
        }
    } else if (parser.has_variable("in")) {
        input = parser.get_variable("in");
        if (parser.has_variable("out")) {
            output = parser.get_variable("out");
        } else if (default_outs.count(parser.get_argument(0))) {
            output = default_outs[parser.get_argument(0)];
        }
    }
    
    if (parser.get_argument(0) == "decomp") {
        log->info("Beginning root decompile. This will take a while.");
        fs::create_directory(output);
        std::vector<types::REL*> knowns;
        types::DOL *main = nullptr;
        // Form list of files to process. Mostly RELs and DOL file.
        for (auto dir : fs::recursive_directory_iterator(parser.get_argument(1))) {
            if (ends_with(dir.path().string(), ".rel")) {
                std::string filename = dir.path().filename().string();
                types::REL *rel = new types::REL(dir.path().string());
                knowns.push_back(rel);
            } else if (ends_with(dir.path().string(), ".dol")) {
                std::string filename = dir.path().filename().string();
                main = new types::DOL(dir.path().string());
            }
        }
        // Process list of files.
        decomp_game(main, knowns, output);
        log->info("Root decompile complete");
    } else if (parser.get_argument(0) == "dump") {
        log->info("Beginnning Root Dump. This may take a while.");
        fs::create_directory(output);
        std::vector<types::REL*> knowns;
        types::DOL *main = nullptr;
        // Form list of files to process. Mostly RELs and DOL file.
        for (auto dir : fs::recursive_directory_iterator(parser.get_argument(1))) {
            if (ends_with(dir.path().string(), ".rel")) {
                std::string filename = dir.path().filename().string();
                types::REL *rel = new types::REL(dir.path().string());
                knowns.push_back(rel);
            } else if (ends_with(dir.path().string(), ".dol")) {
                std::string filename = dir.path().filename().string();
                main = new types::DOL(dir.path().string());
            }
        }
        
        // Process list of files. Disassemble, Form data lists, dump info.
        if (main == nullptr) {
            log->warn("No DOL file found");
        } else {
            fs::path path(main->filename.c_str());
            process_dol(main,
                        output + "/" + path.filename().string().substr(0, path.filename().string().length() - 4));
        }
        for (auto rel : knowns) {
            fs::path path(rel->filename.c_str());
            std::string filename = path.filename().string();
            process_rel(rel, knowns, output + "/" + filename.substr(0, filename.length() - 4));
        }
        
        // Clean up memory
        for (auto rel : knowns) {
            delete rel;
        }
        delete main;
        log->info("Root Dump complete");
    } else if (parser.get_argument(0) == "recompile") {
        types::REL rel(parser.get_argument(1));
        rel.compile(output);
    } else if (parser.get_argument(0) == "rel") {
        types::REL rel(parser.get_argument(1));
        process_rel(&rel, output);
    } else if (parser.get_argument(0) == "dol") {
        types::DOL dol(parser.get_argument(1));
        process_dol(&dol, output);
    } else if (parser.get_argument(0) == "tpl") {
        if (parser.has_flag("e") || parser.has_flag("extract")) {
            log->info("Extracting TPL " + std::string(input));
            types::TPL *tpl = types::tpl_factory(input);
            fs::create_directory(fs::path(output));
            for (uint i = 0; i < tpl->get_num_images(); ++i) {
                std::stringstream outname = std::stringstream();
                outname << output + "/" << i << ".png";
                types::PNG* png = tpl->to_png(i);
                png->save(outname.str());
                delete png;
            }
            log->info("Completed TPL extraction");
        } else if (parser.has_flag("b") || parser.has_flag("build")) {
            // TODO: build image/images into TPL
        }
    } else {
        std::cout << "Unrecognized Operation" << std::endl;
    }

	return 0;
}
