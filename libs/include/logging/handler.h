#pragma once

#include <string>
#include "level.h"

namespace logging {

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

}