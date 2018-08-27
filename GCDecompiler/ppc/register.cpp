
#include <string>

#include "types.h"
#include "register.h"

namespace PPC {

Register::Register() {
    this->number = 0;
    this->type = BAD;
}

Register::Register(const uchar& number, const RType& type) {
    this->number = number;
    this->type = type;
}

Register::Register(const uchar& number, const std::string& type) {
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

bool Register::operator==(const Register& reg) const {
    return this->number == reg.number && this->type == reg.type;
}

bool Register::operator!=(const Register& reg) const {
    return this->number != reg.number || this->type != reg.type;
}

bool Register::operator<(const Register& reg) const {
    if (this->type == reg.type) {
        return this->number < reg.number;
    }
    return this->type < reg.type;
}

bool Register::operator>(const Register& reg) const {
    if (this->type == reg.type) {
        return this->number > reg.number;
    }
    return this->type > reg.type;
}

}