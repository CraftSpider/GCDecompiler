//
// Created by Rune Tynan on 8/5/2018.
//

#include <iostream>
#include <map>
#include "logging.h"

#define DEFAULT_LOGGER_LEVEL (INFO)

namespace logging {

static std::map<std::string, Logger*> loggers = std::map<std::string, Logger*>();
static ConsoleHandler *ch = new ConsoleHandler();

Level::Level() {
    this->name = "NULL";
    this->priority = 0;
}

Level::Level(const int &priority, const std::string &name) {
    this->name = name;
    this->priority = priority;
}

std::string Level::get_name() {
    return name;
}

bool Level::operator==(const Level& level) const {
    return priority == level.priority && name == level.name;
}

bool Level::operator <=(const Level& level) const {
    return priority <= level.priority;
}

bool Level::operator>=(const Level& level) const {
    return priority >= level.priority;
}

Level::operator std::string() const {
    return name;
}

Handler::Handler() {
    this->level = TRACE;
}

void Handler::set_level(const Level &level) {
    this->level = level;
}

void ConsoleHandler::log(const std::string &message, const Level &level) {
    if (level >= this->level) {
        std::cout << (std::string)level << ": " << message << std::endl;
    }
}

Logger::Logger() {
    this->name = "NULL";
    level = NO_LEVEL;
    handlers = std::vector<Handler*>();
}

Logger::Logger(const std::string &name) : Logger() {
    this->name = name;
}

Level Logger::get_effective_level() {
    if (level == NO_LEVEL) {
        return parent->get_effective_level();
    }
    return level;
}

void Logger::set_level(const Level &level) {
    if (name == "root" && level == NO_LEVEL) {
        throw std::runtime_error("Root logger cannot have NO_LEVEL");
    }
    this->level = level;
}

void Logger::set_parent(Logger* parent) {
    this->parent = parent;
}

void Logger::log(const std::string &message, const Level &level) {
    if (level >= get_effective_level()) {
        for (auto handler : handlers) {
            handler->log(message, level);
        }
    }
}

void Logger::trace(const std::string &message) {
    log(message, TRACE);
}

void Logger::debug(const std::string &message) {
    log(message, DEBUG);
}

void Logger::info(const std::string &message) {
    log(message, INFO);
}

void Logger::warn(const std::string &message) {
    log(message, WARN);
}

void Logger::error(const std::string &message) {
    log(message, ERROR);
}

void Logger::fatal(const std::string &message) {
    log(message, FATAL);
}

void Logger::add_handler(Handler *handler) {
    handlers.push_back(handler);
}

bool Logger::remove_handler(Handler *handler) {
    for (uint i = 0; i < handlers.size(); ++i) {
        if (handlers[i] == handler) {
            handlers.erase(handlers.begin() + i);
            return true;
        }
    }
    return false;
}

void set_default_level(const Level& level) {
    get_root_logger()->set_level(level);
}

Logger* get_root_logger() {
    if (loggers.count("root")) {
        return loggers["root"];
    }
    Logger *log = new Logger("root");
    log->set_level(DEFAULT_LOGGER_LEVEL);
    loggers.emplace("root", log);
    log->add_handler(ch);
    return log;
}

Logger* get_logger(const std::string& name) {
    if (loggers.count(name)) {
        return loggers[name];
    }
    Logger *log = new Logger(name);
    log->set_parent(get_root_logger());
    loggers.emplace(name, log);
    log->add_handler(ch);
    return log;
}

}