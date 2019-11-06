
#include <set>
#include <cstring>
#include <sstream>
#include <fstream>
#include <at_logging>

#include "ppc/symbol.h"
#include "ppc/instruction.h"

namespace PPC {

using std::ios;

static logging::Logger* logger = logging::get_logger("ppc.symb");

Symbol::Symbol(ulong start, ulong end, const std::string& name) {
    this->start = start;
    this->end = end;
    this->name = name;
    
    this->r_input = std::set<Register>();
    this->inputs_made = false;
}

void Symbol::gen_inputs() {
    std::set<Register> seen_dests;
    
    for (auto i : instructions) {
        for (auto r : i->source_registers()) {
            if (!seen_dests.count(r)) {
                if (r.type == Register::REGULAR) {
                    r_input.emplace(r);
                } else if (r.type == Register::FLOAT) {
                    fr_input.emplace(r);
                }
                seen_dests.emplace(r);
            }
        }
        
        Register r = i->destination_register();
        seen_dests.emplace(r);
    }
}

const std::set<Register>& Symbol::get_input_regular() {
    if (!inputs_made)
        gen_inputs();
    return this->r_input;
}

const std::set<Register>& Symbol::get_input_float() {
    if (!inputs_made)
        gen_inputs();
    return this->fr_input;
}

std::vector<Symbol> generate_symbols(const std::string& file_in, int start, int end) {
    logger->info("Generating symbols");
    
    std::fstream input(file_in, ios::in | ios::binary);
    
    if (end == -1) {
        input.seekg(0, ios::end);
        end = (int)input.tellg();
    }
    
    std::vector<Symbol> out = std::vector<Symbol>();
    std::vector<Instruction*> cur_instructions = std::vector<Instruction*>();
    uint sym_start = start, sym_end = 0, position = start;
    uchar inst[4];
    bool skip_padding = true;
    
    input.seekg(start, ios::beg);
    while (input.tellg() < end) {
        
        input.read((char*)inst, 4);
        
        Instruction* instruction = create_instruction(inst);
        
        if (instruction->code_name() == "blr" || instruction->code_name() == "rfi") {
            sym_end = position;
            
            std::stringstream name;
            
            if (instruction->code_name() == "blr") {
                name << "f_";
            } else if (instruction->code_name() == "rfi") {
                name << "i_";
            }
            
            name << std::hex << sym_start - start;
            Symbol symb = Symbol(sym_start, sym_end, name.str());
            symb.instructions = std::move(cur_instructions);
            out.emplace_back(symb);
            
            sym_start = position + 4;
            cur_instructions = std::vector<Instruction*>();
            skip_padding = true;
        } else if (instruction->code_name() == "PADDING" && skip_padding) {
            sym_start += 4;
        } else {
            cur_instructions.emplace_back(instruction);
            skip_padding = false;
        }
        
        position += 4;
    }
    
    logger->info("Symbol generation complete");
    return out;
}

void generate_inputs(std::vector<Symbol>& symbols) {
    
    for (auto& symbol : symbols) {
        // Go through each instruction, find its inputs
        
    }
}

std::vector<Symbol> load_symbols(const std::string& file_in) {
    // TODO: read inputs
    
    std::fstream input(file_in, ios::in);
    
    std::vector<Symbol> out = std::vector<Symbol>();
    std::string line, name;
    std::stringstream token;
    uint i = 0;
    ulong start = 0, end = 0;
    
    while (std::getline(input, line)) {
        for (auto ch : line) {
            if (ch == ';') {
                switch (i) {
                    case 0:
                        name = token.str();
                        break;
                    case 1:
                        token >> start;
                        break;
                    case 2:
                        token >> end;
                        break;
                }
                
                token.str("");
                token.clear();
                i++;
            } else {
                token << ch;
            }
        }
        
        Symbol symb = Symbol(start, end, name);
        i = 0;
    }
    
    return out;
}

void write_symbols(const std::vector<Symbol>& symbols, const std::string& file_out) {
    // TODO: write inputs
    
    std::fstream output(file_out, ios::out);
    
    for (const auto& symb : symbols) {
        output << symb.name << ";" << symb.start << ";" << symb.end << "\n";
    }
}

}