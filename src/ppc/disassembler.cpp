
#include <cmath>
#include <fstream>
#include <at_utils>
#include <at_logging>

#include "types.h"
#include "ppc/instruction.h"
#include "ppc/symbol.h"
#include "ppc/disassembler.h"

namespace PPC {

using std::ios;

static logging::Logger* logger = logging::get_logger("ppc.dis");

void disassemble(const std::string& file_in, const std::string& file_out, int start, int end, bool info) {
    logger->info("Disassembling PPC");
    
    std::fstream input(file_in, ios::in | ios::binary);
    std::fstream output(file_out, ios::out);
    
    uint position;
    
    if (end == -1) {
        input.seekg(0, ios::end);
        end = (int)input.tellg();
    }
    
    const int size = end - start;
    const int hex_length = (int)std::floor((std::log(size) / std::log(16)) + 1) + 2;
    uchar instruction[4];
    
    // New style
    std::vector<Symbol> symbs = generate_symbols(file_in, start, end);
    for (const auto& symbol : symbs) {
        logger->debug(symbol.name);
        
        output << "; function: " << symbol.name << " at " << util::ltoh(symbol.start) << "\n";
        
        input.seekg(symbol.start);
        while (input.tellg() <= (std::streamoff)symbol.end) {
            position = (ulong)input.tellg() - start;
            
            input.read((char*)instruction, 4);
            
            Instruction* instruct = create_instruction(instruction);
            
            std::string hex = util::itoh(position);
            std::string padding(hex_length - hex.length(), ' ');
            
            if (info) {
                output << padding << hex << "    ";
                output << util::ctoh(instruction[0], false, true) << " " << util::ctoh(instruction[1], false, true)
                       << " "
                       << util::ctoh(instruction[2], false, true) << " " << util::ctoh(instruction[3], false, true)
                       << "    ";
            }
            output << instruct->code_name() << " " << instruct->get_variables() << "\n";
            
            delete instruct;
        }
    }
    
    input.close();
    output.close();
    
    logger->info("PPC disassembly finished");
}

}
