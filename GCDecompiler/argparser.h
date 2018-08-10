//
// Created by Rune Tynan on 8/9/2018.
//

#pragma once

#include <string>
#include <vector>
#include <map>

class ArgParser {
    
    std::vector<std::string> flags;
    std::vector<std::string> arguments;
    std::map<std::string, std::string> variables;
    
public:

    ArgParser(const int& argc, char **argv);
    
    uint num_flags();
    uint num_arguments();
    uint num_variables();
    
    bool has_flag(const std::string& flag);
    
    std::string get_argument(uint index);
    
    bool has_variable(const std::string& var);
    std::string get_variable(const std::string& var);

};
