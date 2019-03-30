
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <set>
#include <unordered_map>

#include "at_logging"
#include "at_utils"
#include "types.h"
#include "filetypes/rel.h"
#include "ppc/register.h"
#include "ppc/instruction.h"
#include "ppc/symbol.h"

namespace PPC {

using std::ios;

static logging::Logger *logger = logging::get_logger("ppc");

void relocate(types::REL *rel, const uint& bss_pos, const std::string& file_out) {
    logger->info("Relocating file");

    std::fstream input = std::fstream(rel->filename, ios::binary | ios::in);
    std::fstream output = std::fstream(file_out, ios::binary | ios::out);

    char *data = new char[rel->file_size];
    input.read(data, rel->file_size);
    output.write(data, rel->file_size);

    for (const auto& imp : rel->imports) {
        if (imp.module == rel->id) {
            for (auto reloc : imp.instructions) {
                uint abs_offset = reloc.get_src_offset();
                if (reloc.get_src_section().address == 0) {
                    abs_offset += bss_pos;
                }

                // Reloc in the output file
                output.seekg(reloc.get_dest_offset());
                switch (reloc.type) {
                case R_PPC_ADDR32:
                    util::write_uint(output, abs_offset, 4);
                    break;
                case R_PPC_ADDR24:
                    util::write_uint(output, abs_offset, 3);
                    break;
                case R_PPC_ADDR16:
                    util::write_uint(output, abs_offset, 2);
                    break;
                case R_PPC_ADDR16_LO:
                    util::write_uint(output, abs_offset & ((uint)pow(2, 16) - 1), 2);
                    break;
                case R_PPC_ADDR16_HI:
                    util::write_uint(output, abs_offset << 16u, 2);
                    break;
                case R_PPC_ADDR16_HA:
                    util::write_uint(output, (abs_offset << 16u) + 0x10000, 2);
                    break;
                case R_PPC_REL24:
                    util::write_uint(output, abs_offset - reloc.get_dest_offset());
                    break;
                default:
                    util::write_uint(output, 0);
                    break;
                }
            }
        }
    }
    
    logger->info("Relocation complete");
}

void relocate(types::REL *input, const std::string& file_out) {
    relocate(input, input->file_size, file_out);
}

void read_data(const std::string& file_in, const std::string& file_out, int start, int end) {
    logger->info("Reading data section");
    
    std::fstream input(file_in, ios::in | ios::binary);
    std::fstream output(file_out, ios::out);

    input.seekg(start);
    while(input.tellg() != end) {
        //TODO: Data guessing (tm)
    }
    
    input.close();
    output.close();
    
    logger->info("Finished reading data section");
}

void read_data(types::REL *to_read, const Section *section, const std::vector<types::REL*>& knowns, const std::string& file_out) {
    // open the file, look through every import in every REL file to check if they refer to a
    // location in the right area in this one. If they do, add it to the list. Once done, go through
    // the list and generate output values. Anything leftover goes in a separate section clearly marked.
    logger->info("Reading REL data section");
    
    std::fstream output(file_out, ios::out);
    
    std::set<int> offsets;
    for (auto rel : knowns) {
        for (auto& imp : rel->imports) {
            if (imp.module != to_read->id && rel->id != to_read->id) {
                continue;
            }
            for (auto& reloc : imp.instructions) {
                if (reloc.get_src_section().id == section->id && imp.module == rel->id) {
                    offsets.insert(reloc.get_src_offset());
                }
                if (rel->id == to_read->id && reloc.get_dest_section().id == section->id) {
                    offsets.insert(reloc.get_dest_offset());
                }
            }
        }
    }
    for (int offset : offsets) {
        output << util::itoh(offset) << ": ";
        // Need to add non ASCII stuff later
        int lookahead = offset;
        char dat = section->get_data()[lookahead++ - section->offset];
        std::string out;
        while (dat >= 32 && dat <= 126 && offsets.find(lookahead) == offsets.end()) {
            out.append({ dat });
            dat = section->get_data()[lookahead++ - section->offset];
        }
        if (dat == 0 && !out.empty()) {
            output << out << '\n';
        } else {
            lookahead = offset;
            out = util::ctoh(section->get_data()[lookahead++ - section->offset], false);
            while (offsets.find(lookahead) == offsets.end() && lookahead - offset < 5) {
                out += util::ctoh(section->get_data()[lookahead++ - section->offset], false);
            }
            if (!out.empty()) {
                output << out << '\n';
            }
        }
    }
    
    output.close();
    
    logger->info("Finished reading REL data section");
}

}