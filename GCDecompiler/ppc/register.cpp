
#include <string>

#include "../types.h"
#include "register.h"

using std::string;

namespace PPC {

    Register::Register() {
        this->number = -1;
        this->type = BAD;
    }

    Register::Register(uint number, RType type) {
        this->number = number;
        this->type = type;
    }

    Register::Register(uint number, string type) {
        this->number = number;
        if (type == "r") {
            this->type = REGULAR;
        } else if (type == "fr") {
            this->type = FLOAT;
        } else if (type == "crf") {
            this->type = CONDITION;
        } else {
            this->type = BAD;
        }
    }

    bool Register::operator ==(const Register &reg) const {
        return this->number == reg.number && this->type == reg.type;
    }

    bool Register::operator != (const Register &reg) const {
        return this->number != reg.number || this->type != reg.type;
    }

    bool Register::operator <(const Register &reg) const {
        if (this->type == reg.type) {
            return this->number < reg.number;
        } else {
            return this->type < reg.type;
        }
    }

}