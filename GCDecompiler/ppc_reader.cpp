
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <set>
#include <unordered_map>
#include "logging.h"
#include "types.h"
#include "filetypes/rel.h"
#include "utils.h"
#include "ppc/register.h"
#include "ppc/instruction.h"
#include "ppc/symbol.h"

namespace PPC {

using std::ios;

static logging::Logger *logger = logging::get_logger("ppc");

Instruction* create_instruction(const uchar *instruction) {
    const uchar opcode = (uchar)get_range(instruction, 0, 5);

    if (opcode == 4) return new PairedSingleFamily(opcode, instruction);
    else if (opcode == 10 || opcode == 11) return new CmpFamily(opcode, instruction);
    else if (opcode >= 12 && opcode <= 15) return new AddFamily(opcode, instruction);
    else if (opcode == 16 || opcode == 18) return new BFamily(opcode, instruction);
    else if (opcode == 19) return new SpecBranchFamily(opcode, instruction);
    else if (opcode == 23) return new ConditionInstruction(opcode, instruction);
    else if (opcode == 31) return new MathFamily(opcode, instruction);
    else if (opcode == 59) return new FloatSingleFamily(opcode, instruction);
    else if (opcode == 63) return new FloatDoubleFamily(opcode, instruction);
    else return new Instruction(opcode, instruction);
}

void relocate(types::REL *rel, const uint& bss_pos, const std::string& file_out) {
    logger->info("Relocating file");

    std::fstream input = std::fstream(rel->filename, ios::binary | ios::in);
    std::fstream output = std::fstream(file_out, ios::binary | ios::out);

    char *data = new char[rel->file_size];
    input.read(data, rel->file_size);
    output.write(data, rel->file_size);

    for (auto imp = rel->imports.begin(); imp != rel->imports.end(); imp++) {
        if (imp->module == rel->id) {
            for (auto reloc = imp->instructions.begin(); reloc != imp->instructions.end(); reloc++) {
                uint abs_offset = reloc->get_src_offset();
                if (reloc->get_src_section().address == 0) {
                    abs_offset += bss_pos;
                }

                // Reloc in the output file
                output.seekg(reloc->get_dest_offset());
                switch (reloc->type) {
                case R_PPC_ADDR32:
                    write_int(output, abs_offset, 4);
                    break;
                case R_PPC_ADDR24:
                    write_int(output, abs_offset, 3);
                    break;
                case R_PPC_ADDR16:
                    write_int(output, abs_offset, 2);
                    break;
                case R_PPC_ADDR16_LO:
                    write_int(output, abs_offset & ((uint)pow(2, 16) - 1), 2);
                    break;
                case R_PPC_ADDR16_HI:
                    write_int(output, abs_offset << 16u, 2);
                    break;
                case R_PPC_ADDR16_HA:
                    write_int(output, (abs_offset << 16u) + 0x10000, 2);
                    break;
                case R_PPC_REL24:
                    write_int(output, abs_offset - reloc->get_dest_offset());
                    break;
                default:
                    write_int(output, 0);
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

void disassemble(const std::string& file_in, const std::string& file_out, int start, int end) {
    logger->info("Disassembling PPC");
    
    std::fstream input(file_in, ios::in | ios::binary);
    std::fstream output(file_out, ios::out);

    uint position;
    bool func_end = true, q1 = false, q2 = false, q3 = false;

    if (end == -1) {
        input.seekg(0, ios::end);
        end = (int)input.tellg();
    }
    const int size = end - start;
    const int hex_length = (int)std::floor((std::log(size) / std::log(16)) + 1) + 2;
    uchar instruction[4];

    input.seekg(start, ios::beg);
    while (input.tellg() < end) {
        position = (uint)input.tellg() - start;
        
        if ((float)position / size > .25 && !q1) {
            logger->info("  25% Complete");
            q1 = true;
        } else if ((float)position / size > .5 && !q2) {
            logger->info("  50% Complete");
            q2 = true;
        } else if ((float)position / size > .75 && !q3) {
            logger->info("  75% Complete");
            q3 = true;
        }

        input.read((char*)instruction, 4);

        Instruction *instruct = create_instruction(instruction);

        if (instruct->code_name() == "blr" || instruct->code_name() == "rfi") {
            func_end = true;
        }

        if (func_end && instruct->code_name() != "blr" && instruct->code_name() != "rfi" && instruct->code_name() != "PADDING") {
            output << "; function: f_" << std::hex << position << std::dec << " at " << itoh(position) << '\n';
            func_end = false;
        }

        std::string hex = itoh(position);
        std::string padding(hex_length - hex.length(), ' ');
        output << padding << hex << "    " << ctoh(instruction[0]) << " " << ctoh(instruction[1]) << " " <<
            ctoh(instruction[2]) << " " << ctoh(instruction[3]) << "    ";

        output << instruct->code_name();
        output << " " << instruct->get_variables();
        output << std::endl;
        delete instruct;
    }
    
    input.close();
    output.close();
    
    logger->info("PPC disassembly finished");
}

void read_data(const std::string& file_in, const std::string& file_out, int start, int end) {
    logger->info("Reading data section");
    
    std::fstream input(file_in, ios::in | ios::binary);
    std::fstream output(file_out, ios::out);

    //TODO: Data guessing (tm)
    
    input.close();
    output.close();
    
    logger->info("Finished reading data section");
}

void read_data(types::REL *to_read, Section *section, const std::vector<types::REL*>& knowns, const std::string& file_out) {
    // open the file, look through every import in every REL file to check if they refer to a
    // location in the right area in this one. If they do, add it to the list. Once done, go through
    // the list and generate output values. Anything leftover goes in a separate section clearly marked.
    logger->info("Reading REL data section");
    
    std::fstream output(file_out, ios::out);
    
    std::set<int> offsets;
    for (auto rel_r = knowns.begin(); rel_r != knowns.end(); ++rel_r) {
        types::REL *rel = (*rel_r);
        for (auto imp = rel->imports.begin(); imp != rel->imports.end(); ++imp) {
            if (imp->module != to_read->id && rel->id != to_read->id) {
                continue;
            }
            for (auto reloc = imp->instructions.begin(); reloc != imp->instructions.end(); ++reloc) {
                if (reloc->get_src_section().id == section->id && imp->module == rel->id) {
                    offsets.insert(reloc->get_src_offset());
                }
                if (rel->id == to_read->id && reloc->get_dest_section().id == section->id) {
                    offsets.insert(reloc->get_dest_offset());
                }
            }
        }
    }
    for (int offset : offsets) {
        output << itoh(offset) << ": ";
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
            out = ctoh(section->get_data()[lookahead++ - section->offset]);
            while (offsets.find(lookahead) == offsets.end() && lookahead - offset < 5) {
                out += ctoh(section->get_data()[lookahead++ - section->offset]);
            }
            if (!out.empty()) {
                output << out << '\n';
            }
        }
    }
    
    output.close();
    
    logger->info("Finished reading REL data section");
}

void decompile(const std::string& file_in, const std::string& file_out, int start, int end) {
    logger->info("Decompiling PPC");
    
    std::fstream input(file_in, ios::in | ios::binary);
    std::fstream output(file_out, ios::out);

    uint position = 0;
    bool in_func = true, q1 = false, q2 = false, q3 = false, branch = false;

    if (end == -1) {
        input.seekg(0, ios::end);
        end = (int)input.tellg();
    }
    int size = end - start;
    uchar instruction[4];

    input.seekg(start, ios::beg);

    // First read/generate/write symbol table.
    //   Read in an existing symbol table into symbol objects
    // Generate list of symbol objects from code, using existing symbol objects.
    std::vector<Symbol> symbols;
    uint f_start = 0, f_end = 0;
    while (input.tellg() < end) {
        
        position = (ulong)input.tellg() - start;

        if ((float)position / size > .25 && !q1) {
            logger->info("  25% Complete");
            q1 = true;
        } else if ((float)position / size > .5 && !q2) {
            logger->info("  50% Complete");
            q2 = true;
        } else if ((float)position / size > .75 && !q3) {
            logger->info("  75% Complete");
            q3 = true;
        }

        input.read((char*)instruction, 4);

        Instruction *instruct = create_instruction(instruction);

        if (!in_func && instruct->code_name() != "blr" && instruct->code_name() != "rfi" && instruct->code_name() != "PADDING") {
            f_start = position;
            in_func = true;
        }

        if (instruct->code_name() == "blr" || instruct->code_name() == "rfi") {
            f_end = position;
            std::stringstream name;
            name << "f_" << f_start << "_" << f_end;
            symbols.push_back(Symbol(f_start, f_end, name.str()));
            in_func = false;
        } else if (instruct->code_name() == "PADDING" && branch) {
            f_end = position - 4;
            std::stringstream name;
            name << "f_" << f_start << "_" << f_end;
            symbols.push_back(Symbol(f_start, f_end, name.str()));
            in_func = false;
        }

        branch = (instruct->code_name() == "b");

        delete instruct;
    }
    f_end = position;
    std::stringstream name;
    name << "f_" << f_start << "_" << f_end;
    symbols.push_back(Symbol(f_start, f_end, name.str()));

    // Check internal code to determine inputs/return.
    for (auto sym = symbols.begin(); sym != symbols.end(); sym++) {
        //std::cout << std::hex << sym->start << " " << sym->end << std::dec << endl;
        input.seekg(sym->start + start, ios::beg);

        uchar *registers = new uchar[10]();
        while (input.tellg() < sym->end + start + 4) {

            input.read((char*)instruction, 4);
            Instruction *instruct = create_instruction(instruction);

            std::set<Register> sources = instruct->source_registers();
            for (int i = 0; i < 10; i++) {
                if (registers[i] == 0) {
                    const Register tester = Register(i + 3, Register::REGULAR);
                    if (sources.find(tester) != sources.end()) {
                        registers[i] = 1;
                    } else if (instruct->destination_register() == tester) {
                        registers[i] = 2;
                    }
                }
            }

            delete instruct;
        }
        
        for (int i = 0; i < 10; ++i) {
            if (registers[i] == 1) {
                sym->add_source(Register(i + 3, Register::REGULAR));
            }
        }

        delete[] registers;
    }

    // Write final list to symbol table
    for (auto sym = symbols.begin(); sym != symbols.end(); sym++) {
        output << "f_" << std::hex << sym->start << "_" << sym->end << std::dec << "(";
        bool start = true;
        for (auto num = sym->r_input.begin(); num != sym->r_input.end(); ++num) {
            if (!start) {
                output << ", ";
            } else {
                start = false;
            }
            output << "r" << ((int)*num);
        }
        output << ");" << '\n';
    }

    // Start,Stop,Output,Name,[Type,Input]

    // Second read/generate/write C function internals
    //   Parse relocations if this is a rel

    // Third read/generate/write output files
    
    output.close();
    
    logger->info("PPC decompile finished");
}

}