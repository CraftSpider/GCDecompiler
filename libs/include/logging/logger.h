#pragma once

#include <string>
#include <vector>
#include "level.h"
#include "handler.h"

#define DEFAULT_LOGGER_LEVEL (logging::Level::INFO)

namespace logging {

class Logger {

protected:
    
    bool propagate = true;
    Logger* parent = nullptr;
    std::string name;
    std::string pattern = "";
    Level level;
    std::vector<Handler*> handlers;
    
    Level get_effective_level() const;
    std::string get_effective_pattern() const;
    std::string format_instruct(const std::string &instruct, std::string message, const Level& level);
    std::string log_format(const std::string &message, const Level& level);

public:
    
    Logger();
    explicit Logger(const std::string& name);
    
    void set_level(const Level& level);
    Level get_level() const;
    
    void set_pattern(const std::string& pattern);
    std::string get_pattern() const;

    void set_parent(Logger* parent);
    Logger* get_parent() const;

    void set_propagation(bool propagate);
    bool get_propagation() const;
    
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