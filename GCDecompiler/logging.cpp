//
// Created by Rune Tynan on 8/5/2018.
//

#include <iostream>
#include <map>
#include "logging.h"

namespace logging {

static std::map<std::string, Logger*> loggers = std::map<std::string, Logger*>();
static ConsoleHandler *ch = new ConsoleHandler();
static Level default_level;

Level::Level() {
    this->name = "NULL";
    this->priority = 0;
}

Level::Level(const int &priority, const std::string &name) {
    this->name = name;
    this->priority = priority;
}

bool Level::operator <=(const Level& level) const {
    return priority <= level.priority;
}

bool Level::operator>=(const Level &level) const {
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
    level = default_level;
    handlers = std::vector<Handler*>();
}

Logger::Logger(const std::string &name) : Logger() {
    this->name = name;
}

void Logger::set_level(const Level &level) {
    this->level = level;
}

void Logger::log(const std::string &message, const Level &level) {
    if (level >= this->level) {
        for (auto handler : handlers) {
            handler->log(message, level);
        }
    }
}

void Logger::trace(const std::string &message) {
    log(message, TRACE);
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
    default_level = level;
}

Logger* get_root_logger() {
    Logger *root = get_logger("root");
    root->set_level(INFO);
    return root;
}

Logger* get_logger(const std::string& name) {
    if (loggers.count(name)) {
        return loggers[name];
    }
    Logger *log = new Logger(name);
    loggers.emplace(name, log);
    log->add_handler(ch);
    return log;
}

}