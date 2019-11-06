
#include "ppc/decompiler.h"
#include "ppc/symbol.h"
#include "ppc/instruction.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <at_logging>

namespace PPC {

using std::ios;

static logging::Logger* logger = logging::get_logger("ppc.decomp");

void decompile(const std::string& file_in, const std::string& file_out, int start, int end) {
    logger->info("Decompiling PPC");
    
    std::fstream input(file_in, ios::in | ios::binary);
    std::fstream output(file_out, ios::out);
    
    uint position = 0;
    
    if (end == -1) {
        input.seekg(0, ios::end);
        end = (int)input.tellg();
    }
    int size = end - start;
    uchar instruction[4];
    
    input.seekg(start, ios::beg);
    
    // New way
    
    std::vector<Symbol> symbols = generate_symbols(file_in, start, end);
    generate_inputs(symbols);
    
    for (auto& symbol : symbols) {
        logger->debug(symbol.name);
        
        bool start = true;
        
        output << "void " << symbol.name << "(";
        for (auto r : symbol.get_input_regular()) {
            if (!start) {
                output << ", ";
            } else {
                start = false;
            }
            output << "int32_t r" << r.number;
        }
        for (auto r : symbol.get_input_float()) {
            if (!start) {
                output << ", ";
            } else {
                start = false;
            }
            output << "float fr" << r.number;
        }
        output << ") {\n";
        
        for (auto i : symbol.instructions) {
//            if (i->code_name() == "bl") {
//                output << "" << ";";
//            }
        }
        
        output << "}\n";
    }
    
    // Old way
    
    // First read/generate/write symbol table.
    //   Read in an existing symbol table into symbol objects
    // Generate list of symbol objects from code, using existing symbol objects.
/*    bool in_func = true, q1 = false, q2 = false, q3 = false, branch = false;
    std::vector<Symbol> symbols;
    
    uint f_start = 0, f_end = 0;
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
        
        Instruction* instruct = create_instruction(instruction);
        
        if (!in_func && instruct->code_name() != "blr" && instruct->code_name() != "rfi" &&
            instruct->code_name() != "PADDING") {
            f_start = position;
            in_func = true;
        }
        
        if (instruct->code_name() == "blr" || instruct->code_name() == "rfi") {
            f_end = position;
            std::stringstream name;
            name << "f_" << f_start << "_" << f_end;
            symbols.emplace_back(Symbol(f_start, f_end, name.str()));
            in_func = false;
        } else if (instruct->code_name() == "PADDING" && branch) {
            f_end = position - 4;
            std::stringstream name;
            name << "f_" << f_start << "_" << f_end;
            symbols.emplace_back(Symbol(f_start, f_end, name.str()));
            in_func = false;
        }
        
        branch = (instruct->code_name() == "b");
        
        delete instruct;
    }
    f_end = position;
    std::stringstream name;
    name << "f_" << f_start << "_" << f_end;
    symbols.emplace_back(Symbol(f_start, f_end, name.str()));
    
    // Check internal code to determine inputs/return.
    for (auto& sym : symbols) {
        //std::cout << std::hex << sym->start << " " << sym->end << std::dec << endl;
        input.seekg(sym.start + start, ios::beg);
        
        uchar* registers = new uchar[10]();
        while (input.tellg() < (std::streamoff)sym.end + start + 4) {
            
            input.read((char*)instruction, 4);
            Instruction* instruct = create_instruction(instruction);
            
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
                sym.add_input(Register(i + 3, Register::REGULAR));
            }
        }
        
        delete[] registers;
    }
    
    // Write final list to symbol table
    for (const auto& sym : symbols) {
        output << "f_" << std::hex << sym.start << "_" << sym.end << std::dec << "(";
        bool first = true;
        for (auto num : sym.r_input) {
            if (!first) {
                output << ", ";
            } else {
                first = false;
            }
            output << "r" << (int)num;
        }
        output << ");" << '\n';
    }*/
    
    // Start,Stop,Output,Name,[Type,Input]
    
    // Second read/generate/write C function internals
    //   Parse relocations if this is a rel
    
    // Third read/generate/write output files
    
    output.close();
    
    logger->info("PPC decompile finished");
}

}
