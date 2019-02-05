#pragma once

#include <string>

namespace logging {

class Level {
    
    std::string name;
    int priority;

public:
    
    static const Level NO_LEVEL;
    static const Level TRACE;
    static const Level DEBUG;
    static const Level INFO;
    static const Level WARN;
    static const Level ERROR;
    static const Level FATAL;
    
    Level() noexcept;
    Level(const int& priority, const std::string& name) noexcept;
    
    std::string get_name();
    
    bool operator==(const Level& level) const;
    bool operator!=(const Level& level);
    bool operator<=(const Level& level) const;
    bool operator>=(const Level& level) const;
    bool operator>(const Level& level) const;
    bool operator<(const Level& level) const;
    
    explicit operator std::string() const;
    
};

}