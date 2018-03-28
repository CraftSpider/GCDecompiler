#pragma once

#include <string>
#include "../types.h"

namespace PPC {

    class Register {

    public:

        uint number, type;

        enum RType { BAD, REGULAR, FLOAT, CONDITION };

        Register();
        Register(uint number, RType type);
        Register(uint number, std::string type);

        bool operator ==(const Register &reg) const;
        bool operator != (const Register &reg) const;
        bool operator <(const Register &reg) const;

    };

}