#pragma once

#include <string>
#include "types.h"

namespace PPC {

class Register {

public:

    enum RType { BAD, REGULAR, FLOAT, CONDITION };
    
    uchar number;
    RType type;

    Register();
    Register(const uchar& number, const RType& type);
    Register(const uchar& number, const std::string& type);

    bool operator==(const Register& reg) const;
    bool operator!=(const Register& reg) const;
    bool operator<(const Register& reg) const;
    bool operator>(const Register& reg) const;

};

}