#pragma once

#include <vector>
#include <string>
#include "types.h"
#include "section.h"

namespace types {

class DOL {

public:
    uint entry_offset, bss_address, bss_size;
    std::string filename;
    
    std::vector<Section> sections;
    
    DOL(const std::string& filename);
    
    std::string dump_all();
    void dump_all(const std::string& filename);
    
};

}