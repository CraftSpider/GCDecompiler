//
// Created by Rune Tynan on 8/9/2018.
//

#pragma once

#include <string>
#include <vector>
#include <map>

class ArgParser {
    
    std::string invokation;
    std::vector<std::string> raw_args;
    
    std::vector<std::string> flags;
    std::vector<std::string> arguments;
    std::map<std::string, std::string> variables;
    
public:

    ArgParser(const int& argc, char **argv);
    
    ulong num_flags();
    ulong num_arguments();
    ulong num_variables();
    
    bool has_flag(const std::string& flag);
    uint flag_count(const std::string& flag);
    
    std::string get_argument(uint index);
    
    bool has_variable(const std::string& var);
    std::string get_variable(const std::string& var);

};
