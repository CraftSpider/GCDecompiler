#pragma once

#include <string>

class Formattable {
public:
    virtual std::string __format__(std::string spec) = 0;
};

std::string format(const std::string& pattern, ...);
