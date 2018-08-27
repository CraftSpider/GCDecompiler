#pragma once

#include <string>
#include <vector>
#include "level.h"
#include "handler.h"

namespace logging {

class Logger {
    
    std::string name;
    Level level;
    std::vector<Handler*> handlers;
    Logger* parent = nullptr;
    
    Level get_effective_level();

public:
    
    Logger();
    Logger(const std::string& name);
    
    void set_level(const Level& level);
    void set_parent(Logger* parent);
    
    void log(const std::string& message, const Level& level);
    void trace(const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);
    
    void add_handler(Handler *handler);
    bool remove_handler(Handler *handler);
    
};

}