
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <experimental/filesystem>

#include "at_logging"
#include "at_utils"
#include "types.h"
#include "gcd_main.h"
#include "ppc/ppc_reader.h"
#include "filetypes/lz.h"
#include "filetypes/tpl.h"
#include "filetypes/png.h"
#include "argparser.h"

namespace fs = std::experimental::filesystem;

logging::Logger *log = logging::get_logger("main");

void process_rel(types::REL *rel, const std::vector<types::REL*>& knowns, const std::string& output) {
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
    
	for (auto sect = rel->sections.begin(); sect != rel->sections.end(); ++sect) {
		if (!sect->exec && sect->offset != 0 && sect->length > 4 && rel->id == 1) {
			std::stringstream name;
			name << output << "/Section" << sect->id << ".ppd";
			PPC::read_data(rel, &*sect, knowns, name.str());
		}
	}
}

int command_decomp(const std::string& input, const std::string& output) {
    log->info("Beginning root decompile. This will take a while.");
    fs::create_directory(output);
    std::vector<types::REL*> knowns;
    types::DOL *main = nullptr;
    // Form list of files to process. Mostly RELs and DOL file.
    for (const auto& dir : fs::recursive_directory_iterator(input)) {
        if (util::ends_with(dir.path().string(), ".rel")) {
            std::string filename = dir.path().filename().string();
            types::REL *rel = new types::REL(dir.path().string());
            knowns.push_back(rel);
        } else if (util::ends_with(dir.path().string(), ".dol")) {
            std::string filename = dir.path().filename().string();
            main = new types::DOL(dir.path().string());
        }
    }
    
    if (main == nullptr) {
        log->error("Couldn't find .dol file for game");
        return 1;
    }
    // Process list of files.
	for (auto sect = main->sections.begin(); sect != main->sections.end(); ++sect) {
		if (sect->exec && sect->offset) {
			std::stringstream name;
			name << output << "/Section" << sect->id << ".c";
			// TODO: do relocations on each REL
			PPC::decompile(main->filename, name.str(), sect->offset, sect->offset + sect->length);
		}
	}
    
    log->info("Root decompile complete");
	return 0;
}

int command_dump(const std::string& input, const std::string& output) {
    log->info("Beginnning Root Dump. This may take a while.");
    fs::create_directory(output);
    std::vector<types::REL*> knowns;
    types::DOL *main = nullptr;
    // Form list of files to process. Mostly RELs and DOL file.
    for (const auto& dir : fs::recursive_directory_iterator(input)) {
        if (util::ends_with(dir.path().string(), ".rel")) {
            std::string filename = dir.path().filename().string();
            types::REL *rel = new types::REL(dir.path().string());
            knowns.push_back(rel);
        } else if (util::ends_with(dir.path().string(), ".dol")) {
            std::string filename = dir.path().filename().string();
            main = new types::DOL(dir.path().string());
        }
    }
    
    // Process list of files. Disassemble, Form data lists, dump info.
    if (main == nullptr) {
        log->warn("No DOL file found");
    } else {
        fs::path path(main->filename.c_str());
        command_dol(main->filename,
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
    return 0;
}

int command_recomp(const std::string& input, const std::string& output) {
    types::REL rel(input);
    rel.compile(output);
    return 0;
}

int command_rel(const std::string& input, const std::string& output) {
    types::REL rel(input);
    fs::create_directory(fs::path(output));
    rel.dump_header(output + "/header.txt");
    rel.dump_sections(output + "/sections.txt");
    rel.dump_imports(output + "/imports.txt");
    for (const auto& sect : rel.sections) {
        if (sect.exec && sect.offset) {
            std::stringstream name;
            name << output << "/Section" << sect.id << ".ppc";
            PPC::disassemble(rel.filename, name.str(), sect.offset, sect.offset + sect.length);
        }
    }
    return 0;
}

int command_dol(const std::string& input, const std::string& output) {
    types::DOL dol(input);
    fs::create_directory(fs::path(output));
    dol.dump_all(output + "/dol.txt");
    for (const auto& sect : dol.sections) {
        if (sect.exec && sect.offset) {
            std::stringstream name;
            name << output << "/Section" << sect.id << ".ppc";
            PPC::disassemble(dol.filename, name.str(), sect.offset, sect.offset + sect.length);
        }
    }
    return 0;
}

int command_tpl(const std::string& input, const std::string& output, ArgParser& parser) {
    if (parser.has_flag("e") || parser.has_flag("extract")) {
        log->info("Extracting TPL " + std::string(input));
        types::TPL *tpl = types::tpl_factory(input);
        if (tpl == nullptr) {
            return 1;
        }
        fs::create_directory(fs::path(output));
        for (uint i = 0; i < tpl->get_num_images(); ++i) {
            for (uint j = 0; j < tpl->get_num_mipmaps(i); ++j) {
                std::stringstream outname = std::stringstream();
                outname << output << "/" << i << "_" << j << ".png";
                std::string filename = outname.str();
                
                types::PNG *png = tpl->to_png(i, j);
                log->info("Writing PNG to " + filename);
                png->save(filename);
                
                delete png;
            }
        }
        delete tpl;
        log->info("Completed TPL extraction");
    } else if (parser.has_flag("b") || parser.has_flag("build")) {
        // TODO: build image/images into TPL
        log->info("Building TPL from " + std::string(input));
        log->warn("This feature is not yet complete");
        std::vector<types::Image*> images = std::vector<types::Image*>();
        for (const auto& file : fs::directory_iterator(input)) {
            if (util::ends_with(file.path().filename(), ".png")) {
                types::PNG png = types::PNG(file.path().filename());
                types::Image *image = new types::Image(png.get_image());
                images.push_back(image);
            }
        }
        types::TPL* tpl = new types::GCTPL(images);
        tpl->save(output);
    }
    return 0;
}

static std::map<std::string, std::string> default_outs = {
        {"decomp", "root_decomp"}, {"dump", "root_dump"}, {"recompile", "recompile.rel"}, {"dol", "dol_dump"},
        {"rel", "rel_dump"}, {"tpl", "tpl_out"}
};

static std::map<std::string, int> commands = {
        {"decomp", 1}, {"dump", 2}, {"recomp", 3}, {"rel", 4}, {"dol", 5}, {"tpl", 6}
};

int main(int argc, char **argv) {
    ArgParser parser = ArgParser(argc, argv);

	if (parser.num_arguments() == 0 && parser.has_flag("help")) {
		std::cout << "Usage:\n";
		std::cout << "  gcd <decomp|dump|rel|dol|tpl> [args]...\n";
		std::cout << "Description:\n";
		std::cout << "  The GameCube Decompiler is a tool designed to assist in working with GameCube hacking, especially ";
		std::cout << "monkey ball. Still in alpha; send any inquiries, bug reports, or feature requests to CraftSpider.\n";
		std::cout << "Flags:\n";
		std::cout << "  -in=<path>: equivalent to an input argument\n";
		std::cout << "  -out=<path>: equivalent to an output argument\n";
		std::cout << "  -v: verbose logging\n";
		std::cout << "  -vv: super verbose logging\n";
		std::cout << "  -q: quiet logging\n";
		std::cout << "  -qq: super quiet logging\n";
		std::cout.flush();
		return 0;
	} else if (parser.has_flag("help")) {
	    std::string subcom = parser.get_argument(0);
	    std::stringstream usage;
        usage << "Usage:\n";
	    if (subcom == "decomp") {
            usage << "  gcd decomp <root in> [path out]\n";
            usage << "Description:\n";
	    } else if (subcom == "dump") {
            usage << "  gcd dump <root in> [path out]\n";
	    } else if (subcom == "rel") {
            usage << "  gcd rel <file in> [directory out]\n";
	    } else if (subcom == "dol") {
            usage << "  gcd dol <file in> [directory out]\n";
	    } else if (subcom == "tpl") {
            usage << "  gcd tpl <-e|-b|--extract|--build> <path in> [path out]\n";
	    } else {
	        std::cout << "Unknown subcommand, no help available. do `gcd --help` to see all subcommands.\n";
	    }
	    std::cout << usage.str();
        // TODO: descriptions
        // TODO: flags
        std::cout.flush();
	}
    
    if (parser.flag_count("v") == 1) {
        logging::set_default_level(logging::DEBUG);
    } else if (parser.flag_count("v") == 2) {
        logging::set_default_level(logging::TRACE);
    } else if (parser.flag_count("q") == 1) {
        logging::set_default_level(logging::WARN);
    } else if (parser.flag_count("q") == 2) {
        logging::set_default_level(logging::ERROR);
    } else {
        logging::set_default_level(logging::INFO);
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
    
    int command = commands[parser.get_argument(0)];
    
    switch (command) {
        case 1:
            return command_decomp(input, output);
        case 2:
            return command_dump(input, output);
        case 3:
            return command_recomp(input, output);
        case 4:
            return command_rel(input, output);
        case 5:
            return command_dol(input, output);
        case 6:
            return command_tpl(input, output, parser);
        default:
            std::cout << "Unrecognized Subcommand" << std::endl;
            return 1;
    }
}
