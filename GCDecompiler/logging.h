//
// Created by Rune Tynan on 8/5/2018.
//

#pragma once

#include <string>
#include <vector>

namespace logging {

class Level {
    
    std::string name;
    int priority;
    
public:
    
    Level();
    Level(const int& priority, const std::string& name);
    
    std::string get_name();
    
    bool operator ==(const Level& level) const;
    bool operator <=(const Level& level) const;
    bool operator >=(const Level& level) const;
    
    operator std::string() const;
    
};

static const Level NO_LEVEL = Level();
static const Level TRACE = Level(0, "TRACE");
static const Level DEBUG = Level(10, "DEBUG");
static const Level INFO = Level(20, "INFO");
static const Level WARN = Level(30, "WARN");
static const Level ERROR = Level(40, "ERROR");
static const Level FATAL = Level(50, "FATAL");

class Handler {

protected:
    
    Level level;

public:
    
    Handler();
    
    void set_level(const Level& level);
    
    virtual void log(const std::string& message, const Level& level) = 0;
    
};

class ConsoleHandler : public Handler{

public:
    
    void log(const std::string& message, const Level& level) override;
    
};

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

void set_default_level(const Level& level);
Logger* get_root_logger();
Logger* get_logger(const std::string& name);

}